/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h> 
#include <mytype.h>

#include "port.h"
#include "menu.h"
#include "Oper.h"


void port_init_direction(void)
{
	/*in-out setup */
	PORTA.DIR = 0xfc;
	//PORTB.DIR = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
	PORTB.DIR = PIN7_bm;
	//PORTC.DIR = PIN0_bm |PIN4_bm | PIN5_bm | PIN7_bm;
<<<<<<< HEAD
<<<<<<< HEAD
	PORTC.DIR = 0x80;
=======
	PORTC.DIR = 0x88;
	//PORTC.DIR = 0x80;
>>>>>>> parent of f8ff870... test063003
=======
	PORTC.DIR = 0x88;
	//PORTC.DIR = 0x80;
>>>>>>> parent of f8ff870... test063003
	PORTD.DIR = PIN1_bm;
	PORTE.DIR = PIN1_bm | PIN5_bm;
	PORTF.DIR = PIN1_bm | PIN6_bm;
	PORTH.DIR = 0x00;
	PORTJ.DIR = 0x00;
	PORTK.DIR = PIN7_bm;
	PORTQ.DIR = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm;
	
	PORTE.PIN2CTRL = 0x18; // pullup , both edge
	
	PORTF.PIN5CTRL = 0x18;  // pullup , both edge
	PORTF.PIN6CTRL = 0x18;  // pullup , both edge
	//PORTE.REMAP  |= PORT_SPI_bm;
	//PORTE.REMAP |= PORT_SPI_bm;
	//PORTE.reserved_0x0E |= 0x20;	
	
	// adc setup

	
	//PORTA.PIN0CTRL = 7;	

	
}

void port_init_value(void)
{
	//PORTA.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
	#if 0
	PORTB.OUT = 0x00;
	
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
	#endif
}

void port_init(void)
{	
	port_init_direction();
	port_init_value();
}





u8 get_ch1_Input(void)
{
	return (PORTH.IN & PIN0_bm) ? false : true;
}


u8 get_ch2_Input(void)
{
	return (PORTH.IN & PIN1_bm) ? false : true;
}


u8 get_ch3_Input(void)
{
	return (PORTH.IN & PIN2_bm) ? false : true;
}


u8 get_ch4_Input(void)
{
	return (PORTH.IN & PIN3_bm) ? false : true;
}


u8 get_ch5_Input(void)
{
	return (PORTH.IN & PIN4_bm) ? false : true;
}


u8 get_ch6_Input(void)
{
	return (PORTH.IN & PIN5_bm) ? false : true;
}


u8 get_ch7_Input(void)
{
	return (PORTH.IN & PIN6_bm) ? false : true;
}

u8 get_ch8_Input(void)
{
	return (PORTH.IN & PIN7_bm) ? false : true;
}


u8 get_ch9_Input(void)
{
	return (PORTJ.IN & PIN0_bm) ? false : true;
}


u8 get_ch10_Input(void)
{
	return (PORTJ.IN & PIN1_bm) ? false : true;
}


u8 get_ch11_Input(void)
{
	return (PORTJ.IN & PIN2_bm) ? false : true;
}



u8 get_ch12_Input(void)
{
	return (PORTJ.IN & PIN3_bm) ? false : true;
}



u8 get_ch13_Input(void)
{
	return (PORTJ.IN & PIN4_bm) ? false : true;
}



u8 get_ch14_Input(void)
{
	return (PORTJ.IN & PIN5_bm) ? false : true;
}


u8 get_ch15_Input(void)
{
	return (PORTJ.IN & PIN6_bm) ? false : true;
}



u8 get_ch16_Input(void)
{
	return (PORTJ.IN & PIN7_bm) ? false : true;
}


u8 get_ch17_Input(void)
{
	return (PORTK.IN & PIN0_bm) ? false : true;
}


u8 get_ch18_Input(void)
{
	return (PORTK.IN & PIN1_bm) ? false : true;
}


u8 get_sr_key_sw(void)
{
	return (PORTK.IN & PIN2_bm) ? false : true;
}


u8 get_tampa_sw(void)
{
	return (PORTK.IN & PIN3_bm) ? false : true;
}



u8 get_BattLow(void)
{
	return (PORTK.IN & PIN4_bm) ? false : true;
}


