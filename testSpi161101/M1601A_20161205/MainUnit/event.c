/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h>
#include <includes.h>
#include <string.h>
#include "port.h"
#include "env.h"
#include "event.h"
#include "Oper.h"
#include "rf_task.h"
#include "menu.h"
#include "key.h"
#include "spi.h"


#define WAIT_EORX() while (! (SPIF.STATUS & SPI_IF_bm)){}
#define	SPIE_BIT_ENABLE	6
#define SPIE_BIT_MASTER 4
#define	SPIF_BIT_ENABLE	6
#define SPIF_BIT_MASTER 4


u32	Os_GetTime_for_DoorOpen_Resettimeout=0;


#define CMD_WAIT	1
#define CMD_READ	2
#define CMD_WRITE	3
#define CMD_DATA_SEND	4

#define REG_RELAY_1										0x00
#define REG_RELAY_2										0x01
#define REG_RELAY_3										0x02
#define REG_RELAY_4										0x03
#define REG_RELAY_5										0x04
#define INPUT_IF										0x05
#define REG_M16_EXBOARD_INPUT_ALL_SIG_STATUS			0x06
#define INPUT_CH1										0x07
#define INPUT_CH2										0x08
#define INPUT_CH3										0x09
#define INPUT_CH4										0x0A
#define INPUT_CH5										0x0B
#define INPUT_CH6										0x0C
#define INPUT_CH7										0x0D
#define INPUT_CH8										0x0E
#define INPUT_CH9										0x0F
#define INPUT_CH10										0x10
#define INPUT_CH11										0x11
#define INPUT_CH12										0x12
#define INPUT_CH13										0x13
#define INPUT_CH14										0x14
#define INPUT_CH15										0x15
#define INPUT_CH16										0x16
#define INPUT_CH17										0x17
#define INPUT_CH18										0x18
#define INPUT_SR										0x19




u8 chInputStatus1(void)
{
	
	
	
}




void EventTask(void *p_arg)
{
	volatile testCount=0;
	volatile u8 testCharacter[100];
	volatile u8 cmd_data[30];
	volatile u8 reverse_data[20];
	volatile u8 test=0;
	volatile u8 rxdata=0;
	volatile u8 txdata=0;
	volatile u8 txdata02[20];
	volatile u8 testtest=0;
	
	volatile int cntRevData=0;
	
	u8 state = CMD_WAIT;
	volatile int cnt = 0;
	volatile int cnt02 = 0;
	u8 dummy;
	 char *id,*cmd,*opt,*data,*data2,*data3;
	//volatile char *test_cmd;
	volatile char *test_cmd1;
	volatile char *test_cmd2;
	volatile char *test_cmd3;
	volatile char *test_cmd4;
	
	
	while(1)
	{
	
	
		if(SPIF.STATUS & 0x80)
		{

			rxdata = SPIF.DATA;

			cmd_data[cnt++] = rxdata;
			

			if(cntRevData == 0)
			{
				
				if(cnt02 != 0)
				{
					cnt02 = 0;
				}
				
				
				if(cnt > 30)
				{
					cnt = 0;
				}
				else if(cnt == 1)
				{
					if(cmd_data[0] != 'M')
					{
						cnt = 0;
					}
				}
				else if(cnt == 2)
				{
					if(cmd_data[1] != 'E')
					{
						cnt = 0;
					}
				}
				

				
				if(cnt > 2 && cmd_data[cnt-1]==0x0d)
				{
					id = strtok(cmd_data, ",");
					cmd = strtok(NULL, ",");
					opt = strtok(NULL, ",");
					data = strtok(NULL, ",");
					data2 = strtok(NULL, ",");
					data3 = strtok(NULL, ",");
				
					cnt = 0;
					
					
					if(strstr(cmd,"rd") != NULL)
					{

						if(strstr(opt,"inputall") != NULL)
						{
							#if 0
							reverse_data[0] = 0x12;
							reverse_data[1] = 0x34;
							reverse_data[2] = 0x56;
							reverse_data[3] = 0x88;
							#endif
							
							reverse_data[0] = 'M';
							reverse_data[1] = 'E';
							reverse_data[2] = ',';
							reverse_data[3] = 'i';
							reverse_data[4] = 'n';
							reverse_data[5] = 'p';
							reverse_data[6] = 'u';
							reverse_data[7] = 't';
							reverse_data[8] = 'a';
							reverse_data[9] = 'l';
							reverse_data[10] = 'l';
							reverse_data[11] = ',';
							

							#if 1
							reverse_data[12] = 0;
							reverse_data[12] |= get_ch8_Input() << 7;
							reverse_data[12] |= get_ch7_Input() << 6;
							reverse_data[12] |= get_ch6_Input() << 5;
							reverse_data[12] |= get_ch5_Input() << 4;
							reverse_data[12] |= get_ch4_Input() << 3;
							reverse_data[12] |= get_ch3_Input() << 2;
							reverse_data[12] |= get_ch2_Input() << 1;
							reverse_data[12] |= get_ch1_Input();
							
							reverse_data[13] = 0;
							reverse_data[13] |= get_ch16_Input() << 7;
							reverse_data[13] |= get_ch15_Input() << 6;
							reverse_data[13] |= get_ch14_Input() << 5;
							reverse_data[13] |= get_ch13_Input() << 4;
							reverse_data[13] |= get_ch12_Input() << 3;
							reverse_data[13] |= get_ch11_Input() << 2;
							reverse_data[13] |= get_ch10_Input() << 1;
							reverse_data[13] |= get_ch9_Input();							
							#endif
							
							
							reverse_data[14] = 0x0d;
							
							cntRevData = 15;
						
						}
					
					}
					else if(strstr(cmd,"wr") != NULL)
					{
					
						if(strstr(opt,"relayCtl") != NULL)	//relayCtl
						{
							if(strstr(data,"ch1") != NULL)
							{
								if(strstr(data2,"on") != NULL)
								{								
									set_Relay1(true);
								}
							
								if(strstr(data2,"off") != NULL)
								{								
									set_Relay1(false);
								}								
							}
						
							if(strstr(data,"ch2") != NULL)
							{
								if(strstr(data2,"on") != NULL)
								{
									set_Relay2(true);
								}
								
								if(strstr(data2,"off") != NULL)
								{
									set_Relay2(false);
								}
							}
						
							if(strstr(data,"ch3") != NULL)
							{
								if(strstr(data2,"on") != NULL)
								{
									set_Relay3(true);
								}
								
								if(strstr(data2,"off") != NULL)
								{
									set_Relay3(false);
								}
							}
						
							if(strstr(data,"ch4") != NULL)
							{
								if(strstr(data2,"on") != NULL)
								{
									set_Relay4(true);
								}
								
								if(strstr(data2,"off") != NULL)
								{
									set_Relay4(false);
								}
							}
									
						}
					}
				}
			}
			else
			{

				if(cnt02 < cntRevData)
				{
					dummy = SPIF.DATA;
					SPIF.DATA = reverse_data[cnt02];
					while(!(SPIF.STATUS & 0x80));
					//spi_write_single(reverse_data[cnt02]);
					cnt02++;
				}
				else
				{
					cnt02 = 0;
					cntRevData = 0;
					cnt = 0;
				}
			}
		
			test=1;
			//OSTimeDlyHMSM(0,0,0,10);
		
		}
	}		
}