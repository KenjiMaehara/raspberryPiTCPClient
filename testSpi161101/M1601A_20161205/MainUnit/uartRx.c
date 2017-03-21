/*
 * CFile1.c
 *
 * Created: 2017-03-21 16:36:48
 *  Author: kmaehara
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
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
#include "uartRx.h"



u8 cmd_data[50];
u8 flagSpi1Rx;


void uartRxTask(void *p_arg)
{
	u8 rxdata=0;
	int cnt=0;
	
	flagSpi1Rx = false;
	
	while(1)
	{
		
		if(SPIF.STATUS & 0x00 && flagSpi1Rx == false)
		{
			
			rxdata = SPIF.DATA;
			cmd_data[cnt++] = rxdata;			

			if(cnt > 50)
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
				flagSpi1Rx=true;
				cnt = 0;
			}

		}
		
		
	}
}
