#ifndef LIN_SCHEDULE_H
#define LIN_SCHEDULE_H
#include "lin_types.h"
typedef struct { lin_schedule_slot_t slots[LIN_MAX_SLOTS]; uint8_t count; uint8_t index; uint32_t cycle_count; uint64_t next_deadline_us; } lin_schedule_t;
void lin_schedule_init(lin_schedule_t *schedule);
lin_status_t lin_schedule_add(lin_schedule_t *schedule,lin_schedule_slot_t slot);
const lin_schedule_slot_t *lin_schedule_current(lin_schedule_t *schedule);
void lin_schedule_advance(lin_schedule_t *schedule,uint64_t now_us);
#endif
