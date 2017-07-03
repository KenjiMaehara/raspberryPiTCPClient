/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h> 
#include <mytype.h>
#include "port.h"


void port_init_direction(void)
{
	/*in-out setup */
	PORTA.DIR = 0x00;
	PORTB.DIR = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm;
	PORTC.DIR = PIN0_bm | PIN4_bm | PIN5_bm | PIN7_bm;
	PORTD.DIR = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm  | PIN5_bm;
	PORTE.DIR = 0x00;
	PORTF.DIR = 0x00;
	
	
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


}

void port_init_value(void)
{
	PORTB.OUTCLR = PIN0_bm;
	PORTB.OUTCLR = PIN1_bm;
	PORTB.OUTCLR = PIN2_bm;
	PORTB.OUTCLR = PIN3_bm;
	
	PORTC.OUTSET = PIN4_bm;
	PORTC.OUTCLR = PIN5_bm;
	PORTC.OUTCLR = PIN7_bm;

	PORTD.OUTCLR = PIN0_bm;
	PORTD.OUTCLR = PIN1_bm;
	PORTD.OUTCLR = PIN2_bm;
	PORTD.OUTCLR = PIN3_bm;
	PORTD.OUTCLR = PIN4_bm;
	
}

void port_init(void)
{	
	port_init_direction();
	port_init_value();
}

u8 get_door_lock_test(void)
{
	return (PORTA.IN & PIN2_bm) ? false : true;
}


u8 get_door_unlock_test(void)
{
	return (PORTA.IN & PIN1_bm) ? false : true;
}

u8 get_extern_door_lock(void)
{
	//return (PORTA.IN & PIN3_bm) ? false : true;
	return (PORTA.IN & PIN3_bm) ? true : false;
}

u8 get_internal_door_lock(void)
{
	//return (PORTA.IN & PIN4_bm) ? false : true;
	return (PORTA.IN & PIN4_bm) ? true : false;
}


void set_buzzer(u8 set)
{
	if(set == true)
		PORTC.OUTSET = PIN0_bm;
	else		
		PORTC.OUTCLR = PIN0_bm;
}

void set_room_light(u8 set)
{
	if(set == true)
		PORTB.OUTSET = PIN1_bm;
	else
		PORTB.OUTCLR = PIN1_bm;
}

void set_cabin_leftdoor(DOOR_OPER oper)
{
	if(oper == OPEN)
	{
		PORTB.OUTSET = PIN3_bm;
		PORTB.OUTCLR = PIN2_bm;
	}
	else if(oper == CLOSE)
	{
		PORTB.OUTSET = PIN2_bm;
		PORTB.OUTCLR = PIN3_bm;
	}
	else if(oper == STOP)
	{
		PORTB.OUTSET = PIN2_bm;
		PORTB.OUTSET = PIN3_bm;
	}
	else
	{
		PORTB.OUTCLR = PIN2_bm;
		PORTB.OUTCLR = PIN3_bm;
	}
}

u8 get_cabin_door_sw(void)
{
	return (PORTC.IN & PIN1_bm) ? false : true;
}

u8 get_gdoa(void)
{
	return (PORTA.IN & PIN7_bm) ? true : false;
}

u8 get_gdo2(void)
{
	return (PORTC.IN & PIN3_bm) ? true : false;
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

void set_power_led(u8 onoff)
{
	if(onoff == true)
		PORTD.OUTSET = PIN0_bm;
	else
		PORTD.OUTCLR = PIN0_bm;
}

void set_communication_led(u8 onoff)
{
	if(onoff == true)
		PORTD.OUTSET = PIN1_bm;
	else
		PORTD.OUTCLR = PIN1_bm;
}

void set_out_door_state_led(u8 state)
{
	if(state == true)
		PORTD.OUTSET = PIN2_bm;
	else
		PORTD.OUTCLR = PIN2_bm;
}

void set_in_door_state_led(u8 state)
{
	if(state == true)
		PORTD.OUTSET = PIN3_bm;
	else
		PORTD.OUTCLR = PIN3_bm;
}

void set_unlock_led(u8 unlock)
{
	if(unlock == true)
		PORTD.OUTSET = PIN4_bm;
	else
		PORTD.OUTCLR = PIN4_bm;
}

void set_lock_led(u8 lock)
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
