/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h>
#include <includes.h>
#include "port.h"
#include "env.h"
#include "event.h"
#include "Oper.h"
#include "rf_task.h"
#include "menu.h"
#include "key.h"
#include "spi.h"





u32	Os_GetTime_for_DoorOpen_Resettimeout=0;


#define CMD_WAIT	1
#define CMD_READ	2
#define CMD_WRITE	3

#define M16_EXBOARD_INPUT_SIG_STATUS                       0x00



u8 chInputStatus1(void)
{
	
	
	
}




void EventTask(void *p_arg)
{
	volatile testCount=0;
	volatile u8 testCharacter[100];
	volatile u8 test=0;
	volatile u8 rxdata=0;
	u8 state = CMD_WAIT;
	int cnt = 0;
	u8 dummy;

	while(1)
	{
	
	
		if(SPIF.STATUS & 0x80)
		{

			rxdata = SPIF.DATA;

			
			switch(state)
			{
				case CMD_WAIT:
				
					
					if(!(rxdata & 0x40))
					{
						state = CMD_WRITE;
					}
					else if(rxdata & 0x40)
					{
						state = CMD_READ;
					}
				
				
					break;
				case CMD_WRITE:
				
					test = rxdata;
					SPIF.DATA = 0x55;
					while(!(SPIF.STATUS & 0x80));
					
					state = CMD_WAIT;
									
					break;
				case CMD_READ:
					
					test = (rxdata & 0x0f);
					
					
					if(test == M16_EXBOARD_INPUT_SIG_STATUS)
					{
						
					}
					
					
					//WAIT_EORX();
					SPIF.DATA = get_ch18_Input();
					while(!(SPIF.STATUS & 0x80));
					//WAIT_EORX();
					
					
					state = CMD_WAIT;
				
					break;
				default:
					state = CMD_WAIT;
			}
			
			
			//dummy = SPIF.DATA;
			
		}
		

		
		test=1;
		
	}



}

