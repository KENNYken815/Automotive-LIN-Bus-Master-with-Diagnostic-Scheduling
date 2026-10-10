#ifndef LIN_SIM_H
#define LIN_SIM_H
#include "lin_master.h"
void lin_sim_respond(lin_slave_t *slave,const lin_frame_t *request,lin_frame_t *response,bool *has_response,uint64_t now_us);
#endif
