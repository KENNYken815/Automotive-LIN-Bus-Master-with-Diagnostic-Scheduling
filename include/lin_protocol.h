#ifndef LIN_PROTOCOL_H
#define LIN_PROTOCOL_H
#include "lin_types.h"
uint8_t lin_make_protected_id(uint8_t frame_id);
bool lin_validate_protected_id(uint8_t pid);
uint8_t lin_checksum(const uint8_t *data,uint8_t length,bool enhanced,uint8_t pid);
lin_status_t lin_encode_frame(lin_frame_t *frame,uint8_t frame_id,const uint8_t *data,uint8_t length,bool enhanced);
lin_status_t lin_validate_frame(const lin_frame_t *frame,bool enhanced);
#endif
