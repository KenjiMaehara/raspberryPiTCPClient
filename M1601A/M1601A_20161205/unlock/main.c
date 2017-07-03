/*
 * 
 *
 * Created: 
 *  Author: 
 */ 

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <mytype.h>
#include <avr/wdt.h>
#include "cc1100.h"

#include <util/delay.h>
#include "port.h"
#include "env.h"
#include "spi.h"


volatile u32 gMasterId;
volatile u8 buzzer_toggle = false;
volatile u8 set_state=false;


volatile u8 gInternalState,gExternState;
volatile u32 gGlobalTimer = 0;
volatile u32 gGlobal500MsTimer = 0;

volatile u8 test_mode;
volatile int btn_door_lock_test_count=-1;
volatile u8 btn_door_lock_test;

volatile int sleep_cansel_count=-1;
volatile int lock_err_e_door_count=-1;
volatile int lock_err_i_door_count=-1;

volatile int close_chk_e_door_count=-1;
volatile int close_chk_i_door_count=-1;
volatile int mentenece_mode = -1;

volatile int cnt_wait_syn_ack=-1;

volatile int devicePowerStart=true;

volatile int checkTwiceDoorWaitCount=-1;
//volatile int test_count=0;
volatile int lockActiveTime=-1;


#define CARRIER_SENSE_DETECT	0x55
#define RF_TIME_OUT 10

//#define UNLOCK_DEBUG

enum DOOR_LOCK_STATE
{
	LOCK=0,
	LOCK_ERROR,
	UNLOCK_SET,
	UNLOCK_END_WAIT,
	UNLOCK,
	UNLOCK_EXTERN_DOOR_OPEN,
	UNLOCK_EXTERN_DOOR_CLOSE,
	LOCK_SET,
	LOCK_END_WAIT
};



#if 1
enum DOOR_CHECK_STATE
{
	IDLE_STATUS=0,
	DOOR_DELAY_STATUS,
	DOOR_OPEN_STATUS,
	DOOR_CLOSE_WAIT_STATUS,
	ERROR_STATUS
};
#endif





void delay_us(u8 time_us)		/* time delay for us(1~255 us) for 32MHz */
{
	register unsigned char i;

	for(i = 0; i < time_us; i++)			// 4 cycles
	{ asm volatile("PUSH R0");			// 1 cycles
		asm volatile("NOP    ");			// 1 cycles
		asm volatile("POP  R0");			// 2 cycles
		asm volatile("PUSH R0");			// 1 cycles
		asm volatile("NOP    ");			// 1 cycles
		asm volatile("POP  R0");			// 2 cycles
		asm volatile("PUSH R0");			// 1 cycles
		asm volatile("NOP    ");			// 1 cycles
		asm volatile("POP  R0");			// 2 cycles
		asm volatile("PUSH R0");			// 1 cycles
		asm volatile("NOP    ");			// 1 cycles
		asm volatile("POP  R0");			// 2 cycles
		asm volatile("PUSH R0");			// 1 cycles
		asm volatile("NOP    ");			// 1 cycles
		asm volatile("POP  R0");			// 2 cycles
		asm volatile("PUSH R0");			// 1 cycles
		asm volatile("NOP    ");			// 1 cycles
		asm volatile("POP  R0");			// 2 cycles
		asm volatile("PUSH R0");			// 1 cycles
		asm volatile("NOP    ");			// 1 cycles
		asm volatile("POP  R0");			// 2 cycles = total 32 cycles
	}
}

void delay_ms(u16 time_ms)		/* time delay for ms(1~65535 ms) for 32MHz */
{
	register unsigned int i;

	for(i = 0; i < time_ms; i++)
	{ 
		delay_us(250);
		delay_us(250);
		delay_us(250);
		delay_us(250);
	}
}


void setClockTo32MHz() 
{
	CCP = CCP_IOREG_gc;              // disable register security for oscillator update
	OSC.CTRL = OSC_RC32MEN_bm;       // enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
	CCP = CCP_IOREG_gc;              // disable register security for clock update
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock
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
	_delay_ms(100);

	//while(1);
	temp = read_adc();

	_delay_ms(100);

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
	

	//fr = 0x00;//get_freq();
	fr = ~get_freq()  & 0x0f;

	frq = 0x66ac;

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

	frq += level_offset[5level];
	#endif



	
	frq += (fr * 0x003f);

	offset = 0x0339 - 0;
	
	level = 13 - (offset / 0x0c);
	

	if(offset & 0x8000)
	level = 13;
	else if((offset / 0x0c)>13)
	level = 0;
	//26867
	cc1101_set_freq(frq);
}





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
	
	PORTA.PIN7CTRL = 0x18; // pullup , both edge
	
	delay_ms(10);

	if(PORTA.IN & 0x80)
	{
		cc1100_cmd_idle();
		
		if(th == 0)
		{
			return CARRIER_SENSE_DETECT;
		}

	}

	PORTA.PIN7CTRL = 0x00;


	cc1101_rtx_reg_set(0);
	cc1101_carrier_wave_setup();
	cc1100_cmd_idle();
	cc1100_cmd_calibrate();
	tx_fifo_write(data,length);
	
	cc1100_write_reg(CC1100_REG_MCSM1,0x00);
	cc1100_cfg_gdo0(0x09);



	cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);
	cc1100_cmd_tx();
	while(!get_gdoa());	//data send start
	while(get_gdoa());		//data send end
	
	//for(int i=0;i<4;i++)
	{
	//	_delay_ms(500);	
	}

	
	return 1;
}

//#define WOR


ISR(PORTA_INT1_vect)
{


}

ISR(PORTC_INT1_vect)
{


}




//#define WOR

#define	RX_WOR		1
#define	RX_NOMAL	2




#define sleep() __asm__ __volatile__ ("sleep")

