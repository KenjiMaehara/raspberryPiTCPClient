/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h>
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
#include <avr/interrupt.h>
>>>>>>> parent of f8ff870... test063003
=======
#include <avr/interrupt.h>
>>>>>>> parent of f8ff870... test063003
=======
#include <avr/interrupt.h>
>>>>>>> parent of f8ff870... test063003
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
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
#include "uartRx.h"
>>>>>>> parent of f8ff870... test063003
=======
#include "uartRx.h"
>>>>>>> parent of f8ff870... test063003
=======
#include "uartRx.h"
>>>>>>> parent of f8ff870... test063003


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



<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003
u8 cmd_data02[50];
int cnt03 = 0;



char hex_to_asc(u8 hex)
{
	char da;
	da = hex & 0x0f;
	if((da >= 0) && (da <= 9)) return ('0' + da);
	else return ('a' + da - 0x0a);
}

<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003

u8 chInputStatus1(void)
{
	
	
	
}



<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003
#if 1
void spif_int(void)
{
	volatile u8 rxdata=0;
	
	rxdata = SPIF.DATA;

	cmd_data02[cnt03++] = rxdata;
	
	if(cmd_data02[cnt03]==0x0d)
	{
		cnt03 = 0;
	}
	                                                                                    
}
#else

ISR(SPIF_INT_vect)
{
	u8 rxdata=0;
	
	rxdata = SPIF.DATA;

	cmd_data02[cnt03++] = rxdata;
	
	if(cmd_data02[cnt03]==0x0d)
	{
		cnt03 = 0;
	}
}


#endif


<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003

