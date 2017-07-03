/*
 * uart.h
 *
 * Created: 2016-09-30 10:04:32
 *  Author: kmaehara
 */ 


#ifndef UART_H_
#define UART_H_



void uart_init(void)
{
	
	
	//USARTC0 for Wireless master Device communication
	
	//PORTC.PIN3CTRL = PORT_OPC_WIREDANDPULL_gc;
	
	USARTC0_BAUDCTRLB = 0x50;	//BSCALE is 0
	USARTC0_BAUDCTRLA = 12;	//BSEL		4.902khz
	

	//Disable interrupts, just for safety
	USARTC0_CTRLC = 0;
	//8 data bits, no parity and 1 stop bit
	USARTC0_CTRLC = USART_CHSIZE_8BIT_gc;
	
	//Enable receive and transmit
	USARTC0_CTRLB = USART_TXEN_bm; // And enable high speed mode

	
	//USARTC1




}




void sendChar(char c)
{
	
	while( !(USARTC0_STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
	
	USARTC0_DATA = c;
	
}




#endif /* UART_H_ */