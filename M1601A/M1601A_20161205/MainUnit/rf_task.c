/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include <includes.h>
#include "port.h"
#include "cc1100.h"
#include "mytype.h"
#include "Oper.h"
#include "rf_task.h"
#include "env.h"
#include "menu.h"
#include "event.h"

fifo_t gfifo;

#define RF_TIME_OUT 10

#define CARRIER_SENSE_DETECT	0x55


OS_EVENT *  rf_lock_event=NULL;

void test_wave_mode(void);
u8 setup_oper(void);
u8 gDoorStatus[12];
u8 gDoorLock[6];

u8 door_error_check = false;
u8 driving_state_door_open_flag=false;

void rf_lock(void)
{
	u8 err;
	if(rf_lock_event == NULL)
	{
		rf_lock_event = OSSemCreate(1);
	}
	OSSemPend(rf_lock_event, 0, &err);
}


void rf_unlock(void)
{
	OSSemPost(rf_lock_event);
}

u8 ReadSignatureByte(u16 Address)
{
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	uint8_t Result;
	__asm__ ("lpm %0, Z\n" : "=r" (Result) : "z" (Address));
	//  __asm__ ("lpm \n  mov %0, r0 \n" : "=r" (Result) : "z" (Address) : "r0");
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;
	return Result;
}

u16 read_adc(void)
{
	u8 Waste;

	if ((ADCA.CTRLA & ADC_ENABLE_bm) == 0)
	{
		ADCA.CTRLA = ADC_ENABLE_bm ; // Enable the ADC
		ADCA.CTRLB |= (1<<4); // Signed Mode?  pos neg?  
		//ADCA.CTRLB |= (1<<3); // Continius Mode
		ADCA.REFCTRL = 0; // Internal 1v ref
		ADCA.EVCTRL = 0 ; // no events
		//ADCA.EVCTRL = (5<<3); // ADC5,6,7selected events
		ADCA.PRESCALER = ADC_PRESCALER_DIV128_gc ;
		ADCA.CALL = ReadSignatureByte(0x20) ; //ADC Calibration Byte 0
		ADCA.CALH = ReadSignatureByte(0x21) ; //ADC Calibration Byte 1
		//ADCA.CALL = 0x00;		//test
		//ADCA.CALH = 0x0F;		//test
		OSTimeDlyHMSM(0,0,0,10); // Wait at least 25 clocks
	}
	ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc | 0x01 ; // Gain = 1, Single Ended
	//ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc | 0x10 ; // Gain = 1, Single Ended
	//ADCA.CH0.MUXCTRL = (0x07<<3);
	ADCA.CH0.MUXCTRL = (0x05<<3);
	//ADCA.CH0.MUXCTRL = (0x00<<3);
	ADCA.CH0.INTCTRL = 0 ; // No interrupt
	for(Waste = 0; Waste<2; Waste++)
	{
		ADCA.CH0.CTRL |= ADC_CH_START_bm; // Start conversion
		while (ADCA.INTFLAGS==0) ; // Wait for complete
		ADCA.INTFLAGS = ADCA.INTFLAGS ;
	}
	
	return ADCA.CH0RES ;
}


u16 temperature_get(void)
{
	u16 temp;
	
	temperature_test_enable();

	OSTimeDlyHMSM(0,0,0,100); // Wait 100msec

	//while(1);
	temp = read_adc();

	OSTimeDlyHMSM(0,0,0,100); // Wait 100msec

	temperature_test_disenable();

	return temp;
}




void set_freq(void)
{
	
	volatile u16	frq;
	volatile u8 	fr;
	volatile u16 	offset;
	volatile u16 	level;
	volatile u16 	temperature;
	
#ifndef KOREA
	//fr = 0x00;//get_freq();
	fr = ~get_freq() & 0x0f;

	frq = 0x66a8;


	eeprom_read_block((void*)&gTele_env,(void*) 0x000,sizeof(tele_env_t));

	frq += gTele_env.ctl_data.adj_freq;	

	#if 0
	if(gTele_env.ctl_data.freq_add_sub == 0x80)
	{
		frq -= gTele_env.ctl_data.adj_freq;	
	}
	else
	{
		frq += gTele_env.ctl_data.adj_freq;		
	}
	#endif

	#if	0
	temperature = temperature_get();

	if(gTele_env.ctl_data.temper_add_sub == 0x80)
	{
		temperature -= gTele_env.ctl_data.adj_temper;		
	}
	else
	{
		temperature += gTele_env.ctl_data.adj_temper;	
	}

	offset = 0x0339 - temperature;
	level = 13 - (offset / 0x0c);

	if(offset & 0x8000)
	{
		level = 13;		
	}
	else if((offset / 0x0c)>13)
	{
		level = 0;		
	}
	gLevel = level;

	frq += level_offset[level];
	#endif



	
	//frq = 0x68fc + (fr * 0x003f);
	//frq = 0x66aa + (fr * 0x003f);
	frq += (fr * 0x003f);
	//frq -= 0x10;
	offset = 0x0339 - 0;
	
	level = 13 - (offset / 0x0c);
	

	if(offset & 0x8000)
		level = 13;
	else if((offset / 0x0c)>13)
		level = 0;
#else
	

	fr = ~get_freq() & 0x0f;

	frq = 0x66a8 + (fr * 0x003f);
	offset = 0x0339 - 0;
	
	level = 13 - (offset / 0x0c);
	

	if(offset & 0x8000)
	level = 13;
	else if((offset / 0x0c)>13)
	level = 0;
	

#endif 		
		
	cc1101_set_freq(frq);
}



#if 1

u8 check01,check02,check03,check04,check05,check06;

u8 cc1101_tx(u8 * data , u8 length,u8 th)
{
	int i;

	set_freq();
	
	cc1101_rtx_reg_set(1); // rx
	//led_flasher(10,500);
	cc1101_carrier_wave_setup();
	cc1100_cfg_gdo0(0x0e);

	cc1100_cmd_idle();
	cc1100_cmd_flush_rx();
	cc1100_cmd_calibrate();
	cc1100_cmd_rx();
	
	PORTA.PIN5CTRL = 0x18; // pullup , both edge
	
	OSTimeDlyHMSM(0,0,0,5);

	if(PORTA.IN & 0x20)
	{
		cc1100_cmd_idle();
		//mdelay(195);
		//_delay_ms(195);
		
		if(th == 0)
		{
			return CARRIER_SENSE_DETECT;
		}

	}

	PORTA.PIN5CTRL = 0x00;
	
	set_freq();

	//check01 = cc1100_read_status(CC1100_REG_MARCSTATE);

	cc1101_rtx_reg_set(0);
	cc1101_carrier_wave_setup();
	cc1100_cmd_idle();
	cc1100_cmd_calibrate();
	tx_fifo_write(data,length);
	
	cc1100_write_reg(CC1100_REG_MCSM1,0x00);
	cc1100_cfg_gdo0(0x09);

	//check02 = cc1100_read_status(CC1100_REG_MARCSTATE);


	cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);
	cc1100_cmd_tx();
//	check03 = cc1100_read_status(CC1100_REG_MARCSTATE);
	while(!get_gdoa());	//data send start
//	check04 = cc1100_read_status(CC1100_REG_MARCSTATE);
	while(get_gdoa())		//data send end
	{
//		check05 = cc1100_read_status(CC1100_REG_MARCSTATE);			
	}

//	OSTimeDlyHMSM(0,0,0,70);
//	check06 = cc1100_read_status(CC1100_REG_MARCSTATE);	

	return 1;
}




u8 cc1101_rx(u8 * data,u8 * rssi, u8 * lqi)
{
	int cnt_wait_syn_ack=0;
	//int cnt_wait_syn_ack02=0;
	u8 length;
	u8 RX_STATUS = RX_FIFO_NONE;
	
	
	cc1100_cmd_idle();
	set_freq();
	//check01 = cc1100_read_status(CC1100_REG_MARCSTATE);
	
	cc1101_rtx_reg_set(1);
	cc1101_carrier_wave_setup();
	cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);

	cc1100_cmd_idle();
	cc1100_cmd_flush_rx();
	cc1100_cmd_calibrate();
	cc1100_cmd_rx();
	
	//check02 = cc1100_read_status(CC1100_REG_MARCSTATE);
	
	while(get_gdoa() == false)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(++cnt_wait_syn_ack > 50)
		{
			return false;
		}
	}
	
	//check03 = cc1100_read_status(CC1100_REG_MARCSTATE);
	
	cnt_wait_syn_ack = 0;
	//cnt_wait_syn_ack02 = 0;

	while(1)
	{
		if(get_gdoa()==true || RX_STATUS==RX_FIFO_MAKING || RX_STATUS==RX_FIFO_COMPLETE)
		{
			
			if(get_gdoa() == false || RX_STATUS==RX_FIFO_COMPLETE)
			{
				if((cc1100_status_crc_lqi() & 0x80))
				{
					length = rx_fifo_read(data,rssi,lqi);	
					return length;
				}
				else
				{
					RX_STATUS=RX_FIFO_COMPLETE;
				}			
			}
			else
			{
				RX_STATUS=RX_FIFO_MAKING;
			}
		}
		
		OSTimeDlyHMSM(0,0,0,1);
		if(++cnt_wait_syn_ack > 50)
		{
			break;
		}
	}
	return 0;
}