u8 cc1101_rx(u8 * data,u8 * rssi, u8 * lqi, u8 rx_type)
{
	//int cnt_wait_syn_ack=0;
	u8 length;
	u8 RX_STATUS = RX_FIFO_NONE;
	
	cc1100_cmd_idle();
	set_freq();

	
	cc1101_rtx_reg_set(1);
	cc1101_carrier_wave_setup();
	cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);

	cc1100_cmd_idle();
	cc1100_cmd_flush_rx();
	cc1100_cmd_calibrate();
	cc1100_cmd_rx();
	
	if(rx_type == RX_NOMAL)
	{
		cnt_wait_syn_ack=0;
		while(get_gdoa() == false)
		{
			//_delay_ms(1);
			if(cnt_wait_syn_ack > 50)
			{
				break;
			}

		}
		
		cnt_wait_syn_ack = 0;
		
		while(1)
		{
			if(get_gdoa()==true || RX_STATUS==RX_FIFO_MAKING || RX_STATUS==RX_FIFO_COMPLETE)
			{
				
				if(get_gdoa() == false || RX_STATUS==RX_FIFO_COMPLETE)
				{
					if((cc1100_status_crc_lqi() & 0x80))
					{
						length = rx_fifo_read(data,rssi,lqi);
						cnt_wait_syn_ack = -1;
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
			
			_delay_ms(1);
			if(cnt_wait_syn_ack > 100)
			{
				cnt_wait_syn_ack = -1;
				break;
			}
		}
		return 0;
	}
	else
	{
		//WOR

		PORTA.INT1MASK=0x9E;
		PORTA.INTCTRL=0x0C;
		cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);
		cc1100_write_reg(CC1100_REG_WOREVT1, 0x20);
		cc1100_write_reg(CC1100_REG_WOREVT0, 0xff);
		cc1100_write_reg(CC1100_REG_WORCTRL, 0x08);
		cc1100_write_reg(CC1100_REG_MCSM0, 0x18);
		cc1100_write_reg(CC1100_REG_MCSM2, 0x00);
		
		cc1100_strobe_cmd(0x38);		//WOR COMMAND
		
		wdt_disable();

		//SLEEP_CTRL |= SLEEP_SMODE_PDOWN_gc;
		SLEEP_CTRL |= SLEEP_SMODE_PSAVE_gc;
		SLEEP_CTRL |= SLEEP_SEN_bm;
		
		sleep();
		
		PORTA.INT1MASK=0x00;

		wdt_enable(0x26);		//wdt timeout 4sec enable
		
		#if 1
		if(get_door_lock_test()==true || get_door_unlock_test()==true || get_extern_door_lock()==false || get_internal_door_lock()==false)
		{
			return 0x77;
		}
		#endif
		
		if((cc1100_status_crc_lqi() & 0x80))
		{
			length = rx_fifo_read(data,rssi,lqi);
			return length;
		}
		
		return 0;
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


void timer_setup(void)
{
	/* Set up Timer/Counter 0 to work from CPUCLK/64, with period 10000 and
	 * enable overflow interrupt.
	 */
	//TCC0.PER = 250;
	//TCC0.PER = 125;
	TCC0.PER = 500;
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV64_gc;
	TCC0.INTCTRLA = ( TCC0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_MED_gc;
}


volatile u8 buz_toggle = false;
volatile u8 toggle = false;
volatile int timer_1S_count = 0;
volatile int send_timer=-1;
volatile u8 gBuzzer;
volatile int deny_setup_id = 0;
volatile int timer_100ms_count=0;

ISR(TCC0_OVF_vect)
{
	if(send_timer > -1)
	{
		send_timer++;
		
	}
	
	if(cnt_wait_syn_ack > -1)
	{
		cnt_wait_syn_ack++;
	}
	
	if(gEnv.beep_b_s_door_used == true)
	{
		if(buzzer_toggle == true)
		{
			if(timer_1S_count == 0)
			{
				buzzer_on(true);
			}
			else if(timer_1S_count == 500 )
			{
				buzzer_on(false);
			}
		}
	}
	
	
	//if(timer_100ms_count++ > 200)
	if(timer_100ms_count++ > 100)
	{
		timer_100ms_count=0;

		if(close_chk_e_door_count > -1)
		{
			close_chk_e_door_count++;
		}
		
		if(close_chk_i_door_count > -1)
		{
			close_chk_i_door_count++;
		}
	}
	
	
	//if(timer_1S_count == 500)
	//	gGlobal500MsTimer++;
	
	
	
	//if(timer_1S_count++ > 2777)		2777 ---> 1.6sec
	//if(timer_1S_count++ > 2000)
	if(timer_1S_count++ > 1000)
	{

		#if 0
		if(gGlobalTimer & 0x01)
		{
			set_room_light(true);
		}
		else
		{
			set_room_light(false);
		}
		#endif	

		
		timer_1S_count  = 0;
		gGlobalTimer++;
		gGlobal500MsTimer++;
		
		if(btn_door_lock_test_count > -1)
			btn_door_lock_test_count++;
		
		
		if(sleep_cansel_count > -1)
			sleep_cansel_count++;
			
		if(sleep_cansel_count > 10)
			sleep_cansel_count = -1;
			
			
		if(lock_err_e_door_count > -1)
			lock_err_e_door_count++;

		if(lock_err_i_door_count > -1)
			lock_err_i_door_count++;
		
		if(deny_setup_id < 600)
			deny_setup_id++;
		
		if(checkTwiceDoorWaitCount > -1)
			checkTwiceDoorWaitCount++;
			
		if(lockActiveTime > -1)
			lockActiveTime++;
		


		if(lockActiveTime > 2)
		{
			#ifndef UNLOCK_DEBUG
			set_cabin_leftdoor(STOP);
			set_cabin_leftdoor(STOP2);
			lockActiveTime=-1;
			#else
			set_communication_led(false);
			lockActiveTime=-1;
			#endif				
		}
		
		
		if(mentenece_mode > -1)
		{
			if(mentenece_mode == 0)
			{
				set_unlock_led(true);
				
				if((~get_dly() & 0x0f)==0x06)
				{
					set_lock_led(true);
				}
			}
			else if(mentenece_mode == 1)
			{
				set_unlock_led(false);
		
				if((~get_dly() & 0x0f)==0x06)
				{
					set_lock_led(false);
				}
			}
			

			
			if(mentenece_mode == 1)
			{
				mentenece_mode = 0; 
			}
			else
			{
				mentenece_mode++;
			}
			
		}
		
	}
}



void buzzer_error(void)
{
	
	
	
}

void buzzer_on(u8 buz)
{
	set_buzzer(buz);
}



/*
F,G,D,E -> A door status
PAYLOAD_DOOR_STATUS
*/

u8 door_status(u8 type,u8 status)
{
	int length;
	door_status_t door;
	u8 rssi,lqi;
	u32 GetTime=0;
	u8	th;
	u8	carrier_sen_check=0;	
	
	
	for(int i=0;i<7;i++)
	{
		wdt_reset();
		
		if(gEnv.unlockRootvanWaveUsed == true)
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
		
		
		
		
		door.hdr.master_id = gMasterId;
		door.hdr.slave_id = gEnv.id;
		door.hdr.payload = PAYLOAD_DOOR_STATUS;
		door.status = status;
		door.door = type;

		if(i==4 || i==6)
		{
			th=1;
		}
		else
		{
			th=0;
		}

		set_freq();
		
		carrier_sen_check = cc1101_tx(&door,sizeof(door_status_t),th);
		
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			GetTime = 1000;
		}
		else
		{
			GetTime = RF_TIME_OUT;
		}		
		
		send_timer = 0;
		
		while(1)
		{
			
			set_freq();
			length = cc1101_rx((u8 *)&door,&rssi,&lqi,RX_NOMAL);
			if(length > 0 )
			{
				if(door.hdr.slave_id == gEnv.id && door.hdr.payload == PAYLOAD_DOOR_STATUS)
				{
					send_timer = -1;
					comm_led();
					return true;
				}
			}
			cc1100_cmd_idle();
			
			if(send_timer > GetTime)
			{
				send_timer = -1;
				break;
			}
			// need timeout;
		}
	
	}
	
	
	return false;
}

/*
F,G,D,E -> A door lock status
PAYLOAD_DOOR_LOCK_STATUS
*/

u8 door_lock_status(u8 status)
{
	int length;
	door_lock_status_t door;
	u8 rssi,lqi;
	u32 GetTime=0;
	u8	th;
	u8	carrier_sen_check=0;

	for(int i=0;i<20;i++)
	{
		wdt_reset();
		
		if(gEnv.unlockRootvanWaveUsed == true)
		{
			if(i==3 || i==8 || i==13)
			{
				pt_data[0] = 0x34;		//output power -10dBm
				cc1101_8PATABLE_write_reg();
			}
			else if(i==4 || i==9 || i==14)
			{
				pt_data[0] = 0x1d;		//output power -15dBm
				cc1101_8PATABLE_write_reg();
			}
			else if(i==5 || i==10 || i==15)
			{
				pt_data[0] = 0x0d;		//output power -20dBm
				cc1101_8PATABLE_write_reg();
			}
			else
			{
				pt_data[0] = 0x60;		//output power 0dBm
				cc1101_8PATABLE_write_reg();
			}
		}		
		
		
		door.hdr.master_id = gMasterId;
		door.hdr.slave_id = gEnv.id;
		door.hdr.payload = PAYLOAD_DOOR_LOCK_STATUS;
		door.lock = status;

		if(i==4 || i==19)
		{
			th=1;
		}
		else
		{
			th=0;
		}
		
		set_freq();
		carrier_sen_check = cc1101_tx(&door,sizeof(door_lock_status_t),1);
	
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			GetTime = 1000;
		}
		else
		{
			GetTime = RF_TIME_OUT;
		}
	
		send_timer = 0;
		
		while(1)
		{
			set_freq();
			length = cc1101_rx((u8 *)&door,&rssi,&lqi,RX_NOMAL);
			if(length > 0 )
			{
				if(door.hdr.slave_id == gEnv.id && door.hdr.payload == PAYLOAD_DOOR_LOCK_STATUS)
				{
					send_timer = -1;
					comm_led();
					return true;
				}
			}
			cc1100_cmd_idle();
			
			if(send_timer > GetTime)
			{
				send_timer = -1;
				break;
			}
			// need timeout;
		}
	}
	
	
	return false;
}

/*
D,E,F,G -> A door_emg
PAYLOAD_DOOR_EMG
*/
u8 emg_send(u8 type,u8 emg)
{
	int length;
	door_emg_t temg;
	u8 rssi,lqi;
	
	temg.hdr.master_id = gMasterId;
	temg.hdr.slave_id = gEnv.id;
	temg.hdr.payload = PAYLOAD_DOOR_EMG;
	temg.emg = emg;
	temg.door = type;

	set_freq();
	cc1101_tx(&temg,sizeof(door_emg_t),1);
	
	send_timer = 0;

	while(1)
	{
		
		set_freq();
		length = cc1101_rx((u8 *)&temg,&rssi,&lqi,RX_NOMAL);
		if(length > 0 )
		{
			if(temg.hdr.slave_id == gEnv.id && temg.hdr.payload == PAYLOAD_DOOR_EMG)
			{
				send_timer = -1;
				comm_led();
				return true;
			}
		}
		cc1100_cmd_idle();
		
		if(send_timer > 1000)
		{
			send_timer = -1;
			break;
		}
		// need timeout;
	}
	
	
	return false;
}


/*
sync ack
*/

void sync_ack(void)
{
	sync_t sync;
	
	sync.hdr.master_id = gMasterId;
	sync.hdr.slave_id = gEnv.id;
	sync.hdr.payload = PAYLOAD_SYNC;
	
	sync.reserve[0] = get_extern_door_lock();
	sync.reserve[1] = get_internal_door_lock();
	sync.reserve[2] = gExternState == UNLOCK ? 1 : 0;
	
	
	set_freq();
	cc1101_tx(&sync,sizeof(sync_t),1);
}



/*
set_state_ack
*/

void set_state_ack(void)
{
	set_t set;
	
	set.hdr.master_id = gMasterId;
	set.hdr.slave_id = gEnv.id;
	set.hdr.payload = SET_STATE;
	
	set_freq();
	cc1101_tx(&set,sizeof(set_t),1);
}



/*
open_ack
*/

void open_ack(void)
{
	door_open_oper_t oper;
	
	oper.hdr.master_id = gMasterId;
	oper.hdr.slave_id = gEnv.id;
	oper.hdr.payload = PAYLOAD_DOOR_OPEN_OPER;
	
	set_freq();
	cc1101_tx(&oper,sizeof(door_open_oper_t),1);
	
}


/*
setup_ack
*/

void setup_ack(u32 id)
{
	setup_t setup;
	
	setup.hdr.master_id = gMasterId;
	setup.hdr.slave_id = id;
	setup.hdr.payload = PAYLOAD_SETUP_ID;
	
	
	set_freq();
	cc1101_tx(&setup,sizeof(setup_t),1);
}




void check_id_send(void)
{
	sync_version_t sync_version;
	
	sync_version.hdr.master_id = gMasterId;
	sync_version.hdr.slave_id = gEnv.id;
	sync_version.hdr.payload = PAYLOAD_SYNC_VERSION;
	
	//sync.reserve[0] = get_extern_door_lock();
	//sync.reserve[1] = get_internal_door_lock();
	//sync.reserve[2] = gExternState == UNLOCK ? 1 : 0;
	sync_version.version_id[0] = 'v';
	sync_version.version_id[1] = 'e';
	sync_version.version_id[2] = 'r';
	
	sync_version.ver_major	= UNLOCK_VERSION_MAJOR;
	sync_version.ver_sub	= UNLOCK_VERSION_SUB;
	sync_version.ver_sub_sub	= UNLOCK_VERSION_SUB_SUB;
	
	
	set_freq();
	cc1101_tx(&sync_version,sizeof(sync_version),1);
}


void checkRootvantype(void)
{
	if(gEnv.unlockRootvanWaveUsed==true)
	{
		for(int i=0;i<5;i++)
		{
			set_communication_led(true);
			delay_ms(50);
			set_communication_led(false);
			delay_ms(50);			
		}
	}
	
}



void wait_input_freq_adjust(void)
{
	int time_cnt=0;
	while(time_cnt < 300)
	{
		wdt_reset();
		delay_ms(10);
		time_cnt++;

		if(get_door_unlock_test()==true)
		{
			
			gTele_env.ctl_data.adj_freq++;
			
			#if 0
			if(gTele_env.ctl_data.freq_add_sub==0x80)
			{
				gTele_env.ctl_data.adj_freq--;
				
				if(gTele_env.ctl_data.adj_freq==0x00)
				{
					gTele_env.ctl_data.freq_add_sub = 0x00;
				}
			}
			else
			{
				gTele_env.ctl_data.adj_freq++;

			}
			#endif
			
			
			tele_env_save(&gTele_env);
			
			set_communication_led(true);
			delay_ms(100);
			set_communication_led(false);
		}
		else if(get_door_lock_test()==true)
		{
			
			gTele_env.ctl_data.adj_freq--;
			
			#if 0
			if(gTele_env.ctl_data.freq_add_sub==0x80)
			{
				gTele_env.ctl_data.adj_freq++;
			}
			else
			{
				gTele_env.ctl_data.adj_freq--;
				
				if(gTele_env.ctl_data.adj_freq==0x00)
				{
					gTele_env.ctl_data.freq_add_sub = 0x80;
				}
			}
			#endif
			
			tele_env_save(&gTele_env);
			
			set_out_door_state_led(true);
			delay_ms(100);
			set_out_door_state_led(false);
		}

	}
}






void test_wave_mode(u8 test_mode)
{
	int i;
	int old,new;


	cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);
	cc1101_init_reg();
	cc1101_8PATABLE_write_reg();
	cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);


	while(1)
	{
		for(i = 0 ; i < 1 ; i++)
		{
			//set_emg_led(1);
			_delay_ms(50);
			//set_emg_led(0);
			_delay_ms(50);
		}
		

		if(test_mode==0x0d)
		{
			cc1100_cmd_idle();
			set_freq();
			delay_ms(100);


			cc1101_rtx_reg_set(0); // tx
			cc1100_write_reg(CC1100_REG_PKTCTRL0,0x22);	//random TX mode
			cc1101_carrier_wave_setup();
			cc1100_cmd_idle();
			cc1100_cmd_calibrate();
			cc1100_cmd_tx();
		}
		else if(test_mode==0x0e)
		{
			cc1100_cmd_idle();
			set_freq();
			delay_ms(100);
			cc1101_tx_carrier();
		}



		
		//delay_ms(3000);
		wait_input_freq_adjust();

		cc1100_cmd_idle();
		//delay_ms(3000);
		wait_input_freq_adjust();

	}
}




