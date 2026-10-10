#ifndef LIN_MASTER_H
#define LIN_MASTER_H
#include "lin_schedule.h"
#include "lin_protocol.h"
typedef struct { lin_schedule_t schedule; lin_slave_t slaves[LIN_MAX_SLAVES]; uint8_t slave_count; lin_stats_t stats; uint32_t timeout_us; uint64_t now_us; } lin_master_t;
void lin_master_init(lin_master_t *master,uint32_t timeout_us);
lin_status_t lin_master_add_slave(lin_master_t *master,lin_slave_t slave);
lin_status_t lin_master_add_slot(lin_master_t *master,lin_schedule_slot_t slot);
lin_status_t lin_master_tick(lin_master_t *master,lin_frame_t *tx,lin_frame_t *rx,bool *received);
lin_status_t lin_master_diagnostic_request(lin_master_t *master,uint8_t slave_index,uint8_t service,const uint8_t *data,uint8_t length,lin_frame_t *tx);
#endif
