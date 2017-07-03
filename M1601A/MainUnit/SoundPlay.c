/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h>
#include <includes.h>
#include <mytype.h>
#include "Oper.h"
//#include "port.h"
//#include "env.h"
//#include "event.h"


/***************************************************************

1.MSG01_緊急1_L.mp3			”扉異常発生”
2.MSG02_緊急2_LR.mp3		”扉異常発生”
3.MSG03_緊急3＿L.mp3		”非常発生”
4.MSG04_緊急4＿LR.mp3		”非常発生”
5.MSG05_緊急アラーム音&異常発生_R.mp3		”PEEEEEPEEEEEEEEPEEEEEEEE　異常発生”
6.MSG06_後部扉異常_R.mp3					”後部扉が施錠されていません　確認してください”
7.MSG07_サイド扉異常_R.mp3			”サイドの扉が施錠されていません　確認してください”
8.MSG08_運転席扉異常_R.mp3			”運転席のドアが施錠されていません　確認してください”
9.MSG09_キャビン扉異常_R.mp3		”キャビンのドアが施錠されていません　確認してください”
10.MSG10_後部内扉異常_R.mp3			”後部内扉に異常が発生しました。緊急通報を行います。”
11.MSG11_サイド内扉異常_R.mp3		”サイドの内扉に異常が発生しました。緊急通報を行います。”
12.MSG12_キャビン内扉異常_R.mp3		”キャビンの内扉に異常が発生しました。緊急通報を行います。”
13.MSG13_複数外扉異常_R.mp3			”扉が施錠されていません。確認してください。”
14.MSG14_後部扉解錠_R.mp3			”後部扉を解錠します。”
15.MSG15_サイド扉解錠_R.mp3			”サイドの扉を解錠します。”
16.MSG16_キャビンドア解錠_R.mp3		”キャビンのドアを解錠します。”
17.MSG17_ドア解錠_R.mp3				”ドアを解錠します。”
18.MSG18_EMG受信1_R.mp3				”運転手の緊急通報を受信しました。緊急通報を行います。”
19.MSG19_EMG受信2_R.mp3				”警乗員の緊急通報を受信しました。緊急通報を行います。”
20.MSG20_緊急解除_R.mp3				”警報を解除します。”
21.MSG21_防犯開始_R.mp3				”防犯モードを開始します。”
22.MSG22_防犯解除_R.mp3				”防犯モードを解除します。”
23.MSG23_防犯ミス_R.mp3				”戸締りが不完全です。防犯モードは開始できません。”
24.MSG24_緊急アラーム音&異常発生_L.mp3		
25.MSG25_緊急アラーム音&異常発生_LR.mp3		”PEEEEEPEEEEEEEEPEEEEEEEE　異常発生　（LR両方版）”
26.MSG26_pinpon.mp3
27.MSG27_boo.mp3
28.MSG28_防犯モードは開始中です_R.mp3
29.MSG29_防犯モードは解除中です_R.mp3
30.MSG30_安否信号を通報します_R.mp3
31.MSG31_戸締りが不完全です。点検して下さい_R.mp3
32.MSG32_整備モードに移行します_R.mp3
33.MSG33_全てのドアを解錠します_R.mp3
34.MSG34_システムを点検します。しばらくお待ちください_R.mp3
35.MSG35_システムは正常です_R.mp3
36.MSG36_運転手用リモコンと通信が出来ません_R.mp3
37.MSG37_警乗員用リモコンと通信が出来ません_R.mp3
38.MSG38_後部ドア制御装置と通信が出来ません_R.mp3
39.MSG39_横扉ドア制御装置と通信が出来ません_R.mp3
40.MSG40_キャビン右ドア制御装置と通信が出来ません_R.mp3
41.MSG41_キャビン左ドア制御装置と通信が出来ません_R.mp3



****************************************************************/


enum
{
	ERR_DOOR_NONE=0,
	ERR_BACK_DOOR,
	ERR_SIDE_DOOR,
	ERR_CABIN_DOOR,
	ERR_DRIVER_DOOR,

};



void uart_init(void)
{
	//PORTC.PIN3CTRL = PORT_OPC_WIREDANDPULL_gc;
	
<<<<<<< HEAD
	USARTC0_BAUDCTRLB = 0x50;	//BSCALE is 0
	USARTC0_BAUDCTRLA = 12;	//BSEL		4.902khz
=======
	//USARTC0_BAUDCTRLB = 0x50;	//BSCALE is 0
	USARTC0_BAUDCTRLB = 0x40;	//BSCALE is 0	
	USARTC0_BAUDCTRLA = 12;	//BSEL		9.6khz
>>>>>>> parent of f8ff870... test063003
	

	//Disable interrupts, just for safety
	//USARTC0_CTRLC = 0;
	USARTC0_CTRLC = 0;
	//8 data bits, no parity and 1 stop bit
	USARTC0_CTRLC = USART_CHSIZE_8BIT_gc;
	
	//Enable receive and transmit
	USARTC0_CTRLB = USART_TXEN_bm; // And enable high speed mode
	
}