void comm_led(void)
{
	int i;
	
	
	for(i = 0 ; i < 1 ; i++)
	{
		set_communication_led(1);
		_delay_ms(50);
		set_communication_led(0);
		_delay_ms(50);
	}
}

volatile int g_length;
volatile u8 gtemp;
volatile packet_t gPacket;






void extern_process_doorLock(void)
{
	static u32 timerOut;
	u8 ret;
	u8 sendOk = false;	
	
	switch(gExternState)	
	{
		case LOCK:
			// door check
			
			if(get_extern_door_lock() == false)
			{
				if(lock_err_e_door_count == -1)
				{
					lock_err_e_door_count = 0;
				}
				
				if(lock_err_e_door_count > 0)
				{
					gExternState = LOCK_ERROR;
					for(int i=0;i<7;i++)
					{
						if(gEnv.unlockRootvanWaveUsed == true)
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
					
						wdt_reset();
						if(emg_send(0,true)==true)
						{
							sendOk = true;
							break;
						}
					}
					
					if(devicePowerStart==true && sendOk!=true)
					{
						if(get_internal_door_lock() == true)
						{
							devicePowerStart=false;
						}
						checkTwiceDoorWaitCount=0;
					}
					
					
				}

			}
			else
			{
				if(devicePowerStart==true)
				{
					gExternState = LOCK_SET;
					if(get_internal_door_lock() == true)
					{					
						devicePowerStart = false;
					}
				}
				
				
				if(lock_err_e_door_count != -1)
				{
					lock_err_e_door_count = -1;
				}
			}
			break;		
		case LOCK_ERROR:
			if(get_extern_door_lock() == true)
			{
				gExternState = LOCK_SET;
				emg_send(0,false);
			}
			
			if(checkTwiceDoorWaitCount > 45)
			{
				gExternState = LOCK;
				checkTwiceDoorWaitCount=-1;
			}
			
			
			break;
			
		case UNLOCK_SET:
			#ifndef UNLOCK_DEBUG
			set_cabin_leftdoor(OPEN);
			lockActiveTime=0;
			#else
			set_communication_led(true);
			lockActiveTime=0;
			#endif
			timerOut = gGlobalTimer + (u32)gEnv.door_unlock_time;
			gExternState = UNLOCK_END_WAIT;

			break;
		case UNLOCK_END_WAIT:
		
			if(timerOut < gGlobalTimer)
			{
				#ifndef UNLOCK_DEBUG
				set_cabin_leftdoor(STOP);
				set_cabin_leftdoor(STOP2);
				lockActiveTime=-1;
				#else
				set_communication_led(false);
				lockActiveTime=-1;
				#endif				
				set_unlock_led(true);
				set_lock_led(false);
		
				//for(int i=0;i<7;i++)
				wdt_reset();
				//if(door_lock_status(false)==true)
				door_lock_status(false);
				
				gExternState = UNLOCK;
				timerOut = gGlobalTimer + (u32)gEnv.door_wait_time;
			}
			break;
		case UNLOCK:
			if(timerOut < gGlobalTimer)
			{
				gExternState = LOCK_SET;
				
			}
			else
			{
				if(get_extern_door_lock() == false)
				{
					//for(int i=0;i<4;i++)
					//if(door_status(0,true)==true)
					door_status(0,true);

					gExternState = UNLOCK_EXTERN_DOOR_OPEN;
				}
			}
			break;
		case UNLOCK_EXTERN_DOOR_OPEN:
			if(get_extern_door_lock() == true)
			{

				if(close_chk_e_door_count==-1)
				{
					close_chk_e_door_count=0;
					//close_chk_e_door_count++;
				}
				
				if(close_chk_e_door_count>1)
				{
					//door_status(0,false);
					#ifndef UNLOCK_DEBUG
					set_cabin_leftdoor(CLOSE);
					lockActiveTime=0;
					#else
					set_communication_led(true);
					lockActiveTime=0;
					#endif
					
					if(gEnv.door_lock_time == 0)
						timerOut = gGlobal500MsTimer + 1;
					else
						timerOut = gGlobal500MsTimer + (u32)gEnv.door_lock_time * (u32)2;
						
					gExternState = LOCK_END_WAIT;
				}
			}
			else
			{
				close_chk_e_door_count=-1;
			}
			break;
		case LOCK_SET:
			#ifndef UNLOCK_DEBUG
			set_cabin_leftdoor(CLOSE);
			lockActiveTime=0;
			#else
			set_communication_led(true);
			lockActiveTime=0;
			#endif
			
			if(gEnv.door_lock_time == 0)
				timerOut = gGlobal500MsTimer + 1;
			else
				timerOut = gGlobal500MsTimer + (u32)gEnv.door_lock_time * (u32)2;
				
			gExternState = LOCK_END_WAIT;
			break;
			
		case LOCK_END_WAIT:
			if(timerOut < gGlobal500MsTimer)
			{
				#ifndef UNLOCK_DEBUG
				set_cabin_leftdoor(STOP);
				set_cabin_leftdoor(STOP2);
				lockActiveTime=-1;
				#else
				set_communication_led(false);
				lockActiveTime=-1;
				#endif				
				
				door_status(0,false);
				set_unlock_led(false);
				set_lock_led(true);
				ret = door_lock_status(true);
				

				gExternState = LOCK;
				gGlobal500MsTimer=0;
			}
			break;
	}
}