u8 get_PowerBlackout(void)
{
	return (PORTK.IN & PIN5_bm) ? false : true;
}


u8 get_BattFull(void)
{
	return (PORTK.IN & PIN6_bm) ? false : true;
}



void set_Router_reset(u8 set)
{
	if(set == true)
		PORTK.OUTSET = PIN7_bm;
	else
		PORTK.OUTCLR = PIN7_bm;
}



void set_DTMF_MSG_Select(u8 set)
{
	if(set == true)
		PORTB.OUTSET = PIN7_bm;
	else
		PORTB.OUTCLR = PIN7_bm;
}



void set_ESP_Reset(u8 set)
{
	if(set == true)
		PORTD.OUTSET = PIN1_bm;
	else
		PORTD.OUTCLR = PIN1_bm;
}



void set_Relay1(u8 set)
{
	if(set == true)
		PORTQ.OUTSET = PIN0_bm;
	else
		PORTQ.OUTCLR = PIN0_bm;
}



void set_Relay2(u8 set)
{
	if(set == true)
	PORTQ.OUTSET = PIN1_bm;
	else
	PORTQ.OUTCLR = PIN1_bm;
}



void set_Relay3(u8 set)
{
	if(set == true)
		PORTQ.OUTSET = PIN2_bm;
	else
		PORTQ.OUTCLR = PIN2_bm;
}



void set_Relay4(u8 set)
{
	if(set == true)
		PORTQ.OUTSET = PIN3_bm;
	else
		PORTQ.OUTCLR = PIN3_bm;
}







void set_spi_ss(u8 set)
{
	if(set == true)
		PORTF.OUTSET = PIN4_bm;
	else
		PORTF.OUTCLR = PIN4_bm;
}

u8 get_spi_ss(void)
{
	return (PORTF.IN & PIN4_bm) ? true : false;
}



void set_spi_mosi(u8 set)
{
	if(set == true)
		PORTE.OUTSET = PIN5_bm;
	else
		PORTE.OUTCLR = PIN5_bm;
}

u8 get_miso(void)
{
	return (PORTE.IN & PIN6_bm) ? false : true;
}


void set_spi_sck(u8 set)
{
	if(set == true)
		PORTE.OUTSET = PIN7_bm;
	else
		PORTE.OUTCLR = PIN7_bm;
}
















u8 get_pstn_L_revers_pos(void)
{
	return (PORTA.IN & PIN1_bm) ? true : false;
}



u8 get_pstn_L_revers_nos(void)
{
	return (PORTA.IN & PIN2_bm) ? true : false;
}


void set_pstn_reset(u8 set)
{
	if(set == true)
		PORTA.OUTSET = PIN3_bm;
	else
		PORTA.OUTCLR = PIN3_bm;
}



void set_pstn_HLD_Pdial(u8 set)
{
	if(set == true)
		PORTA.OUTSET = PIN4_bm;
	else
		PORTA.OUTCLR = PIN4_bm;
}


void set_pstn_LNchange(u8 set)
{
	if(set == true)
		PORTA.OUTSET = PIN5_bm;
	else
		PORTA.OUTCLR = PIN5_bm;
}





void set_pstn_DataLatch(u8 set)
{
	if(set == true)
		PORTA.OUTSET = PIN6_bm;
	else
		PORTA.OUTCLR = PIN6_bm;
}


void set_pstn_ReceiveDataEnable(u8 set)
{
	if(set == true)
		PORTA.OUTSET = PIN7_bm;
	else
		PORTA.OUTCLR = PIN7_bm;
}




u8 get_pstn_DET_Dial_RingBack_BusyTone(void)
{
	return (PORTB.IN & PIN4_bm) ? true : false;
}



u8 get_pstn_DV_DataReciveValid(void)
{
	return (PORTB.IN & PIN5_bm) ? true : false;
}


u8 get_pstn_BELL(void)
{
	return (PORTB.IN & PIN6_bm) ? true : false;
}



void set_pstn_DM_SEL(u8 set)
{
	if(set == true)
		PORTB.OUTSET = PIN7_bm;
	else
		PORTB.OUTCLR = PIN7_bm;
}







#if 0




#endif



