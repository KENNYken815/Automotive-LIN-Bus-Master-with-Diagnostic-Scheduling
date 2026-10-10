#include <assert.h>
#include <stdio.h>
#include "lin_master.h"
#include "lin_sim.h"
int main(void){uint8_t data[]={1,2,3};lin_frame_t f={0};assert(lin_encode_frame(&f,0x12,data,3,true)==LIN_OK);assert(lin_validate_frame(&f,true)==LIN_OK);f.pid^=0x40;assert(lin_validate_frame(&f,true)==LIN_ERR_PID);lin_master_t m;lin_master_init(&m,2000);lin_master_add_slave(&m,(lin_slave_t){.node_id=0x10,.kind=LIN_SLAVE_BODY,.present=true,.response_delay_us=300});lin_master_add_slot(&m,(lin_schedule_slot_t){0x10,LIN_SLOT_UNCONDITIONAL,10,3,0});lin_master_add_slot(&m,(lin_schedule_slot_t){0x11,LIN_SLOT_UNCONDITIONAL,10,3,1});lin_frame_t tx={0},rx={0};bool got=false;assert(lin_master_tick(&m,&tx,&rx,&got)==LIN_OK&&got);assert(lin_master_tick(&m,&tx,&rx,&got)==LIN_ERR_TIMEOUT&&!got);assert(m.stats.recovered_slots==1);lin_frame_t diag={0};assert(lin_master_diagnostic_request(&m,0,0x22,data,2,&diag)==LIN_OK&&diag.id==0x3C);puts("LIN tests passed.");return 0;}