void extern_process_doorCheck(void)
{
	static u32 timerOut;
	u8 sendOk=false;
	
	
	switch(gExternState)
	{
		case IDLE_STATUS:
			if(get_extern_door_lock() == false)
			{
				if(lock_err_e_door_count == -1)
				{
					lock_err_e_door_count=0;
				}
				
				
				if(lock_err_e_door_count > 0)
				{
	
					if(gInternalState == LOCK || gInternalState == LOCK_END_WAIT)
					{
						gExternState = DOOR_DELAY_STATUS;
						for(int i=0;i<7;i++)
						{
							if(gEnv.unlockRootvanWaveUsed == true)
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
							
							if(emg_send(0,true)==true)
							{
								sendOk = true;
								break;
							}
							
							wdt_reset();
							
						}

						if(devicePowerStart==true && sendOk!=true)
						{
							devicePowerStart=false;
							checkTwiceDoorWaitCount=0;
						}
											
						timerOut = gGlobalTimer + (u32)gEnv.door_delay_time;
						buzzer_toggle = true;
						
					}
					else
					{
						gExternState = DOOR_OPEN_STATUS;
						//set_room_light(true);
					}
				}
	
			}
			else
			{
				//set_in_door_state_led(false);
				set_unlock_led(false);
				
				if(lock_err_e_door_count != -1)
				{
					lock_err_e_door_count = -1;
				}
			}
			break;
		case DOOR_DELAY_STATUS:
			if(timerOut < gGlobalTimer)
			{
				buzzer_toggle = false;
	
				gExternState= ERROR_STATUS;
			}
			else
			{
				if(gInternalState == UNLOCK)
				{
	
					gExternState = DOOR_OPEN_STATUS;
					
					if(gEnv.door_error_reset == true)
					{
						emg_send(0,false);
					}
				}
				
				#if 0
				if(checkTwiceDoorWaitCount > 45)
				{
					gExternState = IDLE_STATUS;
					checkTwiceDoorWaitCount=-1;
				}
				#endif			
				
				
			}
			
			break;
		case DOOR_OPEN_STATUS:
			//set_in_door_state_led(true);
			//set_room_light(true);
	
			door_status(0,true);
	
			gExternState= DOOR_CLOSE_WAIT_STATUS;
			break;
			
		case DOOR_CLOSE_WAIT_STATUS:
			if(get_extern_door_lock() == true)
			{
				//set_in_door_state_led(false);
				//set_room_light(false);
	
				door_status(0,false);
	
				gExternState= IDLE_STATUS;
			}

			if(checkTwiceDoorWaitCount > 45)
			{
				gExternState = IDLE_STATUS;
				checkTwiceDoorWaitCount=-1;
			}			
			
			
			break;
		case ERROR_STATUS:
			if(get_extern_door_lock() == true)
			{
				door_status(0,false);
	
	
				gExternState= IDLE_STATUS;
			}
			
			#if 1
			if(checkTwiceDoorWaitCount > 45)
			{
				gExternState = IDLE_STATUS;
				checkTwiceDoorWaitCount=-1;
			}
			#endif	
			
			break;

	}
}