<<<<<<< HEAD
=======
void putstr(u8 * data)
{
	int i =0 ;

	for( i = 0 ;i < 50 ; i++)
	{
		//putc_(data[i]);
		sendChar(data[i]);
		if(data[i] == 0x0d || data[i] == 0x00) break;
	}
}
>>>>>>> parent of f8ff870... test063003


void sendChar(char c)
{
	
	while( !(USARTC0_STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
	
	USARTC0_DATA = c;
	
}





#if 0
void SoundPlay(u8 number)
{
	sendChar(0xef);					//sound stop
	//OSTimeDlyHMSM(0,0,0,200);	
	set_mp3_mute_ctl(true);
	set_mp3_sdown_ctl(false);
	OSTimeDlyHMSM(0,0,0,100);
	//sendChar(0xef);					//sound stop
	//OSTimeDlyHMSM(0,0,0,200);
	sendChar(0xe8);					//volume highest
	OSTimeDlyHMSM(0,0,0,50);
	sendChar(0xf1);					//folder #1
	OSTimeDlyHMSM(0,0,0,50);
	
	sendChar(number);				//sound #5
	
	OSTimeDlyHMSM(0,0,0,100);
	sendChar(0xec);					//play
	
}
#else

void SoundPlay(u8 number)
{
	sendChar(0xef);					//sound stop
	//OSTimeDlyHMSM(0,0,0,200);	
	set_mp3_active(true);			//MP3 Module ON   add 2015_05_18
	set_mp3_mute_ctl(true);
	set_mp3_sdown_ctl(false);
	OSTimeDlyHMSM(0,0,0,600);
	//sendChar(0xef);					//sound stop
	//OSTimeDlyHMSM(0,0,0,200);
	//sendChar(0xe8);					//volume highest
	if(number == 43)
	{
		for(int i=0;i<2;i++)
		{
			//sendChar(0xd8);
			//sendChar(0xda);
			//sendChar(0xdc);
			sendChar(0xdf);
			OSTimeDlyHMSM(0,0,0,300);			
		}
	}
	else
	{
		for(int i=0;i<2;i++)
		{
			sendChar(0xe7);					//volume highest
			OSTimeDlyHMSM(0,0,0,300);
		}
	}
	
	OSTimeDlyHMSM(0,0,0,300);
	sendChar(0xf1);					//folder #1
	OSTimeDlyHMSM(0,0,0,300);
	
	sendChar(number);				//sound #5
	
	OSTimeDlyHMSM(0,0,0,300);
	sendChar(0xec);					//play
	
}
#endif





u8 sound_stop = 0;
u8 door_error;

#if 0
void SoundPlayTask(void *p_arg)
{

	uart_init();


	while(1)
	{
		OSTimeDlyHMSM(0,0,3,0);
	
		#if 1
	
		if(sound_stop==true)
		{
			sendChar(0xef);					//sound stop
		}
	
	
			
		if((sound_stop==false) && (PORTC.IN & 0x04))
		{
			switch(oper->oper)
			{
				case OPER_SET:
					if(door_error == ERR_BACK_DOOR)
					{
						SoundPlay(6);
						break;
					}
					else if(door_error == ERR_SIDE_DOOR)
					{
						SoundPlay(7);
						break;
					}
					else if(door_error == ERR_CABIN_DOOR)
					{
						SoundPlay(9);
						break;
					}
					else if(door_error == ERR_DRIVER_DOOR)
					{
						SoundPlay(8);
						break;						
					}
					else
					{
						SoundPlay(21);
						break;
					}
					
					break;
	
				case OPER_RESET:
					SoundPlay(22);
					break;
	
				case OPER_EMG:
					SoundPlay(24);
					break;
	
				case OPER_EMG_RESET:
					SoundPlay(20);
					break;
	
				case OPER_AUDIO_PLUS:
				case OPER_AUDIO_MINUS:
		
					break;
				
				case OPER_ENG_START:
				
					if(door_error == ERR_BACK_DOOR)
					{
						SoundPlay(6);
						break;
					}
					else if(door_error == ERR_SIDE_DOOR)
					{
						SoundPlay(7);
						break;
					}
					else if(door_error == ERR_CABIN_DOOR)
					{
						SoundPlay(9);
						break;
					}
					else if(door_error == ERR_DRIVER_DOOR)
					{
						SoundPlay(8);
						break;
					}
					else
					{
						SoundPlay(21);
						break;
					}				
	
					break;
				
				case OPER_SERVICE:
	
					break;
				
				case OPER_DOOR:
					SoundPlay(17);
					break;
			}
		}
		
		#endif
		
	}
}

#endif 







