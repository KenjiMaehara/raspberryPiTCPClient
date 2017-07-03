/*
 * 
 *
 * 
 * 
 */ 


#ifndef EVENT_H_
#define EVENT_H_

#include <mytype.h>


static u8 g_state;
static u8 door_unlock_flag;
extern u8 temp_g_state;
extern u8 emg_s_b_door_state;
extern u8 set_wait_reset_state_cancel_flag;

extern u32 Os_GetTime_for_DoorOpen_Resettimeout;

extern void drive_security_door_double_lock(void);

#endif /* EVENT_H_ */