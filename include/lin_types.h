#ifndef LIN_TYPES_H
#define LIN_TYPES_H
#include <stdint.h>
#include <stdbool.h>
#define LIN_MAX_DATA 8u
#define LIN_MAX_SLOTS 16u
#define LIN_MAX_SLAVES 8u
typedef enum { LIN_OK=0, LIN_ERR_ARGUMENT=-1, LIN_ERR_PID=-2, LIN_ERR_CHECKSUM=-3, LIN_ERR_TIMEOUT=-4, LIN_ERR_NO_SLOT=-5, LIN_ERR_CAPACITY=-6 } lin_status_t;
typedef enum { LIN_SLOT_UNCONDITIONAL=0, LIN_SLOT_DIAGNOSTIC=1 } lin_slot_type_t;
typedef enum { LIN_SLAVE_BODY=0, LIN_SLAVE_THERMAL=1, LIN_SLAVE_DOOR=2 } lin_slave_kind_t;
typedef struct { uint8_t id; uint8_t pid; uint8_t data[LIN_MAX_DATA]; uint8_t dlc; uint8_t checksum; uint64_t timestamp_us; } lin_frame_t;
typedef struct { uint8_t frame_id; lin_slot_type_t type; uint16_t delay_ms; uint8_t expected_dlc; uint8_t slave_index; } lin_schedule_slot_t;
typedef struct { uint32_t frames_sent, frames_received, checksum_errors, pid_errors, timeouts, recovered_slots, diagnostic_requests; } lin_stats_t;
typedef struct { uint8_t node_id; lin_slave_kind_t kind; bool present; uint32_t response_delay_us; uint16_t value; uint8_t last_command; } lin_slave_t;
#endif
