#include "lin_master.h"
#include "lin_sim.h"
#include <string.h>

void lin_master_init(lin_master_t *m,uint32_t timeout_us) {
    if(!m) return;
    memset(m,0,sizeof(*m));
    m->timeout_us=timeout_us;
    lin_schedule_init(&m->schedule);
}
lin_status_t lin_master_add_slave(lin_master_t *m,lin_slave_t s) {
    if(!m) return LIN_ERR_ARGUMENT;
    if(m->slave_count>=LIN_MAX_SLAVES) return LIN_ERR_CAPACITY;
    m->slaves[m->slave_count++]=s;
    return LIN_OK;
}
lin_status_t lin_master_add_slot(lin_master_t *m,lin_schedule_slot_t s) {
    return m?lin_schedule_add(&m->schedule,s):LIN_ERR_ARGUMENT;
}
lin_status_t lin_master_tick(lin_master_t *m,lin_frame_t *tx,lin_frame_t *rx,bool *received) {
    if(!m||!tx||!rx||!received) return LIN_ERR_ARGUMENT;
    const lin_schedule_slot_t *slot=lin_schedule_current(&m->schedule);
    if(!slot) return LIN_ERR_NO_SLOT;
    m->now_us+=(uint64_t)slot->delay_ms*1000ULL;
    uint8_t command[LIN_MAX_DATA]={0};
    lin_status_t st;
    if(slot->type==LIN_SLOT_DIAGNOSTIC) {
        command[0]=0x03; command[1]=0x22; command[2]=0xF1; command[3]=0x90;
        st=lin_encode_frame(tx,0x3C,command,4,false);
        m->stats.diagnostic_requests++;
    } else {
        st=lin_encode_frame(tx,slot->frame_id,command,0,true);
    }
    if(st!=LIN_OK) return st;
    tx->timestamp_us=m->now_us;
    m->stats.frames_sent++;
    *received=false;
    if(slot->type==LIN_SLOT_DIAGNOSTIC) {
        /* This slot represents a diagnostic request; response is not required in this demo. */
        st=LIN_OK;
    } else if(slot->slave_index<m->slave_count) {
        lin_slave_t *slave=&m->slaves[slot->slave_index];
        lin_sim_respond(slave,tx,rx,received,m->now_us);
        if(*received) {
            if(rx->timestamp_us<m->now_us || rx->timestamp_us-m->now_us>m->timeout_us) {
                *received=false; m->stats.timeouts++; st=LIN_ERR_TIMEOUT;
            } else {
                st=lin_validate_frame(rx,true);
                if(st==LIN_OK) m->stats.frames_received++;
                else if(st==LIN_ERR_CHECKSUM) m->stats.checksum_errors++;
                else if(st==LIN_ERR_PID) m->stats.pid_errors++;
            }
        } else {
            m->stats.timeouts++; st=LIN_ERR_TIMEOUT;
        }
    } else {
        m->stats.timeouts++; st=LIN_ERR_TIMEOUT;
    }
    if(st==LIN_ERR_TIMEOUT) m->stats.recovered_slots++;
    lin_schedule_advance(&m->schedule,m->now_us);
    return st;
}
lin_status_t lin_master_diagnostic_request(lin_master_t *m,uint8_t idx,uint8_t service,const uint8_t *data,uint8_t n,lin_frame_t *tx) {
    if(!m||!tx||idx>=m->slave_count||n>5||(!data&&n)) return LIN_ERR_ARGUMENT;
    uint8_t payload[LIN_MAX_DATA]={0};
    payload[0]=(uint8_t)(1u+n); payload[1]=service;
    for(uint8_t i=0;i<n;i++) payload[2+i]=data[i];
    m->stats.diagnostic_requests++;
    return lin_encode_frame(tx,0x3C,payload,(uint8_t)(2+n),false);
}