void internal_process_doorLock(void)
{
	static u32 timerOut;
	u8 ret;
	u8 sendOk = false;
	
	switch(gInternalState)
	{
		case LOCK:
			// door check

			
			if(get_internal_door_lock() == false)
			{
				if(lock_err_i_door_count == -1)
				{
					lock_err_i_door_count = 0;
				}
				
				if(lock_err_i_door_count > 0)
				{
					gInternalState = LOCK_ERROR;
					for(int i=0;i<7;i++)
					{
						if(gEnv.unlockRootvanWaveUsed == true)
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
						
						wdt_reset();
						if(emg_send(1,true)==true)
						{
							sendOk = true;
							break;
						}
					}
					
					if(devicePowerStart==true && sendOk!=true)
					{
						if(get_extern_door_lock() == true)
						{						
							devicePowerStart=false;
						}
						checkTwiceDoorWaitCount=0;
					}
				}
	
			}
			else
			{
				if(devicePowerStart==true)
				{
					gInternalState = LOCK_SET;
					if(get_extern_door_lock() == true)
					{					
						devicePowerStart = false;
					}
				}
				
				if(lock_err_i_door_count != -1)
				{
					lock_err_i_door_count = -1;
				}
			}
			break;
		case LOCK_ERROR:
			if(get_internal_door_lock() == true)
			{
				gInternalState = LOCK_SET;
				emg_send(1,false);
			}
			
			if(checkTwiceDoorWaitCount > 45)
			{
				gInternalState = LOCK;
				checkTwiceDoorWaitCount=-1;
			}			
			break;
			
		case UNLOCK_SET:
			#ifndef UNLOCK_DEBUG
			set_cabin_leftdoor(OPEN);
			lockActiveTime=0;
			#else
			set_communication_led(true);
			lockActiveTime=0;
			#endif
			timerOut = gGlobalTimer + (u32)gEnv.door_unlock_time;
			gInternalState = UNLOCK_END_WAIT;
	
			break;
		case UNLOCK_END_WAIT:
			
			if(timerOut < gGlobalTimer)
			{
				#ifndef UNLOCK_DEBUG
				set_cabin_leftdoor(STOP);
				set_cabin_leftdoor(STOP2);
				lockActiveTime=-1;
				#else
				set_communication_led(false);
				lockActiveTime=-1;
				#endif
				set_unlock_led(true);
				set_lock_led(false);
				
				//for(int i=0;i<7;i++)
				wdt_reset();
				//if(door_lock_status(false)==true)
				door_lock_status(false);
				
				gInternalState = UNLOCK;
				timerOut = gGlobalTimer + (u32)gEnv.door_wait_time;
			}
			break;
		case UNLOCK:
			if(timerOut < gGlobalTimer)
			{
				gInternalState = LOCK_SET;
				
			}
			else
			{
				if(get_internal_door_lock() == false)
				{
					//for(int i=0;i<4;i++)
					//if(door_status(0,true)==true)
					door_status(1,true);
	
					gInternalState = UNLOCK_EXTERN_DOOR_OPEN;
				}
			}
			break;
		case UNLOCK_EXTERN_DOOR_OPEN:
			if(get_internal_door_lock() == true)
			{
	
				if(close_chk_e_door_count==-1)
				{
					close_chk_e_door_count=0;
					//close_chk_e_door_count++;
				}
				
				if(close_chk_e_door_count>1)
				{
					//door_status(0,false);
					#ifndef UNLOCK_DEBUG
					set_cabin_leftdoor(CLOSE);
					lockActiveTime=0;
					#else
					set_communication_led(true);
					lockActiveTime=0;
					#endif
					
					if(gEnv.door_lock_time == 0)
					timerOut = gGlobal500MsTimer + 1;
					else
					timerOut = gGlobal500MsTimer + (u32)gEnv.door_lock_time * (u32)2;
					
					gInternalState = LOCK_END_WAIT;
				}
			}
			else
			{
				close_chk_i_door_count=-1;
			}
			break;
		case LOCK_SET:
			#ifndef UNLOCK_DEBUG
			set_cabin_leftdoor(CLOSE);
			lockActiveTime=0;
			#else
			set_communication_led(true);
			lockActiveTime=0;
			#endif
			
			if(gEnv.door_lock_time == 0)
			timerOut = gGlobal500MsTimer + 1;
			else
			timerOut = gGlobal500MsTimer + (u32)gEnv.door_lock_time * (u32)2;
			
			gInternalState = LOCK_END_WAIT;
			break;
			
		case LOCK_END_WAIT:
			if(timerOut < gGlobal500MsTimer)
			{
				#ifndef UNLOCK_DEBUG
				set_cabin_leftdoor(STOP);
				set_cabin_leftdoor(STOP2);
				lockActiveTime=-1;
				#else
				set_communication_led(false);
				lockActiveTime=-1;
				#endif
				door_status(1,false);
				set_unlock_led(false);
				set_lock_led(true);
				ret = door_lock_status(true);
				
	
				gInternalState = LOCK;
				gGlobal500MsTimer=0;
			}
			break;
	}
}


