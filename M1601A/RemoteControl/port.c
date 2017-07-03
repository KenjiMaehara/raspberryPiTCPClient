/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h> 
#include <mytype.h>
#include <util/delay.h>
#include "port.h"


void port_init_direction(void)
{
	/*in-out setup */
	PORTA.DIR = 0x00;
	PORTB.DIR = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
	PORTC.DIR = PIN0_bm | PIN1_bm |PIN4_bm | PIN5_bm | PIN7_bm;
	PORTD.DIR = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN7_bm;
	PORTE.DIR = 0x00;
	PORTF.DIR = 0x00;
	
	PORTC.PIN2CTRL = 0x18; // pullup , both edge
	
	PORTA.PIN3CTRL = 0x18; // pullup , both edge
	PORTA.PIN4CTRL = 0x18; // pullup , both edge
	PORTA.PIN5CTRL = 0x18; // pullup , both edge
	PORTA.PIN6CTRL = 0x18; // pullup , both edge
	PORTA.PIN7CTRL = 0x18; // pullup , both edge

	PORTC.PIN1CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg	
	
	PORTF.PIN0CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg	
	PORTF.PIN1CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg	
	PORTF.PIN2CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg	
	PORTF.PIN3CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg	
	PORTF.PIN4CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg
	PORTF.PIN5CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg
	PORTF.PIN6CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg
	PORTF.PIN7CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg
	
	
	PORTE.PIN0CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg
	PORTE.PIN1CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg
	PORTE.PIN2CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg
	PORTE.PIN3CTRL |= PORT_OPC_PULLUP_gc; // pullup , both edg
	
	

	// adc setup
	PORTA.PIN0CTRL = 7;	
	
	
}

void port_init_value(void)
{
	PORTB.OUT = 0xff;
	
	PORTC.OUTCLR = PIN0_bm;
	PORTC.OUTSET = PIN4_bm;
	PORTC.OUTCLR = PIN5_bm;
	PORTC.OUTCLR = PIN7_bm;

	PORTD.OUTCLR = PIN0_bm;
	PORTD.OUTCLR = PIN1_bm;
	PORTD.OUTCLR = PIN2_bm;
	PORTD.OUTCLR = PIN3_bm;
	PORTD.OUTCLR = PIN4_bm;
	PORTD.OUTCLR = PIN5_bm;
	PORTD.OUTSET = PIN7_bm;
	
}

void port_init(void)
{	
	port_init_direction();
	port_init_value();
}


u8 get_voice_busy(void)
{
	return (PORTA.IN & PIN1_bm) ? true : false;
}

u8 get_emergency_button(void)
{
	return (PORTC.IN & PIN2_bm) ? false : true;
}

u8 get_cab_unlocked(void)
{
	return (PORTA.IN & PIN3_bm) ? false : true;
}

u8 get_rear_door_unlocked(void)
{
	return (PORTA.IN & PIN4_bm) ? false : true;
}

u8 get_side_door_unlocked(void)
{
	return (PORTA.IN & PIN5_bm) ? false : true;
}

u8 get_security_set(void)
{
	return (PORTA.IN & PIN6_bm) ? false : true;
}

u8 get_security_reset(void)
{
	return (PORTA.IN & PIN7_bm) ? false : true;
}



void set_voice(u8 voice)
{
	volatile int count;
	
	if(voice == 0xff)	
	{
		PORTB.OUT = 0xff;
		return ;	
	}
	else
		PORTB.OUT = ~(0x01 << (voice-1));
	
	count = 0;	
	while(1)
	{
		if(get_voice_busy() == true)
		{
			PORTB.OUT = 0xff;
			break;
			
		}
		_delay_ms(1);
		if(count++ > 1000)
			break;
	}
}



void set_speaker_power(u8 set)
{
	if(set == true)
	PORTC.OUTSET = PIN0_bm;
	else
	PORTC.OUTCLR = PIN0_bm;
}



void set_buzzer(u8 set)
{
	if(set == true)
		PORTC.OUTSET = PIN1_bm;
	else
		PORTC.OUTCLR = PIN1_bm;
}



u8 get_gdoa(void)
{
	return (PORTA.IN & 0x04) ? true : false;
}

u8 get_gdo2(void)
{
	return (PORTC.IN & 0x08) ? true : false;
}

void set_spi_ss(u8 set)
{
	if(set == true)
		PORTC.OUTSET = PIN4_bm;
	else
		PORTC.OUTCLR = PIN4_bm;
}

u8 get_spi_ss(void)
{
	return (PORTC.IN & PIN4_bm) ? true : false;
}


void set_spi_mosi(u8 set)
{
	if(set == true)
		PORTC.OUTSET = PIN5_bm;
	else
		PORTC.OUTCLR = PIN5_bm;
}

u8 get_miso(void)
{
	return (PORTC.IN & PIN6_bm) ? false : true;
}


void set_spi_sck(u8 set)
{
	if(set == true)
		PORTC.OUTSET = PIN7_bm;
	else
		PORTC.OUTCLR = PIN7_bm;
}

void set_communication_led(u8 onoff)
{
	if(onoff == true)
		PORTD.OUTSET = PIN0_bm;
	else
		PORTD.OUTCLR = PIN0_bm;
}

void set_bat_led(u8 onoff)
{
	if(onoff == true)
		PORTD.OUTSET = PIN1_bm;
	else
		PORTD.OUTCLR = PIN1_bm;
}

void set_car_emergency_led(u8 state)
{
	if(state == true)
		PORTD.OUTSET = PIN2_bm;
	else
		PORTD.OUTCLR = PIN2_bm;
}

void set_emg_led(u8 unlock)
{
	if(unlock == true)
		PORTD.OUTSET = PIN3_bm;
	else
		PORTD.OUTCLR = PIN3_bm;
}

void set_emg_button_led(u8 lock)
{
	if(lock == true)
		PORTD.OUTSET = PIN4_bm;
	else
		PORTD.OUTCLR = PIN4_bm;
}

void set_security_mode_led(u8 lock)
{
	if(lock == true)
	PORTD.OUTSET = PIN5_bm;
	else
	PORTD.OUTCLR = PIN5_bm;
}

u8 get_test(void)
{
	return PORTE.IN & 0x0f;
}



u8 get_freq(void)
{
	return PORTF.IN & 0x0f;
}

u8 get_dly(void)
{
	return (PORTF.IN & 0xf0) >> 4;
}