#endif




u8 cc1101_rx_02(u8 * data,u8 * rssi, u8 * lqi)
{
	u8 cnt_wait_syn_ack=0;
	u8 length;
	
	//OSTimeDlyHMSM(0,0,0,10);
	//cc1100_cmd_idle();
	//set_freq();
	
	cc1101_rtx_reg_set(1);
	cc1101_carrier_wave_setup();
	cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);

	cc1100_cmd_idle();
	cc1100_cmd_flush_rx();
	cc1100_cmd_calibrate();
	cc1100_cmd_rx();

	while(get_gdoa() == false)
	{
		OSTimeDlyHMSM(0,0,0,1);
		if(++cnt_wait_syn_ack > 50)
			break;
	}
	
	OSTimeDlyHMSM(0,0,0,100);
	cnt_wait_syn_ack=0;

	check01 = cc1100_read_status(CC1100_REG_MARCSTATE);

	while(1)
	{
		//if(get_gdoa() == false)
		if(get_gdoa() == false)
		{
			check02 = cc1100_read_status(CC1100_REG_MARCSTATE);
				
			if((cc1100_status_crc_lqi() & 0x80))
			{

				length = rx_fifo_read(data,rssi,lqi);
				return length;
			}
			//break;
		}
		OSTimeDlyHMSM(0,0,0,10);
		if(++cnt_wait_syn_ack > 200)
		{
			check03 = cc1100_read_status(CC1100_REG_MARCSTATE);			
			break;

		}
	}

	return 0;
}







void cc1101_tx_carrier(void)
{

	cc1101_carrier_wave_setup();

	cc1100_cfg_txoff_mode(CC1100_TXOFF_MODE_STAY_TX);
	cc1100_cfg_manchester_en(CC1100_MANCHESTER_DISABLE);
	cc1100_write_reg(CC1100_REG_MDMCFG3,0x33);
	cc1100_cfg_mod_format(CC1100_MODULATION_ASK);
	cc1100_write_reg(CC1100_REG_FREND0,0x10);
	cc1100_cfg_gdo0(CC1100_GDOx_CLK_XOSC_1);

	/* IDLE */
	cc1100_cmd_idle();
	/* MANCAL*/
	cc1100_cmd_calibrate();
	/* FS WAKEUP */
	cc1100_cmd_flush_tx();

	cc1100_cfg_gdo0(0x0e);
	//carrier_sense();

	cc1100_cmd_tx();
}


void temperature_test_enable(void)
{
	cc1100_cmd_idle();
	cc1100_write_reg(CC1100_REG_PTEST,0xBF);
	cc1100_cfg_gdo0(0x80);
}


void temperature_test_disenable(void)
{
	cc1100_cfg_gdo0(0x3F);
	cc1100_write_reg(CC1100_REG_PTEST,0x7F);
}


int get_device_id(u32 id)
{
	if(gEnv.id.driver == id)
	{
		if(gEnv.use.driver == true)
		{
			return 1;
		}
	}
	else if(gEnv.ex_id.ex_driver == id)
	{
		if(gEnv.ex_use.ex_driver == true)
		{
			return 1;
		}
	}
	else if(gEnv.id.security == id)
	{
		if(gEnv.use.security == true)
		{
			return 2;			
		}
	}
	else if(gEnv.ex_id.ex_security == id)
	{
		if(gEnv.ex_use.ex_security == true)
		{
			return 2;
		}	
	}
	else if(gEnv.id.b_door == id)
	{
		if(gEnv.use.b_door == true)
		{
			return 3;			
		}
	}
	else if(gEnv.id.s_door == id)
	{
		if(gEnv.use.s_door == true)
		{
			return 4;			
		}
	}
	else if(gEnv.id.cabin_left == id)
	{
		if(gEnv.use.cabin_left == true)
		{
			return 5;
		}
	}
	else if(gEnv.id.cabin_right == id)
	{
		if(gEnv.use.cabin_right == true)
		{
			return 6;			
		}
	}

	
	return -1;	
}


u8 rf_sync_device(u32 id)
{
	sync_t sync;
	int length;
	u8 rssi,lqi;
	u32 Os_GetTime=0;
	u8	th;
	u8	carrier_sen_check=0;
	


	set_reardoor_lock_led(false);
	rf_lock();	
	sync.hdr.master_id = gEnv.id.main;	
	sync.hdr.slave_id = id;
	sync.hdr.payload = PAYLOAD_SYNC;
	
	// download setup data
	switch(get_device_id(id))
	{
		case 1:
			sync.reserve[0] = gEnv.remocon.emg_on;
			sync.reserve[1] = gEnv.remocon.emg_off;
			sync.reserve[2] = gEnv.remocon.button_on;
			
			sync.reserve[3] = oper_get_emg_state();
			sync.reserve[4] = event_get_state() == RESET ? 0 : 2;
		
			break;
		case 2:
			
			sync.reserve[0] = gEnv.remocon.emg_on;
			sync.reserve[1] = gEnv.remocon.emg_off;
			sync.reserve[2] = gEnv.remocon.button_on;
			
			sync.reserve[3] = oper_get_emg_state();
			sync.reserve[4] = event_get_state() == RESET ? 0 : 2;
		
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			sync.reserve[0] = (u8)gEnv.main_set.beep_b_s_door_used;		//20150525 add
			
			if(get_device_id(id) == 3 || get_device_id(id) == 4)
			{
				sync.reserve[1] = (u8)gEnv.main_set.unlockRootvanKinkoWaveUsed;	//20160408 add
			}
			else if(get_device_id(id) == 5 || get_device_id(id) == 6)
			{
				sync.reserve[1] = (u8)gEnv.main_set.unlockRootvanDCabWaveUsed;	//20160411 add
			}
			
			
			
			#if 1
			if(get_device_id(id)==3)
				sync.reserve[2] = (u8)gEnv.door_Type.b_door;
			else if(get_device_id(id)==4)
				sync.reserve[2] = (u8)gEnv.door_Type.s_door;
			else if(get_device_id(id)==5)
				sync.reserve[2] = (u8)gEnv.door_Type.cabin_right;
			else if(get_device_id(id)==6)
				sync.reserve[2] = (u8)gEnv.door_Type.cabin_left;
			#endif
			
			
			
					
			sync.reserve[3] = oper_get_emg_state();
			sync.reserve[4] = event_get_state() == RESET ? 0 : 2;

			break;
	}
	
	
	
	for(int i=0;i<10;i++)
	{
		if(i==5 || i==9)
		{
			th = 0;
		}
		else
		{
			th = 1;
		}
		
		carrier_sen_check = cc1101_tx(&sync,sizeof(sync_t),th);
		//OSTimeDlyHMSM(0,0,0,50);		

		//Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT * OS_TICKS_PER_SEC);
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT * 100);	
		}
		else
		{
			Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT);	
		}
		

	
		while(1)
		{
			length = cc1101_rx((u8 *)&sync,&rssi,&lqi);
			if(length > 0 )
			{
				if(sync.hdr.slave_id == id && sync.hdr.payload == PAYLOAD_SYNC)
				{
					switch(get_device_id(id))
					{
						case 1:
						break;
						case 2:
						break;
						case 3:
						case 4:
						case 5:
						case 6:
						//gDoorStatus[(get_device_id(id) - 3) * 2 + 0] = sync.reserve[0];
						//gDoorStatus[(get_device_id(id) - 3) * 2 + 1] = sync.reserve[1];
						//gDoorLock[(get_device_id(id) - 3)]	 = sync.reserve[2];
						gDoorStatus[(get_device_id(id) - 3) * 2 + 0] = 0;
						gDoorStatus[(get_device_id(id) - 3) * 2 + 1] = 0;
						gDoorLock[(get_device_id(id) - 3)]	 = 1;
						break;
					}
				
					rf_unlock();
					return true;
				}
			}
			cc1100_cmd_idle();
		
			if(Os_GetTime <= OSTimeGet())
			{
				break;
			}
		}
		
		if(length > 0)
		{
			break;
		}
		
	}
	
	rf_unlock();
	
	return false;
}



