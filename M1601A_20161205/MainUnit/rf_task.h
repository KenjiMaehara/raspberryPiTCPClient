/*
 * 
 *
 * 
 * 
 */ 


#ifndef RF_TASK_H_
#define RF_TASK_H_

#include <mytype.h>


u8 rf_door_open_oper(u32 id);
u8 rf_send_emg_door(u8 emg);

extern u8 driving_state_door_open_flag;
extern packet_t gPacket;

#endif /* RF_TASK_H_ */