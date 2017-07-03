/*
 * 
 *
 * 
 * 
 */ 


#ifndef PORT_H_
#define PORT_H_

#include <mytype.h>

typedef enum 
{
	OPEN,
	CLOSE,
	STOP,
	STOP2

}DOOR_OPER;


void port_init(void);
u8 get_door_lock_test(void);
u8 get_door_unlock_test(void);
u8 get_extern_door_lock(void);
u8 get_internal_door_lock(void);

u8 get_door_unlock(void);
u8 get_door_lock(void);
void set_buzzer(u8 set);
void set_room_light(u8 set);
void set_cabin_leftdoor(DOOR_OPER oper);
u8 get_cabin_door_sw(void);
u8 get_gdoa(void);
u8 get_gdo2(void);
void set_power_led(u8 onoff);
void set_communication_led(u8 onoff);
void set_out_door_state_led(u8 state);
void set_in_door_state_led(u8 state);
void set_unlock_led(u8 unlock);
void set_lock_led(u8 lock);
u8 get_test(void);
u8 get_freq(void);
u8 get_dly(void);


void set_spi_ss(u8 set);
u8 get_spi_ss(void);
void set_spi_mosi(u8 set);
u8 get_miso(void);
void set_spi_sck(u8 set);


#endif /* PORT_H_ */