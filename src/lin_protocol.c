#include "lin_protocol.h"
static uint8_t parity(uint8_t x){uint8_t p=0;while(x){p^=x&1u;x>>=1;}return p;}
uint8_t lin_make_protected_id(uint8_t id){id&=0x3Fu;uint8_t p0=((id>>0)^(id>>1)^(id>>2)^(id>>4))&1u;uint8_t p1=(uint8_t)(~((id>>1)^(id>>3)^(id>>4)^(id>>5)))&1u;return (uint8_t)(id|(p0<<6)|(p1<<7));}
bool lin_validate_protected_id(uint8_t pid){return (pid&0x3Fu)==(lin_make_protected_id(pid&0x3Fu)&0x3Fu)&&pid==lin_make_protected_id(pid&0x3Fu);}
uint8_t lin_checksum(const uint8_t*d,uint8_t n,bool enhanced,uint8_t pid){uint16_t sum=enhanced?pid:0;for(uint8_t i=0;i<n;i++){sum+=d[i];if(sum>255)sum=(sum&255u)+1u;}return (uint8_t)(~sum);}
lin_status_t lin_encode_frame(lin_frame_t*f,uint8_t id,const uint8_t*d,uint8_t n,bool enhanced){if(!f||n>LIN_MAX_DATA||(!d&&n)||id>0x3Fu)return LIN_ERR_ARGUMENT;f->id=id;f->pid=lin_make_protected_id(id);f->dlc=n;for(uint8_t i=0;i<n;i++)f->data[i]=d[i];f->checksum=lin_checksum(f->data,n,enhanced,f->pid);return LIN_OK;}
lin_status_t lin_validate_frame(const lin_frame_t*f,bool enhanced){if(!f||f->id>0x3Fu||f->dlc>LIN_MAX_DATA)return LIN_ERR_ARGUMENT;if(!lin_validate_protected_id(f->pid))return LIN_ERR_PID;if(lin_checksum(f->data,f->dlc,enhanced,f->pid)!=f->checksum)return LIN_ERR_CHECKSUM;return LIN_OK;}