void internal_process_doorCheck(void)
{
	static u32 timerOut;
	u8 sendOk = false;
	
	switch(gInternalState)
	{
		case IDLE_STATUS:
			if(get_internal_door_lock() == false)
			{
				if(lock_err_i_door_count == -1)
				{
					lock_err_i_door_count=0;
				}
				
				
				if(lock_err_i_door_count > 0)
				{

					if(gExternState == LOCK ||gExternState == LOCK_END_WAIT)
					{
						gInternalState = DOOR_DELAY_STATUS;
						for(int i=0;i<7;i++)
						{
							if(gEnv.unlockRootvanWaveUsed == true)
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
							
							if(emg_send(1,true)==true)
							{
								sendOk=true;
								break;
							}
							wdt_reset();
						}

						if(devicePowerStart==true && sendOk!=true)
						{
							//devicePowerStart=false;
							checkTwiceDoorWaitCount=0;
						}
						else
						{
							devicePowerStart=false;
						}
						
						timerOut = gGlobalTimer + (u32)gEnv.door_delay_time;
						buzzer_toggle = true;
						
					}
					else
					{
						gInternalState = DOOR_OPEN_STATUS;
						set_room_light(true);
					}				
				}

			}
			else
			{
				set_in_door_state_led(false);
				
				if(lock_err_i_door_count != -1)
				{
					lock_err_i_door_count = -1;
				}
			}
			break;
		case DOOR_DELAY_STATUS:
			if(timerOut < gGlobalTimer)
			{
				buzzer_toggle = false;

				gInternalState= ERROR_STATUS;
			}
			else
			{
				if(gExternState == UNLOCK)
				{

					gInternalState = DOOR_OPEN_STATUS;
					
					if(gEnv.door_error_reset == true)
					{
						emg_send(1,false);
					}
				}
			}

			wdt_reset();
			
			break;
		case DOOR_OPEN_STATUS:
			set_in_door_state_led(true);
			set_room_light(true);

			door_status(1,true);

			gInternalState= DOOR_CLOSE_WAIT_STATUS;
			break;
			
		case DOOR_CLOSE_WAIT_STATUS:
			if(get_internal_door_lock() == true)
			{
				set_in_door_state_led(false);
				set_room_light(false);

				door_status(1,false);

				gInternalState= IDLE_STATUS;
			}

			if(checkTwiceDoorWaitCount > 45)
			{
				gInternalState = IDLE_STATUS;
				checkTwiceDoorWaitCount=-1;
			}
			
			
			
			break;
		case ERROR_STATUS:
			if(get_internal_door_lock() == true)
			{
				door_status(1,false);


				gInternalState= IDLE_STATUS;
			}

			if(checkTwiceDoorWaitCount > 45)
			{
				gInternalState = IDLE_STATUS;
				checkTwiceDoorWaitCount=-1;
			}
			
			wdt_reset();
			
			break;

	}
}





