#include "lin_schedule.h"
void lin_schedule_init(lin_schedule_t*s){if(s){s->count=0;s->index=0;s->cycle_count=0;s->next_deadline_us=0;}}
lin_status_t lin_schedule_add(lin_schedule_t*s,lin_schedule_slot_t slot){if(!s)return LIN_ERR_ARGUMENT;if(s->count>=LIN_MAX_SLOTS||slot.frame_id>0x3Fu||slot.expected_dlc>LIN_MAX_DATA)return LIN_ERR_CAPACITY;s->slots[s->count++]=slot;return LIN_OK;}
const lin_schedule_slot_t*lin_schedule_current(lin_schedule_t*s){if(!s||!s->count)return 0;return &s->slots[s->index];}
void lin_schedule_advance(lin_schedule_t*s,uint64_t now){if(!s||!s->count)return;s->index++;if(s->index>=s->count){s->index=0;s->cycle_count++;}s->next_deadline_us=now+(uint64_t)s->slots[s->index].delay_ms*1000ULL;}
