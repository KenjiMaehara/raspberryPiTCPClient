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
#define CMD_DATA_SEND	4

#define REG_M16_EXBOARD_INPUT_ALL_SIG_STATUS			0x00
#define REG_RELAY_1										0x01
#define REG_RELAY_2										0x02
#define REG_RELAY_3										0x03
#define REG_RELAY_4										0x04
#define REG_RELAY_5										0x04
#define INPUT_CH1										0x05
#define INPUT_CH2										0x06
#define INPUT_CH3										0x07
#define INPUT_CH4										0x08
#define INPUT_CH5										0x09
#define INPUT_CH6										0x0A
#define INPUT_CH7										0x0B
#define INPUT_CH8										0x0C
#define INPUT_CH9										0x0D
#define INPUT_CH10										0x0E
#define INPUT_CH11										0x0F
#define INPUT_CH12										0x10
#define INPUT_CH13										0x11
#define INPUT_CH14										0x12
#define INPUT_CH15										0x13
#define INPUT_CH16										0x14
#define INPUT_CH17										0x15
#define INPUT_CH18										0x16
#define INPUT_SR										0x17




u8 chInputStatus1(void)
{
	
	
	
}




void EventTask(void *p_arg)
{
	volatile testCount=0;
	volatile u8 testCharacter[100];
	volatile u8 cmd_data[20];
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

			cmd_data[cnt++] = rxdata;
			
			if(cnt > 2)
			{
				if(cmd_data[0]==CMD_WRITE)
				{
					switch(cmd_data[1])
					{
						case REG_RELAY_1:
							set_Relay1(cmd_data[2]);
							break;
						case REG_RELAY_2:
							set_Relay2(cmd_data[2]);
							break;
						case REG_RELAY_3:
							set_Relay3(cmd_data[2]);
							break;
						case REG_RELAY_4:
							set_Relay4(cmd_data[2]);
							break;
						default:
						
						cnt = 0;
						
					}		
					
					cnt=0;			
					
				}
				
			}
			else if (cnt > 3)
			{
				cnt=0;
			}
			
			if(cmd_data[0] != CMD_WRITE && cmd_data[0] != CMD_READ)
			{
				cnt=0;
			}

			
			
			
			#if 0
			switch(state)
			{
				case CMD_WAIT:
				
					//dummy = SPIF.DATA;
					
					if(rxdata & 0x40)
					{
						state = CMD_READ;
					}
					else if(rxdata & 0x80)
					{
						state = CMD_WRITE;
					}
				
					break;
				case CMD_WRITE:
				
					switch(state_reg)
					{
						case REG_RELAY_1:
						
						
						
						
						
					}


					state = CMD_WAIT;
									
					break;
				case CMD_READ:
					
					if(rxdata == M16_EXBOARD_INPUT_SIG_STATUS)
					{
						
					}
					
					state = CMD_WAIT;
					
					
					//WAIT_EORX();
					SPIF.DATA = get_ch18_Input();
					while(!(SPIF.STATUS & 0x80));
					dummy = SPIF.DATA;
					//WAIT_EORX();
				
					break;
				default:
					state = CMD_WAIT;
			}
			
			
			//dummy = SPIF.DATA;
			#endif
		}
		

		
		test=1;
		//OSTimeDlyHMSM(0,0,0,10);
		
		
		
	}



}