u8 rf_sync_start_device(u32 id)
{
	sync_t sync;
	int length;
	u8 rssi,lqi;
	u32 Os_GetTime=0;
	u8	th;
	u8	carrier_sen_check=0;
	


	set_reardoor_lock_led(false);
	rf_lock();
	sync.hdr.master_id = gEnv.id.main;
	sync.hdr.slave_id = id;
	sync.hdr.payload = PAYLOAD_SYNC_START;
	
	// download setup data
	switch(get_device_id(id))
	{
		case 1:
		sync.reserve[0] = gEnv.remocon.emg_on;
		sync.reserve[1] = gEnv.remocon.emg_off;
		sync.reserve[2] = gEnv.remocon.button_on;
		
		sync.reserve[3] = oper_get_emg_state();
		sync.reserve[4] = event_get_state() == RESET ? 0 : 2;
		
		break;
		case 2:
		
		sync.reserve[0] = gEnv.remocon.emg_on;
		sync.reserve[1] = gEnv.remocon.emg_off;
		sync.reserve[2] = gEnv.remocon.button_on;
		
		sync.reserve[3] = oper_get_emg_state();
		sync.reserve[4] = event_get_state() == RESET ? 0 : 2;
		
		break;
		case 3:
		case 4:
		case 5:
		case 6:
		sync.reserve[0] = (u8)gEnv.main_set.beep_b_s_door_used;		//20150525 add
		
		if(get_device_id(id) == 3 || get_device_id(id) == 4)
		{
			sync.reserve[1] = (u8)gEnv.main_set.unlockRootvanKinkoWaveUsed;	//20160408 add
		}
		else if(get_device_id(id) == 5 || get_device_id(id) == 6)
		{
			sync.reserve[1] = (u8)gEnv.main_set.unlockRootvanDCabWaveUsed;	//20160411 add
		}
		
		
		
		#if 1
		if(get_device_id(id)==3)
		sync.reserve[2] = (u8)gEnv.door_Type.b_door;
		else if(get_device_id(id)==4)
		sync.reserve[2] = (u8)gEnv.door_Type.s_door;
		else if(get_device_id(id)==5)
		sync.reserve[2] = (u8)gEnv.door_Type.cabin_right;
		else if(get_device_id(id)==6)
		sync.reserve[2] = (u8)gEnv.door_Type.cabin_left;
		#endif
		
		
		
		
		sync.reserve[3] = oper_get_emg_state();
		sync.reserve[4] = event_get_state() == RESET ? 0 : 2;

		break;
	}
	
	
	
	for(int i=0;i<10;i++)
	{
		if(i==5 || i==9)
		{
			th = 0;
		}
		else
		{
			th = 1;
		}
		
		carrier_sen_check = cc1101_tx(&sync,sizeof(sync_t),th);
		//OSTimeDlyHMSM(0,0,0,50);

		//Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT * OS_TICKS_PER_SEC);
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT * 100);
		}
		else
		{
			Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT);
		}
		

		
		while(1)
		{
			length = cc1101_rx((u8 *)&sync,&rssi,&lqi);
			if(length > 0 )
			{
				if(sync.hdr.slave_id == id && sync.hdr.payload == PAYLOAD_SYNC)
				{
					switch(get_device_id(id))
					{
						case 1:
						break;
						case 2:
						break;
						case 3:
						case 4:
						case 5:
						case 6:
						//gDoorStatus[(get_device_id(id) - 3) * 2 + 0] = sync.reserve[0];
						//gDoorStatus[(get_device_id(id) - 3) * 2 + 1] = sync.reserve[1];
						//gDoorLock[(get_device_id(id) - 3)]	 = sync.reserve[2];
						gDoorStatus[(get_device_id(id) - 3) * 2 + 0] = 0;
						gDoorStatus[(get_device_id(id) - 3) * 2 + 1] = 0;
						gDoorLock[(get_device_id(id) - 3)]	 = 1;
						break;
					}
					
					rf_unlock();
					return true;
				}
			}
			cc1100_cmd_idle();
			
			if(Os_GetTime <= OSTimeGet())
			{
				break;
			}
		}
		
		if(length > 0)
		{
			break;
		}
		
	}
	
	rf_unlock();
	
	return false;
}




u8 rf_setup_id(u32 id)
{
	setup_t setup_id;
	int length;
	u8 rssi,lqi;
	u32 Os_GetTime=0;

	

	rf_lock();	
	setup_id.hdr.master_id = gEnv.id.main;
	setup_id.hdr.slave_id = 0xffffffff;
	setup_id.hdr.payload = PAYLOAD_SETUP_ID;
	setup_id.id = id;
	
	
	for(int i=0;i<2;i++)
	{
		// download setup data
		
		cc1101_tx(&setup_id,sizeof(setup_t),1);
		

		//Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT * OS_TICKS_PER_SEC);
		Os_GetTime = OSTimeGet() + (2 * OS_TICKS_PER_SEC);
		
		
		while(1)
		{
			
			length = cc1101_rx((u8 *)&setup_id,&rssi,&lqi);
			if(length > 0 )
			{
				if(setup_id.hdr.slave_id == id && setup_id.hdr.payload == PAYLOAD_SETUP_ID)
				{
					rf_unlock();
					return true;
				}
			}
			cc1100_cmd_idle();
			
			if(Os_GetTime <= OSTimeGet())
			{
				break;
			}
		}
		
	
	}
	
	
	rf_unlock();
	
	return false;
}