void button_check(void)
{
	// switch check
	if(btn_door_lock_test != get_door_lock_test())
	{
		if(get_door_lock_test() == true)
		{
			btn_door_lock_test_count=0;
			buzzer_on(true);
		}
		else if(get_door_lock_test() == false)
		{
			buzzer_on(false);
		}
		
		btn_door_lock_test = get_door_lock_test();
	}
}



void button_process(void)
{
	if(btn_door_lock_test_count > 0)
	{
		if(get_door_lock_test() == true)
		{
			if(btn_door_lock_test_count > 2)
			{

				check_id_send();
				btn_door_lock_test_count = -1;
				buzzer_on(false);
				_delay_ms(1000);
				
			}
		}
		else
		{
			btn_door_lock_test_count = -1;
		}
	}
}





volatile u8 door_state,old_door_state;

int main(void)
{
	u8 rssi,lqi;
	int length;
	u32 i;
	int rssi_fixed;
	
	
	setClockTo32MHz();	
	
	port_init();
	spi_init();
	env_init();
	timer_setup();
	TCC0_INTCTRLA = 0x03;				// enable OVF interrupt with high level
	
	


	cc1100_init(delay_ms,spi_write_single,spi_read_single,spi_write,spi_read,spi_select,spi_deselect,spi_read_somi);
	cc1101_init_reg();
	
	if(gEnv.unlockRootvanWaveUsed == true)
	{

		pt_data[0]=0x60;		//Output wave power 0dBm   add 20160408
	}
	
	cc1101_8PATABLE_write_reg();

	wdt_enable(0x26);		//wdt timeout 4sec enable
	wdt_reset();
	
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	sei();	
	set_power_led(true);
	
	gEnv.door_wait_time = 20;
	//gEnv.door_unlock_time = 3;
	gEnv.door_unlock_time = 1;
	gEnv.door_open_time = 2;
	gEnv.door_lock_time=0;
	gEnv.door_close_time = 2; 
	//gEnv.door_delay_time = 10;
	//gEnv.door_delay_time = 2;
	gEnv.door_delay_time = 0;
	gEnv.door_error_reset = false;
	
	
	check_id_send();
	checkRootvantype();
	
	
	if(get_extern_door_lock() == false || get_internal_door_lock() == false)
	{
		door_state = old_door_state = true; // open
	}
	else
	{
		door_state = old_door_state = false;
	}
	
	if(!((~get_dly() & 0x0f) == 0x00 && (~get_test() & 0x0f) == 0x00))
	{
		mentenece_mode = 0;
	}
	
	
	test_mode = ~get_dly() & 0x0f;


	#if 0
	set_cabin_leftdoor(CLOSE);
			
	double time;
	
	for(i = 0 ; i < gEnv.door_lock_time ; i++)
	{
		_delay_ms(100);
		//wdt_reset();
	}
	#endif


	set_cabin_leftdoor(STOP);
	set_cabin_leftdoor(STOP2);
	lockActiveTime=-1;
	


	set_unlock_led(false);
	set_lock_led(true);
	door_lock_status(true);
	
    while(1)
    {
		wdt_reset();
		
		if(gEnv.unlockRootvanWaveUsed == true)
		{
			if(pt_data[0] != 0x60)
			{
				pt_data[0] = 0x60;
				cc1101_8PATABLE_write_reg();
			}
		}
		else
		{
			if(pt_data[0] != 0xc0)
			{
				pt_data[0] = 0xc0;
				cc1101_8PATABLE_write_reg();
			}
		}


	#if 1
		
		set_freq();



		#if 0
		if(sleep_cansel_count > -1 || gExternState != LOCK || gInternalState != IDLE_STATUS || lock_err_e_door_count != -1 || lock_err_i_door_count != -1)
		{
			g_length = cc1101_rx((u8 *)&gPacket,&rssi,&lqi,RX_NOMAL);		
		}
		else
		{
			g_length = cc1101_rx((u8 *)&gPacket,&rssi,&lqi,RX_WOR);

			sleep_cansel_count = 0;
		}
		#else
		g_length = cc1101_rx((u8 *)&gPacket,&rssi,&lqi,RX_NOMAL);
		#endif
		


		if(g_length == sizeof(packet_t))
		{
			gMasterId = gPacket.hdr.master_id;
			//gtemp = ~get_dly() & 0x0c;
			gtemp = ~get_dly() & 0x0f;
			if((gtemp) == 0x06)
			{
				if(gPacket.hdr.slave_id == 0xffffffff && gPacket.hdr.payload == PAYLOAD_SETUP_ID && deny_setup_id < 600)
				{
					
					setup_t * setup;
					int kk;
					

					comm_led();
					setup = (set_t *)	&gPacket;

					setup_ack(setup->id);
					
					gEnv.id = setup->id;
					env_save(&gEnv);
					
					for(kk = 0 ; kk < 10 ; kk++)
					{
						set_in_door_state_led(1);
						_delay_ms(100);
						set_in_door_state_led(0);
						_delay_ms(100);
					}
				}
			}
			else if(gPacket.hdr.slave_id == gEnv.id)
			{
				switch(gPacket.hdr.payload)
				{
					case PAYLOAD_SYNC:
					case PAYLOAD_SYNC_START:
					
						if(gEnv.unlockRootvanWaveUsed==true)
						{
							if(rssi > 128)
							{
								rssi_fixed = (((char)rssi - 256)/2) - 74;
							}
							else
							{
								rssi_fixed = ((char)rssi/2) - 74;
							}						
							
							if(rssi_fixed < -25)
							{
								pt_data[0] = 0x0e;		//output power -20dBm
								cc1101_8PATABLE_write_reg();
							}							
							else if(rssi_fixed < -30)
							{
								pt_data[0] = 0x34;		//output power -10dBm
								cc1101_8PATABLE_write_reg();
							}
						}
					
						sync_ack();
						
						if(gEnv.unlockRootvanWaveUsed == true)
						{
							if(pt_data[0] != 0x60)
							{
								pt_data[0] = 0x60;
								cc1101_8PATABLE_write_reg();
							}
						}
						else
						{
							if(pt_data[0] != 0xc0)
							{
								pt_data[0] = 0xc0;
								cc1101_8PATABLE_write_reg();
							}
						}
						
						//gEnv.main_set.beep_b_s_door_used = gPacket.payload[1];
						gEnv.beep_b_s_door_used = gPacket.payload[1];
						gEnv.unlockRootvanWaveUsed = gPacket.payload[2];
						gEnv.door_Type = gPacket.payload[3];
						
						env_save(&gEnv);
						
						if(gEnv.unlockRootvanWaveUsed == true)
						{
							//pt_data[0]=0xc8;		//Output wave power 7dBm   add 20160408
							//pt_data[0]=0x84;		//Output wave power 5dBm   add 20160408
							pt_data[0]=0x60;		//Output wave power 0dBm   add 20160408
						}
						else
						{
							pt_data[0]=0xc0;		//Output wave power 10dBm   add 20160408
						}
						
						
						cc1101_8PATABLE_write_reg();
						
						if(gPacket.hdr.payload == PAYLOAD_SYNC_START)
						{
							while(1);
						}
						
						break;
						
					case SET_STATE:
					{
						set_t * set;
						
						if(gEnv.unlockRootvanWaveUsed==true)
						{
							if(rssi > 128)
							{
								rssi_fixed = (((char)rssi - 256)/2) - 74;
							}
							else
							{
								rssi_fixed = ((char)rssi/2) - 74;
							}
							
							if(rssi_fixed < -25)
							{
								pt_data[0] = 0x0e;		//output power -20dBm
								cc1101_8PATABLE_write_reg();
							}
							else if(rssi_fixed < -30)
							{
								pt_data[0] = 0x34;		//output power -10dBm
								cc1101_8PATABLE_write_reg();
							}
						}
						
						set_state_ack();

						if(gEnv.unlockRootvanWaveUsed == true)
						{
							if(pt_data[0] != 0x60)
							{
								pt_data[0] = 0x60;
								cc1101_8PATABLE_write_reg();
							}
						}
						else
						{
							if(pt_data[0] != 0xc0)
							{
								pt_data[0] = 0xc0;
								cc1101_8PATABLE_write_reg();
							}
						}

						set = (set_t *)	&gPacket;
						
						set_state = set->set ;
						
						
						break;
					}
					case PAYLOAD_DOOR_OPEN_OPER:
					{

						if(gEnv.unlockRootvanWaveUsed==true)
						{
							if(rssi > 128)
							{
								rssi_fixed = (((char)rssi - 256)/2) - 74;
							}
							else
							{
								rssi_fixed = ((char)rssi/2) - 74;
							}
							
							if(rssi_fixed < -25)
							{
								pt_data[0] = 0x0e;		//output power -20dBm
								cc1101_8PATABLE_write_reg();
							}
							else if(rssi_fixed < -30)
							{
								pt_data[0] = 0x34;		//output power -10dBm
								cc1101_8PATABLE_write_reg();
							}
						}
											
						open_ack();
						
						if(gEnv.unlockRootvanWaveUsed == true)
						{
							if(pt_data[0] != 0x60)
							{
								pt_data[0] = 0x60;
								cc1101_8PATABLE_write_reg();
							}
						}
						else
						{
							if(pt_data[0] != 0xc0)
							{
								pt_data[0] = 0xc0;
								cc1101_8PATABLE_write_reg();
							}
						}						
						
						
						//if(set_state == false)
						if(gEnv.door_Type != 2)
						{
							gExternState = UNLOCK_SET;
						}
						else
						{
							gInternalState = UNLOCK_SET;
						}
					}
				}
				comm_led();


			} 
		}
		cc1100_cmd_idle();
		

		
		{
			gEnv.door_wait_time = 20;
			//gEnv.door_unlock_time = 3;
			gEnv.door_unlock_time = 1;
			gEnv.door_open_time = 2;
			gEnv.door_lock_time=0;
			gEnv.door_close_time = 2;
			//gEnv.door_delay_time = 10;
			gEnv.door_delay_time = 2;
		}
				
		

		if(get_extern_door_lock() == false || get_internal_door_lock() == false)
		{
			door_state = true; // open
		}
		else
		{
			door_state = false;
		}
		
		
		if(get_internal_door_lock() == false && get_extern_door_lock() == false)
		{
			set_room_light(true);	//open
		}
		else
		{
			set_room_light(false);	//close
		}
		
		
		if(old_door_state != door_state || checkTwiceDoorWaitCount > -1 || devicePowerStart == true)
		{
			buzzer_toggle = door_state;
			if(door_state == false)
			{
				_delay_ms(3);
				buzzer_on(false);
			}
			
			if(get_extern_door_lock() == false)
				set_out_door_state_led(1);
			else
				set_out_door_state_led(0);
				
			if(get_internal_door_lock() == false)
				set_in_door_state_led(1);
			else
				set_in_door_state_led(0);
				
			//set_room_light(door_state);
			
			old_door_state = door_state;
		}
		
		
		
		button_check();
		button_process();
		
		
		
		#if 0
		extern_process();
		
		internal_process();
		#endif
		
		
		if(gEnv.door_Type == 2)
		{
			extern_process_doorCheck();
			internal_process_doorLock();
		}
		else
		{
			extern_process_doorLock();
			internal_process_doorCheck();
		}
		
		
		
		
		if(test_mode == 0x0d || test_mode == 0x0e)
		{
			test_wave_mode(test_mode);
		}
	
	
	#endif
		
    }
}