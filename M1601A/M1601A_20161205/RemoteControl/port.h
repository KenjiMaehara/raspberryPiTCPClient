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
	STOP2,

}DOOR_OPER;


void port_init(void);
u8 get_voice_busy(void);
u8 get_emergency_button(void);
u8 get_cab_unlocked(void);
u8 get_rear_door_unlocked(void);
u8 get_side_door_unlocked(void);
u8 get_security_set(void);
u8 get_security_reset(void);
void set_voice(u8 voice);
void set_buzzer(u8 set);
u8 get_gdoa(void);
u8 get_gdo2(void);

void set_communication_led(u8 onoff);
void set_bat_led(u8 onoff);
void set_car_emergency_led(u8 state);
void set_emg_led(u8 unlock);
void set_emg_button_led(u8 lock);
void set_security_mode_led(u8 lock);

u8 get_test(void);
u8 get_freq(void);
u8 get_dly(void);


void set_spi_ss(u8 set);
u8 get_spi_ss(void);
void set_spi_mosi(u8 set);
u8 get_miso(void);
void set_spi_sck(u8 set);


#endif /* PORT_H_ */