//u8 rf_init_device(void)
u8 rf_init_device(void)
{
	u8 device_err_id = true;


	
	if(gEnv.use.driver == true)
	{
		for(int i=0;i<7;i++)
		{
			if(rf_sync_device(gEnv.id.driver) == false)
			{
				if(i==6)
				{
					if(device_err_id == true)
					{
						device_err_id = 5;
					}
				}
			}
			else
			{
				break;
			}
		}
	}
	
	
	if(gEnv.use.security == true)
	{
		for(int i=0;i<7;i++)
		{
			if(rf_sync_device(gEnv.id.security) == false)
			{
				if(i==6)
				{
					if(device_err_id == true)
					{
						device_err_id = 6;
					}	
				}
			}
			else
			{
				break;
			}
		}
	}


	if(gEnv.ex_use.ex_driver == true)
	{
		for(int i=0;i<7;i++)
		{
			if(rf_sync_device(gEnv.ex_id.ex_driver) == false)
			{
				if(i==6)
				{
					if(device_err_id == true)
					{
						device_err_id = 5;
					}
				}
			}
			else
			{
				break;
			}
		}
	}


	if(gEnv.ex_use.ex_security == true)
	{
		for(int i=0;i<7;i++)
		{
			if(rf_sync_device(gEnv.ex_id.ex_security) == false)
			{
				if(i==6)
				{
					if(device_err_id == true)
					{
						device_err_id = 6;
					}
				}
			}
			else
			{
				break;
			}
		}
	}





	#if 1
	if(gEnv.main_set.unlockRootvanKinkoWaveUsed == true)
	{
		pt_data[0] = 0x60;		//output power 0dBm
		//pt_data[0] = 0x12;			//output power -30dBm
		cc1101_8PATABLE_write_reg();
	}
	#endif


	if(gEnv.use.b_door == true)
	{
		
		for(int i=0;i<7;i++)
		{
			if(gEnv.main_set.unlockRootvanKinkoWaveUsed == true)
			{
				if(i==3)
				{
					pt_data[0] = 0x34;		//output power -10dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i==4)
				{
					pt_data[0] = 0x1d;		//output power -15dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i==5)
				{
					pt_data[0] = 0x0d;		//output power -20dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i>5)
				{
					pt_data[0] = 0x60;		//output power 0dBm
					cc1101_8PATABLE_write_reg();
				}
			}			
			
			//if(rf_sync_device(gEnv.id.b_door) == false)
			if(rf_sync_start_device(gEnv.id.b_door) == false)
			{
				if(i==6)
				{
					if(device_err_id == true)
					{
						device_err_id = 7;
					}
					//return 7;	
				}
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}		
	}
	
	if(gEnv.use.s_door == true)
	{
		for(int i=0;i<7;i++)
		{
			
			if(gEnv.main_set.unlockRootvanKinkoWaveUsed == true)
			{
				if(i==3)
				{
					pt_data[0] = 0x34;		//output power -10dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i==4)
				{
					pt_data[0] = 0x1d;		//output power -15dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i==5)
				{
					pt_data[0] = 0x0d;		//output power -20dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i>5)
				{
					pt_data[0] = 0x60;		//output power 0dBm
					cc1101_8PATABLE_write_reg();
				}
			}

			//if(rf_sync_device(gEnv.id.s_door) == false)
			if(rf_sync_start_device(gEnv.id.s_door) == false)
			{
				if(i==6)
				{
					if(device_err_id == true)
					{
						device_err_id = 8;
					}
					//return 8;			
				}
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}

	#if 1
	if(gEnv.main_set.unlockRootvanDCabWaveUsed == true)
	{
		pt_data[0] = 0x60;		//output power 0dBm
		//pt_data[0] = 0x12;			//output power -30dBm
		cc1101_8PATABLE_write_reg();
	}
	else
	{
		if(pt_data[0] != 0xc0)
		{
			pt_data[0] = 0xc0;		//output power 10dBm
			cc1101_8PATABLE_write_reg();
		}
	}
	#endif

	
	if(gEnv.use.cabin_left == true)
	{
		for(int i=0;i<7;i++)
		{

			if(gEnv.main_set.unlockRootvanDCabWaveUsed == true)
			{
				if(i==3)
				{
					pt_data[0] = 0x34;		//output power -10dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i==4)
				{
					pt_data[0] = 0x1d;		//output power -15dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i==5)
				{
					pt_data[0] = 0x0d;		//output power -20dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i>5)
				{
					pt_data[0] = 0x60;		//output power 0dBm
					cc1101_8PATABLE_write_reg();
				}				
			}

			
			//if(rf_sync_device(gEnv.id.cabin_left) == false)
			if(rf_sync_start_device(gEnv.id.cabin_left) == false)
			{
				if(i==6)
				{
					if(device_err_id == true)
					{
						device_err_id = 10;
					}
					//return 9;		
				}
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}
	
	if(gEnv.use.cabin_right == true)
	{
		for(int i=0;i<7;i++)
		{
			
			if(gEnv.main_set.unlockRootvanDCabWaveUsed == true)
			{
				if(i==3)
				{
					pt_data[0] = 0x34;		//output power -10dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i==4)
				{
					pt_data[0] = 0x1d;		//output power -15dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i==5)
				{
					pt_data[0] = 0x0d;		//output power -20dBm
					cc1101_8PATABLE_write_reg();
				}
				else if(i>5)
				{
					pt_data[0] = 0x60;		//output power 0dBm
					cc1101_8PATABLE_write_reg();
				}
			}

			//if(rf_sync_device(gEnv.id.cabin_right) == false)
			if(rf_sync_start_device(gEnv.id.cabin_right) == false)
			{
				if(i==6)
				{
					if(device_err_id == true)
					{
						device_err_id = 9;
					}
					//return 10;	
				}
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}

	}

	if(pt_data[0] != 0xc0)
	{
		pt_data[0] = 0xc0;		//output power 10dBm
		cc1101_8PATABLE_write_reg();
	}
	
	return device_err_id;

}


u8 rf_state_device(u32 id,u8 state)
{
	set_t set;
	int length;
	u8 rssi,lqi;
	u32 Os_GetTime=0;
	u8	th;
	u8	carrier_sen_check=0;

	rf_lock();
	set.hdr.master_id = gEnv.id.main;
	set.hdr.slave_id = id;
	set.hdr.payload = SET_STATE;
	set.set = state;
	
	//set_freq();
	
	for(int i=0;i<10;i++)
	{
		if(i==4 || i==9)
		{
			th = 1;
		}
		else
		{
			th = 0;
		}
		
		carrier_sen_check = cc1101_tx(&set,sizeof(set_t),th);
		//OSTimeDlyHMSM(0,0,0,50);

	
		//Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT * OS_TICKS_PER_SEC);
		
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT * 100);
		}
		else
		{
			Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT);
		}

	
		while(1)
		{
		
			//set_freq();
			length = cc1101_rx((u8 *)&set,&rssi,&lqi);
			if(length > 0 )
			{
				if(set.hdr.slave_id == id && set.hdr.payload == SET_STATE)
				{
					rf_unlock();
					return true;
				}
			}
			cc1100_cmd_idle();
		
			if(Os_GetTime <= OSTimeGet())
			{
				break;
			}
		
		}
		
		if(length > 0)
		{
			break;
		}
	}
	
	rf_unlock();	
	return false;
}


u8 rf_send_state(u8 state)
{
	u8 ret = true;

	
	if(gEnv.use.driver == true)
	{
		for(int i=0;i<2;i++)
		{
			if(rf_state_device(gEnv.id.driver,state) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}
	

	if(gEnv.ex_use.ex_driver == true)
	{
		for(int i=0;i<2;i++)
		{
			if(rf_state_device(gEnv.ex_id.ex_driver,state) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}	
	
	if(gEnv.use.security == true)
	{
		for(int i=0;i<2;i++)
		{
			if(rf_state_device(gEnv.id.security,state) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}		
	}

	if(gEnv.ex_use.ex_security == true)
	{
		for(int i=0;i<2;i++)
		{
			if(rf_state_device(gEnv.ex_id.ex_security,state) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}

	#if 1
	if(gEnv.main_set.unlockRootvanKinkoWaveUsed == true)
	{
		pt_data[0] = 0x60;		//output power 0dBm
		//pt_data[0] = 0x12;			//output power -30dBm
		cc1101_8PATABLE_write_reg();
	}
	#endif
	
	

	if(gEnv.use.b_door == true)
	{
		for(int i=0;i<2;i++)
		{
			if(rf_state_device(gEnv.id.b_door,state) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}

	}
	
	if(gEnv.use.s_door == true)
	{
		for(int i=0;i<2;i++)
		{
			if(rf_state_device(gEnv.id.s_door,state) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}	
			//OSTimeDlyHMSM(0,0,0,20);	
		}

	}

	#if 1
	if(gEnv.main_set.unlockRootvanDCabWaveUsed == true)
	{
		pt_data[0] = 0x60;		//output power 0dBm
		//pt_data[0] = 0x12;			//output power -30dBm
		cc1101_8PATABLE_write_reg();
	}
	else
	{
		if(pt_data[0] != 0xc0)
		{
			pt_data[0] = 0xc0;		//output power 10dBm
			cc1101_8PATABLE_write_reg();
		}
	}
	#endif

	
	if(gEnv.use.cabin_left == true)
	{
		for(int i=0;i<2;i++)
		{
			if(rf_state_device(gEnv.id.cabin_left,state) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}

	if(gEnv.use.cabin_right == true)
	{
		for(int i=0;i<2;i++)
		{
			if(rf_state_device(gEnv.id.cabin_right,state) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}
	
	if(pt_data[0] != 0xc0)
	{
		pt_data[0] = 0xc0;		//output power 10dBm
		cc1101_8PATABLE_write_reg();
	}	
	
	return ret;
}




u8 rf_driver_door_err_report_device(u32 id)
{
	set_t set;
	int length;
	u8 rssi,lqi;
	u32 Os_GetTime=0;
	u8	th;
	u8	carrier_sen_check=0;

	rf_lock();
	set.hdr.master_id = gEnv.id.main;
	set.hdr.slave_id = id;
	set.hdr.payload = PAYLOAD_DRIVER_DOOR_ERR_REPORT;
	//set.set = state;
	
	//set_freq();
	
	for(int i=0;i<10;i++)
	{
		if(i==4 || i==9)
		{
			th = 1;
		}
		else
		{
			th = 0;
		}
		
		carrier_sen_check = cc1101_tx(&set,sizeof(set_t),th);
		//OSTimeDlyHMSM(0,0,0,50);

		
		//Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT * OS_TICKS_PER_SEC);
		
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT * 100);
		}
		else
		{
			Os_GetTime = OSTimeGet() + ((u32)RF_TIME_OUT);
		}

		
		while(1)
		{
			
			//set_freq();
			length = cc1101_rx((u8 *)&set,&rssi,&lqi);
			if(length > 0 )
			{
				if((set.hdr.slave_id == id && set.hdr.payload == SET_STATE) || (set.hdr.slave_id == id && set.hdr.payload == PAYLOAD_DRIVER_DOOR_ERR_REPORT))
				{
					rf_unlock();
					return true;
				}
			}
			cc1100_cmd_idle();
			
			if(Os_GetTime <= OSTimeGet())
			{
				break;
			}
			
		}
		
		if(length > 0)
		{
			break;
		}
	}
	
	rf_unlock();
	return false;
}



u8 rf_send_driver_door_err_report(void)
{
	if(gEnv.use.driver == true)
	{
		rf_driver_door_err_report_device(gEnv.id.driver);
	}
	
	if(gEnv.ex_use.ex_driver == true)
	{
		rf_driver_door_err_report_device(gEnv.ex_id.ex_driver);
	}
	
	if(gEnv.use.security == true)
	{
		rf_driver_door_err_report_device(gEnv.id.security);
	}
	
	if(gEnv.ex_use.ex_security == true)
	{
		rf_driver_door_err_report_device(gEnv.ex_id.ex_security);
	}	
}





u8 rf_emg_device(u32 id,u8 emg)
{
	emg_t temg;
	int length;
	u8 rssi,lqi;
	u32 Os_GetTime=0;
	u8 th;
	u8 carrier_sen_check=0;

	rf_lock();
	temg.hdr.master_id = gEnv.id.main;
	temg.hdr.slave_id = id;
	temg.hdr.payload = PAYLOAD_EMG;
	temg.emg = emg;
	
	//set_freq();
	
	for(int i=0;i<10;i++)
	{
		if(i==5 || i==9)
		{
			th = 1;
		}
		else
		{
			th = 0;
		}

		
		carrier_sen_check = cc1101_tx(&temg,sizeof(emg_t),th);
	
		//Os_GetTime = OSTimeGet() + RF_TIME_OUT * OS_TICKS_PER_SEC;
		
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			Os_GetTime = OSTimeGet() + RF_TIME_OUT * 100;
		}
		else
		{
			Os_GetTime = OSTimeGet() + RF_TIME_OUT;			
		}
		
		while(1)
		{
		
			length = cc1101_rx((u8 *)&temg,&rssi,&lqi);
			if(length > 0 )
			{
				if(temg.hdr.slave_id == id && temg.hdr.payload == PAYLOAD_EMG)
				{
					rf_unlock();
					return true;
				}
			}
			cc1100_cmd_idle();

			if(Os_GetTime <= OSTimeGet())
			{
				break;
			}
		}
		if(length > 0)
		{
			break;
		}
	}
	
	
	rf_unlock();	
	return false;
}


u8 rf_send_emg(u8 emg)
{
	u8 ret = true;
	
	if(gEnv.use.driver == true)
	{
		for(int i=0;i<7;i++)
		{
			if(rf_emg_device(gEnv.id.driver,emg) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}
	
	if(gEnv.ex_use.ex_driver == true)
	{
		for(int i=0;i<7;i++)
		{
			if(rf_emg_device(gEnv.ex_id.ex_driver,emg) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}
	
	if(gEnv.use.security == true)
	{
		for(int i=0;i<7;i++)
		{
			if(rf_emg_device(gEnv.id.security,emg) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}

	if(gEnv.ex_use.ex_security == true)
	{
		for(int i=0;i<7;i++)
		{
			if(rf_emg_device(gEnv.ex_id.ex_security,emg) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
			//OSTimeDlyHMSM(0,0,0,20);
		}
	}

	return ret;
}



u8 emg_door_device(u32 id,u8 emg)
{
	emg_door_t temg;
	int length;
	u8 rssi,lqi;
	u32 Os_GetTime=0;
	u8 th;
	u8 carrier_sen_check=0;

	rf_lock();
	temg.hdr.master_id = gEnv.id.main;
	temg.hdr.slave_id = id;
	temg.hdr.payload = PAYLOAD_EMG_DOOR;
	temg.emg = emg;
	
	//set_freq();
	
	for(int i=0;i<10;i++)
	{
		if(i==5 || i==9)
		{
			th = 1;
		}
		else
		{
			th = 0;
		}
		

		carrier_sen_check = cc1101_tx(&temg,sizeof(emg_t),th);
	
		//Os_GetTime = OSTimeGet() + RF_TIME_OUT * OS_TICKS_PER_SEC;
		
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			Os_GetTime = OSTimeGet() + RF_TIME_OUT * 100;
		}
		else
		{
			Os_GetTime = OSTimeGet() + RF_TIME_OUT;
		}
	
	
		while(1)
		{
		
			length = cc1101_rx((u8 *)&temg,&rssi,&lqi);
			if(length > 0 )
			{
				if(temg.hdr.slave_id == id && temg.hdr.payload == PAYLOAD_EMG_DOOR)
				{
					rf_unlock();
					return true;
				}
			}
			cc1100_cmd_idle();

			if(Os_GetTime <= OSTimeGet())
			{
				break;
			}
		}
		
		if(length > 0)
		{
			break;
		}
	
	}
	
	
	rf_unlock();
	return false;
}


u8 rf_send_emg_door(u8 emg)
{
	u8 ret = true;
	
	if(gEnv.use.driver == true)
	{
		for(int i=0;i<7;i++)
		{
			if(emg_door_device(gEnv.id.driver,emg) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
		}
	}

	if(gEnv.ex_use.ex_driver == true)
	{
		for(int i=0;i<7;i++)
		{
			if(emg_door_device(gEnv.ex_id.ex_driver,emg) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
		}
	}
	
	if(gEnv.use.security == true)
	{
		for(int i=0;i<7;i++)
		{
			if(emg_door_device(gEnv.id.security,emg) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
		}
	}

	if(gEnv.ex_use.ex_security == true)
	{
		for(int i=0;i<7;i++)
		{
			if(emg_door_device(gEnv.ex_id.ex_security,emg) == false)
			{
				ret = false;
			}
			else
			{
				break;
			}
		}
	}

	
	return ret;
}


u8 rf_door_open_oper(u32 id)
{
	door_open_oper_t oper;
	int length;
	u8 rssi,lqi;
	u32 Os_GetTime=0;
	u8 th;
	u8 carrier_sen_check=0;
	

	rf_lock();
	oper.hdr.master_id = gEnv.id.main;
	oper.hdr.slave_id = id;
	oper.hdr.payload = PAYLOAD_DOOR_OPEN_OPER;
	
	
	// download setup data

	for(int i=0;i<10;i++)
	{	
		if(i==5 || i==9)
		{
			th = 1;
		}
		else
		{
			th = 0;
		}
		
		carrier_sen_check = cc1101_tx(&oper,sizeof(door_open_oper_t),th);
	
		//Os_GetTime = OSTimeGet() + RF_TIME_OUT * OS_TICKS_PER_SEC;
		
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			Os_GetTime = OSTimeGet() + RF_TIME_OUT * 50;	
		}
		else
		{
			Os_GetTime = OSTimeGet() + RF_TIME_OUT;
		}

	
		while(1)
		{

			length = cc1101_rx((u8 *)&oper,&rssi,&lqi);
			if(length > 0 )
			{
				if(oper.hdr.slave_id == id && oper.hdr.payload == PAYLOAD_DOOR_OPEN_OPER)
				{
					rf_unlock();
					return true;
				}
			}
			cc1100_cmd_idle();
		
			if(Os_GetTime <= OSTimeGet())
			{
				break;
			}
		}
	}
	
	rf_unlock();
	
	return false;
}


void sync2_ack(u32 id)
{
	sync2_t sync;
	set_freq();	

	sync.hdr.master_id = gEnv.id.main;	
	sync.hdr.slave_id = id;
	sync.hdr.payload = PAYLOAD_SYNC2;
	
	// download setup data
	switch(get_device_id(id))
	{
		case 1:
			sync.reserve[0] = gEnv.remocon.emg_on;
			sync.reserve[1] = gEnv.remocon.emg_off;
			sync.reserve[2] = gEnv.remocon.button_on;
			
			sync.reserve[3] = oper_get_emg_state();
			sync.reserve[4] = event_get_state() == RESET ? 0 : 2;
		
			break;
		case 2:
			
			sync.reserve[0] = gEnv.remocon.emg_on;
			sync.reserve[1] = gEnv.remocon.emg_off;
			sync.reserve[2] = gEnv.remocon.button_on;
			
			sync.reserve[3] = oper_get_emg_state();
			sync.reserve[4] = event_get_state() == RESET ? 0 : 2;
		
			break;
		case 3:
		case 4:
		case 5:
		case 6:
		
			sync.reserve[3] = oper_get_emg_state();
			sync.reserve[4] = event_get_state() == RESET ? 0 : 2;
			break;
	}
		
	set_freq();
	cc1101_tx(&sync,sizeof(sync2_t),1);
}	


void door_open_ack(u32 id,u8 unlock_done)
{
	set_freq();	
	
	door_open_t tdoor;
	
	tdoor.hdr.master_id = gEnv.id.main;
	tdoor.hdr.slave_id = id;
	tdoor.hdr.payload = PAYLOAD_DOOR_OPEN_CMD;
	
	tdoor.reserve[0] = unlock_done;
	
	set_freq();
	cc1101_tx(&tdoor,sizeof(door_open_t),1);
}	


void set_cmd_ack(u32 id)
{
	
	set_cmd_t cmd;
	
	cmd.hdr.master_id = gEnv.id.main;
	cmd.hdr.slave_id = id;
	cmd.hdr.payload = PAYLOAD_SET_CMD;
	set_freq();
	//OSTimeDlyHMSM(0,0,0,10); // Wait 10msec
	cc1101_tx(&cmd,sizeof(set_cmd_t),1);		

}	


void remocon_emg_ack(u32 id)
{
	
	remote_emg_t emg;
	
	emg.hdr.master_id = gEnv.id.main;
	emg.hdr.slave_id = id;
	emg.hdr.payload = PAYLOAD_REMOCON_EMG;
	set_freq();
	cc1101_tx(&emg,sizeof(remote_emg_t),1);
}	

void door_emg_ack(u32 id)
{
	
	door_emg_t emg;
	
	emg.hdr.master_id = gEnv.id.main;
	emg.hdr.slave_id = id;
	emg.hdr.payload = PAYLOAD_DOOR_EMG;
	set_freq();
	cc1101_tx(&emg,sizeof(door_emg_t),1);
}


void door_lock_status_ack(u32 id)
{
	
	door_lock_status_t status;
	
	status.hdr.master_id = gEnv.id.main;
	status.hdr.slave_id = id;
	status.hdr.payload = PAYLOAD_DOOR_LOCK_STATUS;
	set_freq();
	cc1101_tx(&status,sizeof(door_lock_status_t),1);
}
										
void door_status_ack(u32 id)
{
	
	door_status_t status;
	
	status.hdr.master_id = gEnv.id.main;
	status.hdr.slave_id = id;
	status.hdr.payload = PAYLOAD_DOOR_STATUS;
	set_freq();
	cc1101_tx(&status,sizeof(door_status_t),1);
}



void get_door_status_ack(u32 id)
{
	
	get_door_open_status_t status;
	
	status.hdr.master_id = gEnv.id.main;
	status.hdr.slave_id = id;
	status.hdr.payload = PAYLOAD_GET_DOOR_STATUS;
	status.door = event_get_door_state();
	status.doorlock = event_get_doorlock_state();
	set_freq();
	//OSTimeDlyHMSM(0,0,0,10); // Wait 10msec
	cc1101_tx(&status,sizeof(get_door_open_status_t),1);
}


void rssi_check_ack(u32 id)
{
	
	//door_status_t status;
	sync_version_t * sync_version;
	
	sync_version->hdr.master_id = gEnv.id.main;
	sync_version->hdr.slave_id = id;
	sync_version->hdr.payload = PAYLOAD_RSSI_CHECK;
	set_freq();
	cc1101_tx(&sync_version,sizeof(sync_version_t),1);
}



void id_check_ack(u32 id)
{
	
	door_status_t status;
	
	status.hdr.master_id = gEnv.id.main;
	status.hdr.slave_id = id;
	status.hdr.payload = PAYLOAD_SYNC_VERSION;
	set_freq();
	cc1101_tx(&status,sizeof(door_status_t),1);
}









int get_door_id(u32 id)
{
	if(gEnv.id.driver == id) return 0;
	if(gEnv.id.security == id) return 1;
	#if 1
	if(gEnv.id.b_door == id) return 2;
	if(gEnv.id.s_door == id) return 3;
	if(gEnv.id.cabin_left == id) return 4;
	if(gEnv.id.cabin_right == id) return 5;
	#endif
	
	#if 0
	if(gEnv.id.cabin_left == id) return 2;
	if(gEnv.id.cabin_right == id) return 3;
	if(gEnv.id.b_door == id) return 4;
	if(gEnv.id.s_door == id) return 5
	#endif
	
	return -1;	
}


packet_t gPacket;
extern  u8 g_device_door[4];
extern u8 temp_g_state;

void RFRecvTask(void *p_arg)
{
	u8 rssi,lpi;
	int length;
	u8 device_check_value;
	int valid_id;
	u8 unlock_done;
	u8 check_reset;
	
	#if 1
	
	OSTimeDlyHMSM(0,0,5,0);

	lcd_clear();
	lcd_position(0,0);
	lcd_prString("Please wait....");
	SoundPlay(34);
	
	//memset(gDoorStatus,0,12);
	//memset(gDoorLock,1,6);
		
//	while(1)
	{
		device_check_value = rf_init_device();
		
	}
	if(device_check_value != true)
	{
		// sound error message
		#if 0
		while(1);
		{
			OSTimeDlyHMSM(0,0,1,0);
		}
		#endif
		
		if(device_check_value==5)
		{
			SoundPlay(36);		//NG sound
		} 
		else if(device_check_value==6)
		{
			SoundPlay(37);		//NG sound
		}
		else if(device_check_value==7)
		{
			SoundPlay(38);		//NG sound			
		}
		else if(device_check_value==8)
		{
			SoundPlay(39);		//NG sound
		}
		else if(device_check_value==9)
		{
			SoundPlay(40);		//NG sound			
		}
		else if(device_check_value==10)
		{
			SoundPlay(41);		//NG sound					
		}
		


		
		//SoundPlay(36);		//NG sound
	}
	else
	{
		//OSTimeDlyHMSM(0,0,2,0);
		// amp on
		set_mute_off(true);
		set_s_down(false);

		Os_SoundModuleOnTime = OSTimeGet() + ((u32)10 * OS_TICKS_PER_SEC);
		
		SoundPlay(35);		//OK sound

	

		
	}
	#endif

	//memset(gDoorStatus,0,12);
	//memset(gDoorLock,1,6);

	OSTimeDlyHMSM(0,0,5,0);
	
	
	memset(gDoorStatus,0,12);
	memset(gDoorLock,1,6);

	lcd_clear();
	
	if(gEnv.main_set.maintenance_active == true)
	{
		operation_service();
	}
	

	while(1)
	{
		
		if(old_radio_mode!=radio_mode)
		{
			old_radio_mode = radio_mode;
		}



		if(event_get_state() == MAINTENCE)
		{
			OSTimeDlyHMSM(0,0,0,100);
			if(old_radio_mode!=radio_mode)
			{
				old_radio_mode = radio_mode;
			}
			else
			{
				radio_mode = RF_MENTENANCE;				
			}
		}
		else
		{
			if(radio_mode!=RF_TEST_CARRIER && radio_mode!=RF_TEST_MODURATION && radio_mode!=RF_TEST_ADJ_FREQ && radio_mode!=RF_TEST_ID_CHECK && radio_mode!= RF_POWER_DOWN && radio_mode!=RF_TEST_RSSI_CHECK && radio_mode!=RF_PAIRING_MODE)
			{
				if(radio_mode != RF_NOMAL_MODE)
				{
					radio_mode = RF_NOMAL_MODE;
				}
			}
			
		}
		
		
		if(radio_mode==RF_NOMAL_MODE)
		{		
			
			#if 1
			rf_lock();
			set_freq();
			length = cc1101_rx((u8 *)&gPacket,&rssi,&lpi);
			rf_unlock();
			
			
			if(length == sizeof(packet_t))
			{
				
				if(gPacket.hdr.master_id = gEnv.id.main || gPacket.hdr.payload == PAYLOAD_SYNC2)						
				{
					valid_id = get_device_id(gPacket.hdr.slave_id);
					if(valid_id == -1)
						continue;
					// remocon async
					switch(gPacket.hdr.payload)				
					{
						
						case PAYLOAD_SYNC2:
						{
							sync2_t * sync;
							
							sync = (sync2_t *)&gPacket;
							sync2_ack(gPacket.hdr.slave_id);
							
							break;
						}
						
						
						
						// B,C ->A 
						case PAYLOAD_DOOR_OPEN_CMD:
						{
							door_open_t * open;
							
							open = (door_open_t *)&gPacket;
							//door_open_ack(gPacket.hdr.slave_id);

							if((event_get_state() == DRIVING && open->door == 3)||(event_get_state() == DRIVING && open->door == 4)
							||(gEnv.main_set.reset_with_unlock_b_s_door_used == false && event_get_state()==SET && open->door == 3)
							||(gEnv.main_set.reset_with_unlock_b_s_door_used == false && event_get_state()==SET && open->door == 4))
							{
								SoundPlay(28);
								unlock_done = false;
								door_open_ack(gPacket.hdr.slave_id,unlock_done);
								break;
							}
							else
							{
								unlock_done = true;
							}

							oper_insert(OPER_RF_DOOR,open->door);
							
							door_open_ack(gPacket.hdr.slave_id,unlock_done);
							break;
						}
						
						case PAYLOAD_SET_CMD:
						{
							
							set_cmd_t * set;
							
							set = (set_cmd_t *)&gPacket;
							set_cmd_ack(gPacket.hdr.slave_id);
							//if(set->set == true)
							if(set->set == SET)
							{
								oper_insert(OPER_RF_SET,1);
							}
							else
							{
								#if 0
								if(gEnv.id.security == gPacket.hdr.slave_id && gEnv.main_set.security_reset_active == 0)
								{
									SoundPlay(28);
									break;
								}
								#endif
								
								oper_insert(OPER_RF_RESET,1);
								
							}
							break;
						}
						
						case PAYLOAD_REMOCON_EMG:
						{
							remote_emg_t * emg;
							
							emg = (remote_emg_t *)&gPacket;
							remocon_emg_ack(gPacket.hdr.slave_id);
							

							oper_insert(OPER_REMOCON_EMG,emg->emg);
							
							if(get_door_id(emg->hdr.slave_id) == 0)
							{
								set_d_emg_led(emg->emg);
							}
							else
							{
								set_g_emg_led(emg->emg);
							}
							
							
							break;
						}
						
						case PAYLOAD_DOOR_EMG:
						{
							volatile door_emg_t * emg;
							
							emg = (door_emg_t *)&gPacket;
							
							door_emg_ack(gPacket.hdr.slave_id);
							
							if(get_door_id(emg->hdr.slave_id) > 0)
							{
								// external
								if(emg->door == 0)
									gDoorStatus[get_door_id(emg->hdr.slave_id) * 2] = emg->emg;
								else
									gDoorStatus[get_door_id(emg->hdr.slave_id) * 2 + 1] = emg->emg;
							}
							
							
							
							break;
						}
						case PAYLOAD_DOOR_LOCK_STATUS:
						{
							door_lock_status_t * status;
							int id;
							
							status = (door_lock_status_t *)&gPacket;
							
							door_lock_status_ack(gPacket.hdr.slave_id);
							
							id = get_door_id(status->hdr.slave_id);
							
							temp_g_state = event_get_state();
							
							if(id > -1)
							{
								if(status->lock == 1)
								{
									if(id == 4 || id == 5)
									{
										set_cavin_lock_led(true);
										//set_cavin_door_led(false);
									}
									else if(id == 2)
									{	
										set_reardoor_lock_led(true);
									}
									else if(id == 3)
									{
										set_sidedoor_lock_led(true);
									}
		
									//gDoorLock[id - 2] = status->lock;								
										
									if(temp_g_state == RESET)
									{
										event_set_state(RESET);
									}
									else
									{
										#if 0
										//if(g_state_drive_door[0] == DRVR_DOOR_IDLE && g_state_drive_door[1] == DRVR_DOOR_IDLE
										//&& gDoorStatus[4] == false && gDoorStatus[5] == false && gDoorStatus[6] == false && gDoorStatus[7] == false)
										//&& gDoorLock[0] == true && gDoorLock[1] == true && gDoorLock[2] == true && gDoorLock[3] == true)
										
										if(g_state_drive_door[0] == DRVR_DOOR_IDLE && g_state_drive_door[1] == DRVR_DOOR_IDLE
										&& gDoorStatus[4] == false && gDoorStatus[5] == false && gDoorStatus[6] == false && gDoorStatus[7] == false)									
										{
											if(event_get_state() != DRIVING)
											{
												event_set_state(SET);
											}

										}
										#endif
									}										
								}
								//gDoorLock[id - 2] = status->lock;
								gDoorLock[id] = status->lock;
							}
						
							
							
							break;
						}
						case PAYLOAD_DOOR_STATUS:
						{
							int id;
							door_status_t * status;
							
							status = (door_status_t *)&gPacket;

							door_status_ack(gPacket.hdr.slave_id);

							id = get_door_id(status->hdr.slave_id);
							
							if(id > -1)
							{
								gDoorStatus[(id * 2) + status->door] = status->status;
								#if 0
								if(id == 4 || id == 5)
									set_cavin_door_led(status->status);
								else if(id == 0)
									set_re_door_led(status->status);
								else if(id == 1)
									set_se_door_led(status->status);
								#endif
							}
							break;
						}
						
						case PAYLOAD_GET_DOOR_STATUS:
						{
							int id;
							door_status_t * status;
							
							status = (door_status_t *)&gPacket;
							
							get_door_status_ack(gPacket.hdr.slave_id);

							break;
						}
					}
				}
			}
			
			#endif
		}
		else if(radio_mode==RF_TEST_CARRIER || radio_mode==RF_TEST_MODURATION || radio_mode==RF_TEST_ADJ_FREQ || radio_mode==RF_TEST_ID_CHECK || radio_mode==RF_TEST_RSSI_CHECK)
		{
			cc1100_cmd_idle();
			OSTimeDlyHMSM(0,0,1,0);			
			
			if(radio_mode==RF_TEST_CARRIER)
			{
				test_carrier_wave_mode();
			}
			else if(radio_mode==RF_TEST_MODURATION)
			{

				test_wave_mode();				
			}
			else if(radio_mode==RF_TEST_ADJ_FREQ)
			{
				setup_oper();		
			}
			else if(radio_mode==RF_TEST_ID_CHECK)
			{
				test_id_check();
			}
			else if(radio_mode==RF_TEST_RSSI_CHECK)
			{
				int m_rssi=0;
				int m_lqi=0;
				int s_rssi=0;
				int s_lqi=0;
				
				//for(int i=0;i<5;i++)
				{
					test_rssi_check(&m_rssi,&m_lqi,&s_rssi,&s_lqi);
				}
							

			}
		}
		else if(radio_mode==RF_POWER_DOWN)
		{
			
			OSTimeDlyHMSM(0,0,0,100);		
		}
		else if(radio_mode==RF_PAIRING_MODE)
		{
			pairing_device();

		}
	}
}





void test_carrier_wave_mode(void)
{
	
	
	while(1)
	{
		
		set_freq();
		cc1101_tx_carrier();
		OSTimeDlyHMSM(0,0,3,0);
		/* IDLE */
		cc1100_cmd_idle();

		OSTimeDlyHMSM(0,0,3,0);
		
		if(radio_mode!=RF_TEST_CARRIER)
		{
			break;
		}
		
	}
}




void test_wave_mode(void)
{
	int i;
	int old,new;


	cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);
	cc1101_init_reg();
	cc1101_8PATABLE_write_reg();
	cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);


	while(1)
	{

		cc1100_cmd_idle();

		OSTimeDlyHMSM(0,0,0,100);

		set_freq();
		cc1101_rtx_reg_set(0); // tx
		cc1100_write_reg(CC1100_REG_PKTCTRL0,0x22);	//random TX mode
		cc1101_carrier_wave_setup();
		cc1100_cmd_idle();
		cc1100_cmd_calibrate();
		cc1100_cmd_tx();



		
		OSTimeDlyHMSM(0,0,3,0);
		cc1100_cmd_idle();
		OSTimeDlyHMSM(0,0,3,0);	
		//delay_ms(3000);
		//wait_input_freq_adjust();
		if(radio_mode!=RF_TEST_MODURATION)
		{
			break;
		}

	}
}



void pairing_device(void)
{
	u8 rssi,lpi;
	char result[10];
	int length;
	char ver_major[10];
	char ver_sub[10];
	char ver_subsub[10];
	char result02[10];
	int rssi_fixed;


	#if 1

	while(1)
	{

		rf_lock();
		set_freq();
		
		length = cc1101_rx((u8 *)&gPacket,&rssi,&lpi);
		
		if(length > 0)
		{
			id_check_ack(gPacket.hdr.slave_id);
		}

		
		sync_version_t * sync_version;
		sync_version = (sync_version_t *)&gPacket;
		
		rf_unlock();
		
		if(length == sizeof(packet_t) && gPacket.hdr.payload == PAYLOAD_SYNC_VERSION)
		{
			//sprintf(result,"%08lx",gPacket.hdr.slave_id);
			if(devicePairingNumber==OPER_PAIRING_REMO_DRIVER)
			{
				gEnv.id.driver = gPacket.hdr.slave_id;
				gEnv.use.driver = true;
			}
			else if(devicePairingNumber==OPER_PARING_REMO_SECURITY)
			{
				gEnv.id.security = gPacket.hdr.slave_id;
				gEnv.use.security = true;
			}
			if(devicePairingNumber==OPER_PAIRING_EX_REMO_DRIVER)
			{
				//gEnv.id.driver = gPacket.hdr.slave_id;
				gEnv.ex_id.ex_driver = gPacket.hdr.slave_id;
				gEnv.ex_use.ex_driver = true;
			}
			else if(devicePairingNumber==OPER_PARING_EX_REMO_SECURITY)
			{
				//gEnv.id.security = gPacket.hdr.slave_id;
				gEnv.ex_id.ex_security = gPacket.hdr.slave_id;
				gEnv.ex_use.ex_security = true;
			}
			else if(devicePairingNumber==OPER_PARING_DOORCTL_SIDE)
			{
				gEnv.id.s_door = gPacket.hdr.slave_id;
				gEnv.use.s_door = true;
			}
			else if(devicePairingNumber==OPER_PARING_DOORCTL_BACK)
			{
				gEnv.id.b_door = gPacket.hdr.slave_id;
				gEnv.use.b_door = true;
			}
			else if(devicePairingNumber==OPER_PARING_DOORCTL_CABIN_RIGHT)
			{
				gEnv.id.cabin_right = gPacket.hdr.slave_id;
				gEnv.use.cabin_right = true;
			}
			else if(devicePairingNumber==OPER_PARING_DOORCTL_CABIN_LEFT)
			{
				gEnv.id.cabin_left = gPacket.hdr.slave_id;
				gEnv.use.cabin_left = true;
			}
			
			radio_mode=RF_NOMAL_MODE;
			env_save(&gEnv);
	
			#ifdef NEW_TEST_BUZZER_LIMIT
			gOs_BuzzerTimeLimit_GetTime = OSTimeGet() + OS_TICKS_PER_SEC * 5;
			#endif
			
			for(int i=0;i<5;i++)
			{
				set_buzzer_drv(true);
				OSTimeDlyHMSM(0,0,0,100);
				set_buzzer_drv(false);
				OSTimeDlyHMSM(0,0,0,100);
			}
			#ifdef NEW_TEST_BUZZER_LIMIT
			gOs_BuzzerTimeLimit_GetTime = 0;
			#endif
		
		}
		else if(length == sizeof(packet_t))
		{
			for(int i=0;i<5;i++)
			{
				if(devicePairingNumber == OPER_PAIRING_REMO_DRIVER)
				{
					set_driver_lock_led(true);
				}
				else if(devicePairingNumber == OPER_PARING_REMO_SECURITY)
				{
					set_driver_lock_led(true);
				}
				if(devicePairingNumber == OPER_PAIRING_EX_REMO_DRIVER)
				{
					set_driver_lock_led(true);
				}
				else if(devicePairingNumber == OPER_PARING_EX_REMO_SECURITY)
				{
					set_driver_lock_led(true);
				}
				else if(devicePairingNumber == OPER_PARING_DOORCTL_CABIN_RIGHT)
				{
					set_cavin_lock_led(true);
				}
				else if(devicePairingNumber == OPER_PARING_DOORCTL_CABIN_LEFT)
				{
					set_cavin_lock_led(true);
				}
				else if(devicePairingNumber == OPER_PARING_DOORCTL_SIDE)
				{
					set_sidedoor_lock_led(true);
				}
				else if(devicePairingNumber == OPER_PARING_DOORCTL_BACK)
				{
					set_reardoor_lock_led(true);
				}
			
				OSTimeDlyHMSM(0,0,0,100);
			
				set_driver_lock_led(false);
				set_driver_lock_led(false);
				set_cavin_lock_led(false);
				set_cavin_lock_led(false);
				set_sidedoor_lock_led(false);
				set_reardoor_lock_led(false);
				OSTimeDlyHMSM(0,0,0,100);
			}
		}
		
		if(radio_mode!=RF_PAIRING_MODE)
		{
			Os_PairingDeviceOnTime = 0;
			event_set_state(RESET);
			lcd_clear();
			break;
		}
		
	}
	
	#endif
}



void test_id_check(void)
{
	u8 rssi,lpi;
	char result[10];
	int length;
	char ver_major[10];
	char ver_sub[10];
	char ver_subsub[10];
	char result02[10];
	int rssi_fixed;


	#if 1

	while(1)
	{

		rf_lock();
		set_freq();
		
		length = cc1101_rx((u8 *)&gPacket,&rssi,&lpi);
		
		if(length > 0 && gPacket.hdr.payload == PAYLOAD_SYNC_VERSION)
		{
			id_check_ack(gPacket.hdr.slave_id);
		}

		
		sync_version_t * sync_version;
		sync_version = (sync_version_t *)&gPacket;
		
		rf_unlock();
		
		if(length == sizeof(packet_t))
		{
			sprintf(result,"%08lx",gPacket.hdr.slave_id);
			
			
			if(rssi > 128)
			{
				rssi_fixed = (((char)rssi - 256)/2) - 74;
			}
			else
			{
				rssi_fixed = ((char)rssi/2) - 74;
			}
			
			
			//if(rssi_fixed > 128)
			//{
			//	rssi_fixed = rssi_fixed - 128;
			//	sprintf(result02,"n%02d",rssi_fixed);
			//}
			//else
			{
				sprintf(result02,"%02d",rssi_fixed);
				
			}
			

			
			lcd_clear();

			if(sync_version->hdr.payload == PAYLOAD_SYNC_VERSION)
			{
				sprintf(ver_major,"%01d",sync_version->ver_major);
				sprintf(ver_sub,"%01d",sync_version->ver_sub);
				sprintf(ver_subsub,"%01d",sync_version->ver_sub_sub);
				
				lcd_position(0,0);
				lcd_prCString("ver.");
				lcd_prCString(ver_major);
				lcd_prCString(".");
				lcd_prCString(ver_sub);
				lcd_prCString(".");
				lcd_prCString(ver_subsub);
				//lcd_prCString("device ID");
			}
			lcd_position(1,0);
			lcd_prCString(result);
			lcd_prCString("  ");
			lcd_prCString(result02);
			OSTimeDlyHMSM(0,0,5,0);
			lcd_clear();
			lcd_position(0,0);
			lcd_prCString("Please next id...");
			
		}
		
		if(radio_mode!=RF_TEST_ID_CHECK)
		{
			lcd_clear();
			break;
		}
		
	}
	
	#endif
}








void test_rssi_check(u8 * m_rssi, u8 * m_lqi,u8 * s_rssi, u8 * s_lqi)
{
	u8 rssi,lpi;
	char result[10];
	int length;
	char ver_major[10];
	char ver_sub[10];
	char ver_subsub[10];
	char result02[10];
	u8 s_rssi_org;
	int s_rssi_fixed;
	char s_rssi_fixed02[10];
	int m_rssi_fixed;
	char m_rssi_fixed02[10];
	int rssi_cnt=0;
	u32 old_id;


	#if 1

	while(1)
	{

		rf_lock();
		set_freq();
	
		length = cc1101_rx((u8 *)&gPacket,&rssi,&lpi);
		

		//s_rssi_org = gPacket.hdr.rssi;

		
		
		sync_version_t * sync_version;
		sync_version = (sync_version_t *)&gPacket;
		
		rf_unlock();
		
		if(length == sizeof(packet_t) && gPacket.hdr.payload == PAYLOAD_RSSI_CHECK)
		{
			rssi_check_ack(gPacket.hdr.slave_id);
			
			if(rssi_cnt > 3)
			{
				s_rssi_org = gPacket.hdr.rssi;

				
				sprintf(result,"%08lx",gPacket.hdr.slave_id);
				
				
				#if 1
				//if(gPacket.hdr.rssi > 128)
				if(rssi > 128)
				{
					m_rssi_fixed = (((int)rssi - 256)/2) - 74;
				}
				else
				{
					m_rssi_fixed = ((int)rssi/2) - 74;
				}
				
				sprintf(m_rssi_fixed02,"%02d",m_rssi_fixed);
				#endif
				

				
				#if 1
				if(s_rssi_org > 128)
				{
					s_rssi_fixed = (((int)s_rssi_org - 256)/2) - 74;
				}
				else
				{
					s_rssi_fixed = ((int)s_rssi_org/2) - 74;
				}
				
				sprintf(s_rssi_fixed02,"%02d",s_rssi_fixed);
				#endif



				//rssi_check_ack(gPacket.hdr.slave_id);			
				
				
				
				
				
				
				//sprintf(s_rssi_fixed02,"%02d",gPacket.hdr.rssi);
				lcd_clear();


				lcd_position(0,0);
				lcd_prCString(result);
				#if 1
				lcd_position(1,0);
				lcd_prCString("M:");
				lcd_prCString(m_rssi_fixed02);
				lcd_prCString("  ");
				lcd_prCString("S:");
				lcd_prCString(s_rssi_fixed02);
				#endif
				OSTimeDlyHMSM(0,0,5,0);
				lcd_clear();
				lcd_position(0,0);
				lcd_prCString("Please next...");

				
				rssi_cnt=0;

			}

			rssi_cnt++;
			
			if(old_id != gPacket.hdr.slave_id)
			{
				rssi_cnt=0;
			}		
			old_id=gPacket.hdr.slave_id;

			
			
			
		}
		
		if(radio_mode!=RF_TEST_RSSI_CHECK)
		{
			lcd_clear();
			break;
		}
		
	}
	
	#endif
}






void adjust_data_preset(adjust_t * adjust_data)
{
		
	adjust_data->id[0] = 'm';
	adjust_data->id[1] = 'e';
	adjust_data->id[2] = '2';

	adjust_data->adj_freq			= gTele_env.ctl_data.adj_freq;
	adjust_data->adj_temper			= gTele_env.ctl_data.adj_temper;
	adjust_data->freq_add_sub		= gTele_env.ctl_data.freq_add_sub;
	adjust_data->temper_add_sub		= gTele_env.ctl_data.temper_add_sub;

	adjust_data->Device_type = DEV_MAIN;
	adjust_data->prog_ver_major = PRG_VERSION_MAJOR;
	adjust_data->prog_ver_sub	= PRG_VERSION_SUB;

	//hex_change_level_02();
	
	adjust_data->hdr.payload	= PAYLOAD_ADJUST_SYNC;
	adjust_data->hdr.master_id	= gEnv.id.main;
}




#if 1

u8 setup_oper(void)
{
	u8	ret;
	u8 rssi,lpi;
	int length;

	adjust_t * p_adjust_data;
	
	
	p_adjust_data = &adjust_data;

	
	while(1)
	{
		//OSTimeDlyHMSM(0,0,3,0);
		adjust_data_preset(p_adjust_data);
		//set_freq();
		ret = cc1101_tx(p_adjust_data,sizeof(adjust_t),0);
		
		if(ret == 1)
		{
			check06 = cc1100_read_status(CC1100_REG_MARCSTATE);		
				
			//length = cc1101_rx((u8 *)adjust_t,0);
			length = cc1101_rx((u8 *)&p_adjust_data,&rssi,&lpi);
			if(length > 0)
			{
				//gggLength = length;
				if((length - 2) == sizeof(adjust_t))
				{
					
					if(p_adjust_data->hdr.payload == PAYLOAD_ADJUST_WRITE)
					{
						#ifdef NEW_TEST_BUZZER_LIMIT
						gOs_BuzzerTimeLimit_GetTime = OSTimeGet() + OS_TICKS_PER_SEC * 5;
						#endif
						
						for(int i=0;i<3;i++)
						{
							set_buzzer_drv(true);
							OSTimeDlyHMSM(0,0,0,50);
							set_buzzer_drv(false);
							OSTimeDlyHMSM(0,0,0,50);
						}
						
						#ifdef NEW_TEST_BUZZER_LIMIT
						gOs_BuzzerTimeLimit_GetTime = 0;
						#endif

						gEnv.id.main = p_adjust_data->hdr.master_id;					
					}


					
					
					
					
					
					#if 0
					memcpy((u8 *)&gEnv.id.main,(u8 *)&p_adjust_data->f_option.env_data[0],4);
					

					gTele_env.ctl_data.adj_freq		=	p_adjust_data->f_option.env_data[4];
					gTele_env.ctl_data.freq_add_sub =	p_adjust_data->f_option.env_data[5];
					gTele_env.ctl_data.adj_temper	=	p_adjust_data->f_option.env_data[6];
					gTele_env.ctl_data.temper_add_sub =	p_adjust_data->f_option.env_data[7];

					
					hex_change_level(p_adjust_data->f_option.level);
					#endif
					tele_env_save(&gTele_env);
					env_save(&gEnv);
					
					
					
					cc1101_tx_carrier();
					OSTimeDlyHMSM(0,0,5,0);
					return 1;
				}
			}
		}
	}
}



#endif