void EventTask(void *p_arg)
{
	volatile testCount=0;
	volatile u8 testCharacter[100];
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	volatile u8 cmd_data[20];
=======
	//volatile u8 cmd_data[50];
	volatile u8 reverse_data[20];
>>>>>>> parent of f8ff870... test063003
=======
	//volatile u8 cmd_data[50];
	volatile u8 reverse_data[20];
>>>>>>> parent of f8ff870... test063003
=======
	//volatile u8 cmd_data[50];
	volatile u8 reverse_data[20];
>>>>>>> parent of f8ff870... test063003
	volatile u8 test=0;
	volatile u8 rxdata=0;
	volatile u8 txdata=0;
	volatile u8 txdata02[20];
	volatile u8 testtest=0;
	
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
	u8 state = CMD_WAIT;
	volatile int cnt = 0;
	u8 dummy;
	volatile char *id,*cmd, *opt, *data;
	//volatile char *test_cmd;
	while(1)
	{
	
	
		if(SPIF.STATUS & 0x80)
		{

			rxdata = SPIF.DATA;

			cmd_data[cnt++] = rxdata;
			
			
			if(cnt > 20)
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
			

			
			if(cnt > 2 && cmd_data[cnt]==0x0d)
			{
				id = strtok(cmd_data, ",");
				cmd = strtok(NULL, ",");
				opt = strtok(NULL, ",");
				data = strtok(NULL, ",");
			
				cnt = 0;
				
				char test_cmd[] = "rd";
				
				if(strcmp(cmd,test_cmd)==0)
				{
					testtest++;
				}
				
				
			}
			
			
			#if 0
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
				else if(cmd_data[0]==CMD_READ)
				{
					switch(cmd_data[1])
					{
						case REG_M16_EXBOARD_INPUT_ALL_SIG_STATUS:
							break;
						case INPUT_CH1:
							txdata = get_ch1_Input();
							break;
						case INPUT_CH18:
							testtest = get_ch18_Input();
							
							if(testtest==0)
							{
								txdata=0x55;
							}
							else
							{
								txdata=0xaa;
							}
							break;
							
					}
					dummy = SPIF.DATA;
					//SPIF.CTRL |= (1<<SPIF_BIT_MASTER);
					//spi_write_single(txdata);
					//SPIF.CTRL &= ~(1<<SPIF_BIT_MASTER);
					//SPIF.CTRL |= (1<<SPIF_BIT_MASTER);
					SPIF.DATA = txdata;
					while(!(SPIF.STATUS & 0x80));
					//WAIT_EORX();
					//dummy = SPIF.DATA;
					//SPIF.CTRL &= ~(1<<SPIF_BIT_MASTER);
					
					//cnt=0;		
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

			#endif
			
			
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

=======
=======
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003
	volatile int cntRevData=0;
	
	u8 state = CMD_WAIT;
	volatile int cnt = 0;
	volatile int cnt02 = 0;
	u8 dummy;
	 char *id,*cmd,*opt,*data,*data2,*data3,*data4,*data5,*data6;
	//volatile char *test_cmd;
	volatile char *test_cmd1;
	volatile char *test_cmd2;
	volatile char *test_cmd3;
	volatile char *test_cmd4;
	
	
	while(1)
	{
	
		if(flagSpifRx == true)
		{
			id = strtok(cmd_data, ",");
			cmd = strtok(NULL, ",");
			opt = strtok(NULL, ",");
			data = strtok(NULL, ",");
			data2 = strtok(NULL, ",");
			data3 = strtok(NULL, ",");
			data4 = strtok(NULL, ",");
			data5 = strtok(NULL, ",");
			data6 = strtok(NULL, ",");
		
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
					
					int i=0;
										
					reverse_data[i++] = 'M';
					reverse_data[i++] = 'E';
					reverse_data[i++] = ',';
					reverse_data[i++] = 'i';
					reverse_data[i++] = 'n';
					reverse_data[i++] = 'p';
					reverse_data[i++] = 'u';
					reverse_data[i++] = 't';
					reverse_data[i++] = 'a';
					reverse_data[i++] = 'l';
					reverse_data[i++] = 'l';
					reverse_data[i++] = ',';
					
					
					u8 makeData;

					#if 1
					makeData = 0;
					makeData |= get_ch8_Input() << 7;
					makeData |= get_ch7_Input() << 6;
					makeData |= get_ch6_Input() << 5;
					makeData |= get_ch5_Input() << 4;
					makeData |= get_ch4_Input() << 3;
					makeData |= get_ch3_Input() << 2;
					makeData |= get_ch2_Input() << 1;
					makeData |= get_ch1_Input();
					//hexToAsc(tSlave2.serial[0],(u8 *)&reverse_data[i]);
					reverse_data[i++] = hex_to_asc((makeData >> 4)&0x0f);
					reverse_data[i++] = hex_to_asc((makeData >> 0)&0x0f);
					reverse_data[i++] = ',';
					
					
					makeData = 0;
					makeData |= get_ch16_Input() << 7;
					makeData |= get_ch15_Input() << 6;
					makeData |= get_ch14_Input() << 5;
					makeData |= get_ch13_Input() << 4;
					makeData |= get_ch12_Input() << 3;
					makeData |= get_ch11_Input() << 2;
					makeData |= get_ch10_Input() << 1;
					makeData |= get_ch9_Input();
					
					reverse_data[i++] = hex_to_asc((makeData >> 4)&0x0f);
					reverse_data[i++] = hex_to_asc((makeData >> 0)&0x0f);
					reverse_data[i++] = ',';
					#endif
					
					
					
					
					reverse_data[i++] = 0x0d;
					
					cntRevData = i;
				
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
					
					
					reverse_data[0] = 'M';
					reverse_data[1] = 'E';
					reverse_data[2] = ',';
					reverse_data[3] = 'r';
					reverse_data[4] = 'e';
					reverse_data[5] = 'l';
					reverse_data[6] = 'a';
					reverse_data[7] = 'y';
					reverse_data[8] = 'C';
					reverse_data[9] = 't';
					reverse_data[10] = 'l';
					reverse_data[11] = ',';
					reverse_data[12] = 'o';
					reverse_data[13] = 'k';
					reverse_data[14] = ',';
					reverse_data[15] = 0x0d;
					
					cntRevData = 16;
					
							
				}
				else if(strstr(opt,"wSensor") != NULL)
				{
					putstr(data);
					sendChar(0x20);
					putstr(data2);
					sendChar(0x20);
					putstr(data3);
					sendChar(0x20);
					putstr(data4);
					sendChar(0x20);
					putstr(data5);
					sendChar(0x20);
					putstr(data6);
					sendChar(0x0d);
					sendChar(0x0a);
					
					
				}
			}
		}
	}		
<<<<<<< HEAD
<<<<<<< HEAD
}
>>>>>>> parent of f8ff870... test063003
=======
}
>>>>>>> parent of f8ff870... test063003
=======
}
>>>>>>> parent of f8ff870... test063003
