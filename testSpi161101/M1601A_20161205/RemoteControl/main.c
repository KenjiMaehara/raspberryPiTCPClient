/*
 * 
 *
 * Created: 
 *  Author: 
 */ 
//#define F_CPU 2000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <mytype.h>
#include "cc1100.h"

#include <util/delay.h>
#include "port.h"
#include "env.h"
#include "spi.h"


volatile int btn_emg_count=-1;
volatile int btn_open_count=-1;
volatile int btn_rear_open_count=-1;
volatile int btn_side_open_count=-1;
volatile int btn_set_count=-1;
volatile int btn_reset_count=-1;
volatile u8 btn_emg,btn_open,btn_rear_open,btn_side_open,btn_set,btn_reset;
volatile u8 emg_state = false;
volatile u8 set_state = false;
volatile u32 gMasterId;
volatile u8 gDriver = false;

volatile int batt_low_voice=-1;
volatile int btn_ctl_deny_timeout_count=-1;


volatile int btn_powerdown_count=-1;
volatile u8 btn_powerdown;
volatile u8 remocon_emg= 0;
volatile u8 main_emg = 0;

volatile int sleep_cansel_count=-1;

volatile u8 rssi_fixed = 0;

volatile int mentenece_mode = -1;
volatile int sound_mute = -1;
volatile int temp_reset_count = -1;

volatile int door_open_msg_cancel = -1;

volatile int cnt_wait_syn_ack=-1;

packet_t gPacket;


#define CARRIER_SENSE_DETECT	0x55
#define RF_TIME_OUT_10MSEC	55
#define RF_TIME_OUT_100MSEC	550
#define RF_TIME_OUT_1SEC	5554



void delay_us(u8 time_us)		/* time delay for us(1~255 us) for 32MHz */
{
	register unsigned char i;

	for(i = 0; i < time_us; i++)			// 4 cycles
	{ asm volatile("PUSH R0");			// 1 cycles
		
		#if 1
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
		#endif
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
		//ADCA.REFCTRL = 0; // Internal 1v ref
		ADCA.REFCTRL = 1; // Internal 1/6 v ref
		//ADCA.REFCTRL = 0x20; // PORTA AREF ref
		ADCA.EVCTRL = 0 ; // no events
		//ADCA.EVCTRL = (5<<3); // ADC5,6,7selected events
		ADCA.PRESCALER = ADC_PRESCALER_DIV128_gc ;
		//ADCA.PRESCALER = ADC_PRESCALER_DIV4_gc;
		//ADCA.PRESCALER = ADC_PRESCALER_DIV64_gc;
		
		ADCA.CALL = ReadSignatureByte(0x20) ; //ADC Calibration Byte 0
		ADCA.CALH = ReadSignatureByte(0x21) ; //ADC Calibration Byte 1
		//ADCA.CALL = 0x00;		//test
		//ADCA.CALH = 0x0F;		//test
		_delay_ms(10);//OSTimeDlyHMSM(0,0,0,10); // Wait at least 25 clocks
	}
	ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc | 0x01 ; // Gain = 1, Single Ended
	//ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc | 0x10 ; // Gain = 1, Single Ended
	//ADCA.CH0.MUXCTRL = (0x07<<3);
	ADCA.CH0.MUXCTRL = (0x00<<3) ;
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
	

#ifndef KOREA
	//fr = 0x00;//get_freq();
	fr = ~get_freq()  & 0x0f;

	frq = 0x66c0 + (fr * 0x003f);	
	//frq = 0x66c0 - 0x1f4 + (fr * 0x003f);
	//frq = 0x66c0 - 0x4e20 + 0x500 + 0x1a0 + (fr * 0x003f);



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



	


	offset = 0x0339 - 0;
	
	level = 13 - (offset / 0x0c);
	

	if(offset & 0x8000)
	level = 13;
	else if((offset / 0x0c)>13)
	level = 0;
	
#else


	fr = ~get_freq()  & 0x0f;

	frq = 0x66c0 + (fr * 0x003f);

	offset = 0x0339 - 0;

	level = 13 - (offset / 0x0c);


	if(offset & 0x8000)
	level = 13;
	else if((offset / 0x0c)>13)
	level = 0;

#endif 	
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
	
	PORTA.PIN2CTRL = 0x18; // pullup , both edge
	
	delay_ms(10);

	if(PORTA.IN & 0x04)
	{
		cc1100_cmd_idle();
		
		if(th == 0)
		{
			return CARRIER_SENSE_DETECT;
		}

	}

	PORTA.PIN2CTRL = 0x00;





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
		//_delay_ms(500);	
	}

	
	return 1;
}

#define	RX_WOR		1
#define	RX_NOMAL	2



ISR(PORTA_INT1_vect)
{


}

ISR(PORTC_INT1_vect)
{


}



#define sleep() __asm__ __volatile__ ("sleep")



u8 cc1101_rx(u8 * data,u8 * rssi, u8 * lqi, u8 rx_type)
{
	//int cnt_wait_syn_ack=0;
	u8 length;
	u8 RX_STATUS = RX_FIFO_NONE;
	

	cc1100_cmd_idle();
	set_freq();


	if(rx_type == RX_NOMAL)
	{
		cc1100_write_reg(CC1100_REG_MCSM0, 0x04);
		cc1100_write_reg(CC1100_REG_MCSM2, 0x07);
	}

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
			if(cnt_wait_syn_ack > RF_TIME_OUT_10MSEC)
			break;
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
			
			//_delay_ms(1);
			if(cnt_wait_syn_ack > RF_TIME_OUT_100MSEC)
			{
				cnt_wait_syn_ack = -1;
				break;
			}
		}
		return 0;
	}
	else
	{
		PORTC.INT1MASK=0x04;
		PORTC.INTCTRL=0x0C;
		PORTA.INT1MASK=0xFC;
		PORTA.INTCTRL=0x0C;
		
		cc1100_cfg_gdo0(CC1100_GDOx_SYNC_WORD);
		cc1100_write_reg(CC1100_REG_WOREVT1, 0x20);
		cc1100_write_reg(CC1100_REG_WOREVT0, 0xff);
		cc1100_write_reg(CC1100_REG_WORCTRL, 0x08);
		cc1100_write_reg(CC1100_REG_MCSM0, 0x18);
		cc1100_write_reg(CC1100_REG_MCSM2, 0x00);
		
		cc1100_strobe_cmd(0x38);		//WOR COMMAND
		
		set_speaker_power(false);
		
		wdt_disable();
		
		//SLEEP_CTRL |= SLEEP_SMODE_PDOWN_gc;
		SLEEP_CTRL |= SLEEP_SMODE_PSAVE_gc;
		SLEEP_CTRL |= SLEEP_SEN_bm;
		
		sleep();
		
		PORTC.INT1MASK=0x00;
		PORTA.INT1MASK=0x00;
		

		wdt_enable(0x26);		//wdt timeout 4sec enable

		set_speaker_power(true);
		//set_speaker_power(false);
		
		_delay_ms(50);
		
		
		if(get_security_reset() || get_security_set() || get_side_door_unlocked() || get_rear_door_unlocked() || get_emergency_button() || get_cab_unlocked())
		{
			buzzer_on(true);
			return 0x77;
		}
		
		
		if((cc1100_status_crc_lqi() & 0x80))
		{
			length = rx_fifo_read(data,rssi,lqi);
			return length;
		}
		
		return 0;
	}
	
	return 0;
}






#if 0

u8 cc1101_wor_rx(u8 * data,u8 * rssi, u8 * lqi)
{
	int cnt_wait_syn_ack=0;
	u8 length;
	u8 RX_STATUS = RX_FIFO_NONE;

	cc1100_cmd_idle();
	set_freq();	

	cc1101_rtx_reg_set(1);
	cc1101_carrier_wave_setup();

	cc1100_cmd_idle();
	cc1100_cmd_flush_rx();
	cc1100_cmd_calibrate();
	cc1100_cmd_rx();


	PORTC.INT1MASK=0x04;
	PORTC.INTCTRL=0x0C;
	PORTA.INT1MASK=0xFC;
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
	
	PORTC.INT1MASK=0x00;
	PORTA.INT1MASK=0x00;

	wdt_enable(0x26);		//wdt timeout 4sec enable
	


	if(get_security_reset() || get_security_set() || get_side_door_unlocked() || get_rear_door_unlocked() || get_emergency_button())
	{
		return 0x77;
	}


	if((cc1100_status_crc_lqi() & 0x80))
	{
		length = rx_fifo_read(data,rssi,lqi);
		return length;
	}

	return 0;	
}

#endif





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
	TCC0.PER = 90;
	//TCC0.PER = 10;
	TCC0.CTRLA = ( TCC0.CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_DIV64_gc;
	TCC0.INTCTRLA = ( TCC0.INTCTRLA & ~TC0_OVFINTLVL_gm ) | TC_OVFINTLVL_MED_gc;
}


volatile  u8 buz_toggle = false;
volatile u8 toggle = false;
volatile int timer_1S_count = 0;
volatile int send_timer=-1;
volatile u8 gBuzzer;
volatile int deny_setup_id = 0;
volatile int temp_reset_led_toggle = 0;
volatile int timer_500ms_count = 0;


ISR(TCC0_OVF_vect)
{
	#if 1
	if(gBuzzer == true)
	{
		buz_toggle ^= 0x01;
		set_buzzer(buz_toggle);
	}
	else
	{
		set_buzzer(0);
	}
	#endif
	
	if(send_timer > -1)
	{
		send_timer++;
		
	}
	
	if(cnt_wait_syn_ack > -1)
	{
		cnt_wait_syn_ack++;
	}
	
	
	if(timer_500ms_count++ > 2776)	
	{
		
		#if 0
		buz_toggle ^= 0x01;
		set_bat_led(buz_toggle);
		#endif		
		
		
		if(temp_reset_led_toggle != 0)
		{
			temp_reset_led_toggle=0;
		}
		else
		{
			temp_reset_led_toggle++;
		}
		
		if(temp_reset_count > -1)
		{
			if(btn_powerdown_count < 1)
			{
				set_security_mode_led(temp_reset_led_toggle);				
			}
			else
			{
				set_bat_led(false);
				set_communication_led(false);
				set_car_emergency_led(false);
				set_emg_led(false);
				set_emg_button_led(false);
				set_security_mode_led(false);
			}

		}
		
		timer_500ms_count=0;
	}
	

	
	if(timer_1S_count++ > 5554)
	{

		#if 0
		buz_toggle ^= 0x01;
		set_bat_led(buz_toggle);
		#endif

		
		
		timer_1S_count  = 0;
		
		if(btn_emg_count > -1)
			btn_emg_count++;
			
		if(btn_open_count > -1)
			btn_open_count++;
		if(btn_rear_open_count > -1)
			btn_rear_open_count++;
		if(btn_side_open_count > -1)
			btn_side_open_count++;
		if(btn_set_count > -1)
			btn_set_count++;
		if(btn_reset_count > -1)
			btn_reset_count++;
		if(btn_powerdown_count > -1)
			btn_powerdown_count++;
		
		
		if(sleep_cansel_count > -1)
			sleep_cansel_count++;
			
		if(sleep_cansel_count > 10)
			sleep_cansel_count = -1;

		
		if(batt_low_voice > -1)
			batt_low_voice++;
			
		if(batt_low_voice > 3)
			batt_low_voice = -1;
			
			
		if(btn_ctl_deny_timeout_count > -1)
			btn_ctl_deny_timeout_count++;
			
		if(btn_ctl_deny_timeout_count > 2)
			btn_ctl_deny_timeout_count = -1;
			
		if(deny_setup_id < 600)
			deny_setup_id++;
			
		if(temp_reset_count > -1)
			temp_reset_count++;
			
			
		if(mentenece_mode > -1)
		{
			if(mentenece_mode == 0)
			{
				set_bat_led(true);
				mentenece_mode++;
				
				if((~get_test() & 0x0f)==0x06)
				{
					set_security_mode_led(true);
				}
			}
			else
			{
				set_bat_led(false);
				mentenece_mode=0;

				if((~get_test() & 0x0f)==0x06)
				{
					set_security_mode_led(false);
				}
			}
		}
			

	}
	
	#if 0
	if(get_security_reset() || get_security_set() || get_side_door_unlocked() || get_rear_door_unlocked() || get_emergency_button())
	{
		buzzer_on(true);
	}
	#endif

	
}



void buzzer_error(void)
{
	buzzer_on(true);
	_delay_ms(300);
	buzzer_on(false);
	_delay_ms(300);
	buzzer_on(true);
	_delay_ms(300);
	buzzer_on(false);
	_delay_ms(300);
	buzzer_on(true);
	_delay_ms(300);
	buzzer_on(false);
	_delay_ms(300);
}

volatile void buzzer_on(u8 buz)
{
	gBuzzer = buz;
}

u8 emg_send(u8 emg)
{
	int length;
	remote_emg_t temg;
	u8 rssi,lqi;
	
	
	temg.hdr.slave_id = gEnv.id;
	temg.hdr.payload = PAYLOAD_REMOCON_EMG;
	temg.emg = emg;

	set_freq();
	cc1101_tx(&temg,sizeof(emg_t),1);
	
	send_timer = 0;
	
	while(1)
	{
		
		set_freq();
		length = cc1101_rx((u8 *)&temg,&rssi,&lqi,RX_NOMAL);
		if(length > 0 )
		{
			if(temg.hdr.slave_id == gEnv.id && temg.hdr.payload == PAYLOAD_REMOCON_EMG)
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


void emg_set_process(void)
{
	// buzzer off;
	#if 0
	if(main_emg == true)
		return;
	#endif
	
	//emg_state = true;
	//set_emg_led(true);
	//set_voice(5);
	
	
	//for(int i=0;i<7;i++)
	{
		if(emg_send(true))
		{
			emg_state = true;
			remocon_emg = true;
			set_emg_led(true);
			set_emg_button_led(true);
			set_voice(5);
			//break;
		}	
	}
}


void emg_reset_process(void)
{
	// buzzer off;
	#if 0
	if(main_emg == true)
		return;
	#endif	
						
	
	//emg_state = false;
	//set_emg_led(false);
	//emg_send(false);
	
	
	//for(int i=0;i<7;i++)
	{
		if(emg_send(false))
		{
			emg_state = false;
			remocon_emg = false;
			set_emg_led(false);
			set_emg_button_led(false);

			//break;
		}
	}
	
}


u8 door_open_send(u8 door)
{
	int length;
	door_open_t tdoor;
	u8 rssi,lqi;
	u32 GetTime=0;
	u8	th;
	u8	carrier_sen_check=0;
	
	for(int i=0;i<8;i++)
	{	
		wdt_reset();
		
		tdoor.hdr.slave_id = gEnv.id;
		tdoor.hdr.payload = PAYLOAD_DOOR_OPEN_CMD;
		if(door == 1)
			tdoor.door = gDriver == true ? 1 : 2;
		else
			tdoor.door = door;
			
		tdoor.open = 1;
		
		if(i==4 || i==6)
		{
			th = 1;
		}
		else
		{
			th = 0;
		}
		
		set_freq();
		
		carrier_sen_check = cc1101_tx(&tdoor,sizeof(door_open_t),th);
		
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			GetTime = RF_TIME_OUT_1SEC;
		}
		else
		{
			GetTime = RF_TIME_OUT_10MSEC;
		}
		
		send_timer = 0;
		while(1)
		{
			
			set_freq();
			length = cc1101_rx((u8 *)&tdoor,&rssi,&lqi,RX_NOMAL);
			if(length > 0 )
			{
				if(tdoor.hdr.slave_id == gEnv.id && tdoor.hdr.payload == PAYLOAD_DOOR_OPEN_CMD)
				{
					send_timer = -1;
					comm_led();
					
					if(tdoor.reserve[0]==false)
					{
						return Door_open_deny;
					}

					return true;
				}
			}
			cc1100_cmd_idle();
			
			if(send_timer > GetTime)
			{
				send_timer = -1;
				break;
			}
		}
	}
		
	return false;
}

void emg_driver_door_process(void)
{
	if(door_open_send(1) == false)
	{
		set_voice(8);
		buzzer_error();
		return;		
	}
	
	if(sound_mute == -1)
	{
		set_voice(3);
	}


}


void emg_rear_door_process(void)
{
	u8 ret;
	
	ret = door_open_send(3);
	
	
	//if(door_open_send(3) == false)
	if(ret == false)
	{
		set_voice(8);
		buzzer_error();
		return;
	}
	
	if(ret == Door_open_deny)
	{
		//set_voice(8);
		//set_voice(6);
		return;
	}
	
	if(sound_mute == -1)
	{
		set_voice(3);
	}

	btn_ctl_deny_timeout_count=0;
	buzzer_on(false);
}


void emg_side_door_process(void)
{
	u8 ret;
	
	ret = door_open_send(4);
	
	//if(door_open_send(4) == false)
	if(ret == false)
	{
		set_voice(8);
		buzzer_error();
		return;
	}
	
	if(ret == Door_open_deny)
	{
		//set_voice(8);
		//set_voice(6);
		return;
	}
	
	if(sound_mute == -1)
	{
		set_voice(3);
	}

	btn_ctl_deny_timeout_count=0;
	buzzer_on(false);

}


u8 set_send_old(u8 set)
{
	u8 rssi,lqi;
	int length;
	set_cmd_t lset;
	
	lset.hdr.slave_id = gEnv.id;
	lset.hdr.payload = PAYLOAD_SET_CMD;
	lset.set = set;
	//lset.version_id[0] = 'v';
	//lset.version_id[1] = 'e';
	//lset.version_id[2] = 'r';
	
	//lset.remocon_ver_major	= REMOCON_VERSION_MAJOR;
	//lset.remocon_ver_sub	= REMOCON_VERSION_SUB;
	//lset.remocon_ver_sub_sub = REMOCON_VERSION_SUB_SUB;

	set_freq();
	cc1101_tx(&lset,sizeof(set_cmd_t),1);

	send_timer = 0;	
	
	while(1)
	{
		
		set_freq();
		length = cc1101_rx((u8 *)&lset,&rssi,&lqi,RX_NOMAL);
		if(length > 0 )
		{
			if(lset.hdr.slave_id == gEnv.id && lset.hdr.payload == PAYLOAD_SET_CMD)
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
	}
	
	
	return false;
}




u8 set_send(u8 set)
{
	int length;
	//door_open_t tdoor;
	volatile set_cmd_t lset;
	u8 rssi,lqi;
	u32 GetTime=0;
	u8	th;
	u8	carrier_sen_check=0;
	
	for(int i=0;i<5;i++)
	{
		wdt_reset();
		
		#if 0
		tdoor.hdr.slave_id = gEnv.id;
		tdoor.hdr.payload = PAYLOAD_SET_CMD;
		#endif
		
		lset.hdr.slave_id = gEnv.id;
		lset.hdr.payload = PAYLOAD_SET_CMD;
		lset.set = set;
				
		#if 0
		if(door == 1)
			tdoor.door = gDriver == true ? 1 : 2;
		else
			tdoor.door = door;
		
		tdoor.open = 1;
		#endif
		
		if(i==1 || i==3)
		{
			th = 1;
		}
		else
		{
			th = 0;
		}
		
		set_freq();
		cc1101_tx(&lset,sizeof(set_cmd_t),1);
		//carrier_sen_check = cc1101_tx(&tdoor,sizeof(door_open_t),th);
		
		if(carrier_sen_check == CARRIER_SENSE_DETECT)
		{
			GetTime = RF_TIME_OUT_1SEC;
		}
		else
		{
			GetTime = RF_TIME_OUT_10MSEC;
		}
		
		send_timer = 0;
		while(1)
		{
			
			set_freq();
			//length = cc1101_rx((u8 *)&tdoor,&rssi,&lqi,RX_NOMAL);
			length = cc1101_rx((u8 *)&lset,&rssi,&lqi,RX_NOMAL);
			if(length > 0 )
			{
				//if(tdoor.hdr.slave_id == gEnv.id && tdoor.hdr.payload == PAYLOAD_SET_CMD)
				if(lset.hdr.slave_id == gEnv.id && lset.hdr.payload == PAYLOAD_SET_CMD)
				{
					send_timer = -1;
					comm_led();
					
					#if 0
					if(tdoor.reserve[0]==false)
					{
						return Door_open_deny;
					}
					#endif

					return true;
				}
			}
			cc1100_cmd_idle();
			
			if(send_timer > GetTime)
			{
				send_timer = -1;
				break;
			}
		}
	}
	
	return false;
}





/*
door open status
PAYLOAD_GET_DOOR_STATUS

B,C -> A door open?
*/
//int get_door_status(void)
get_door_open_status_t get_door_status(void)
{
	u8 rssi,lqi;
	int length;
	get_door_open_status_t door;
	
	door.hdr.slave_id = gEnv.id;
	door.hdr.payload = PAYLOAD_GET_DOOR_STATUS;

	set_freq();
	cc1101_tx(&door,sizeof(get_door_open_status_t),1);

	send_timer = 0;
	
	while(1)
	{
		
		set_freq();
		length = cc1101_rx((u8 *)&door,&rssi,&lqi,RX_NOMAL);
		if(length > 0 )
		{
			if(door.hdr.slave_id == gEnv.id && door.hdr.payload == PAYLOAD_GET_DOOR_STATUS)
			{
				send_timer = -1;
				comm_led();
				
				//return (int)door.door;
				return (get_door_open_status_t)door;
			}
		}
		cc1100_cmd_idle();
		
		if(send_timer > 1000)
		{
			send_timer = -1;
			break;
		}
	}
	
	door.door = -1;
	return door;
	
	//return -1;
}



u8 sync2_send(void)
{
	int length;
	sync2_t sync;
	u8 rssi,lqi;
	
	
	sync.hdr.slave_id = gEnv.id;
	sync.hdr.payload = PAYLOAD_SYNC2;
	
	set_freq();
	cc1101_tx(&sync,sizeof(sync2_t),1);
	
	send_timer = 0;
	while(1)
	{
		
		set_freq();
		length = cc1101_rx((u8 *)&sync,&rssi,&lqi,RX_NOMAL);
		if(length > 0 )
		{
			if(sync.hdr.slave_id == gEnv.id && sync.hdr.payload == PAYLOAD_SYNC2)
			{
				gEnv.emg_btn_set = sync.reserve[0];
				gEnv.emg_btn_reset = sync.reserve[1];
				gEnv.set_time=sync.reserve[2];
				gEnvDown.emg_btn_set = sync.reserve[0];
				gEnvDown.emg_btn_reset = sync.reserve[1];
				gEnvDown.set_time=sync.reserve[2];
				emg_state = sync.reserve[3] ;
				set_state = sync.reserve[4] ;
				
				if(set_state == SET)
				{
					set_security_mode_led(true);
				}
				else
				{
					set_security_mode_led(false);					
				}
				
				
				//set_security_mode_led(set_state);
				set_emg_led(emg_state);

				
				send_timer = -1;
				comm_led();

				return true;
			}
		}
		cc1100_cmd_idle();
		
		if(send_timer > 2000)
		{
			send_timer = -1;
			break;
		}
	}
	
	
	return false;
}




u8 sync_version_send(void)
{
	int length;
	sync_version_t sync_version;
	u8 rssi,lqi;
	
	
	sync_version.hdr.slave_id = gEnv.id;
	sync_version.hdr.payload = PAYLOAD_SYNC_VERSION;
	sync_version.ver_major = REMOCON_VERSION_MAJOR;
	sync_version.ver_sub = REMOCON_VERSION_SUB;
	sync_version.ver_sub_sub = REMOCON_VERSION_SUB_SUB;
	
	set_freq();
	cc1101_tx(&sync_version,sizeof(sync_version_t),1);
	
	send_timer = 0;
	while(1)
	{
		
		set_freq();
		length = cc1101_rx((u8 *)&sync_version,&rssi,&lqi,RX_NOMAL);
		if(length > 0 )
		{
			if(sync_version.hdr.slave_id == gEnv.id && sync_version.hdr.payload == PAYLOAD_SYNC_VERSION)
			{
				rssi_fixed = rssi;
				
				

				#if 0
				gEnv.emg_btn_set = sync.reserve[0];
				gEnv.emg_btn_reset = sync.reserve[1];
				gEnv.set_time=sync.reserve[2];
				gEnvDown.emg_btn_set = sync.reserve[0];
				gEnvDown.emg_btn_reset = sync.reserve[1];
				gEnvDown.set_time=sync.reserve[2];
				emg_state = sync.reserve[3] ;
				set_state = sync.reserve[4] ;
				
				set_security_mode_led(set_state);
				set_emg_led(emg_state);

				
				send_timer = -1;
				comm_led();
				
				#endif

				return true;
			}
		}
		cc1100_cmd_idle();
		
		if(send_timer > 2000)
		{
			send_timer = -1;
			break;
		}
	}
	
	
	return false;
}




u8 sync_rssi_send(u8 * rssi, u8 * lqi)
{
	int length;
	volatile sync_version_t sync_version;
	//u8 rssi,lqi;
	
	
	sync_version.hdr.slave_id = gEnv.id;
	sync_version.hdr.payload = PAYLOAD_RSSI_CHECK;
	sync_version.ver_major = REMOCON_VERSION_MAJOR;
	sync_version.ver_sub = REMOCON_VERSION_SUB;
	sync_version.ver_sub_sub = REMOCON_VERSION_SUB_SUB;
	sync_version.hdr.rssi = rssi;
	sync_version.hdr.lqi = lqi;
	
	set_freq();
	cc1101_tx(&sync_version,sizeof(sync_version_t),1);
	
	send_timer = 0;
	while(1)
	{
		wdt_reset();
		//cc1101_tx(&sync_version,sizeof(sync_version_t),1);
		
		set_freq();
		length = cc1101_rx((u8 *)&sync_version,rssi,lqi,RX_NOMAL);
		if(length > 0 )
		{

			//if(sync_version.hdr.slave_id == gEnv.id)
			//if((sync_version.hdr.slave_id == gEnv.id) && (sync_version.hdr.payload == PAYLOAD_RSSI_CHECK))
			{
				//rssi_fixed = rssi;
				send_timer = -1;
				return true;
			}
		}
		cc1100_cmd_idle();
		
		if(send_timer > 2000)
		{
			send_timer = -1;
			break;
		}
	}
	
	
	return false;
}






void set_process(u8 set)
{
	//int ret;
	get_door_open_status_t ret;
	int ret2;
	
	
	//if(set == true)
	if(set == SET)
	{
		for(int i=0;i<4;i++)
		{
			//ret = get_door_status();
			//ret.door = get_door_status();
			ret = get_door_status();
			if(ret.door != -1)
			{
				break;
			}
		}
	}
	else
	{
		ret.door = 0;
	}
	
	if(ret.door < 0)
	{
		set_voice(8);
		buzzer_error();
		return ;
	}
	
	//if(ret.door == 1 || ret.doorlock == 1)
	if(ret.door == true || ret.doorlock == true)
	{
		set_voice(6);
		buzzer_error();
	}
	else
	{
		//for(int i=0;i<4;i++)
		{
			ret2 = set_send(set);
			
			if(ret2 == true)
			{
							
				btn_ctl_deny_timeout_count = 0;
				//break;
			}
		}
		
		//if(set_send(set) == false)
		if(ret2 == false)
		{
			set_voice(8);
			buzzer_error();
			return;
		}
		#if 1
		//set_state = set;
		
		if(set_state == SET)
		{
			set_security_mode_led(true);
		}
		else
		{
			set_security_mode_led(false);	
		}
		
		//set_security_mode_led(set);
		if(set== SET)
		{
			//set_voice(1);
		}
		else
		{
			#if 0
			if(sound_mute == -1)
			{
				set_voice(2);
			}
			#endif
		
			//set_car_emergency_led(0);
		}
		#endif 	
	}
}



void button_check(void)
{
	// switch check
	if(btn_emg != get_emergency_button())
	{
		if(get_emergency_button() == true && emg_state == false)
		{
			btn_emg_count=0;
			buzzer_on(true);
		}
		else if(get_emergency_button() == true && emg_state == true)
		{
			btn_emg_count=0;
			buzzer_on(true);
		}
		else if(get_emergency_button() == false)
		{
			//buzzer_on(false);
		}
		
		btn_emg = get_emergency_button();
	}
	
	if(btn_open != get_cab_unlocked())
	{
		if(get_cab_unlocked() == true)
		{
			btn_open_count=0;
			buzzer_on(true);
		}
		else
		{
			//buzzer_on(false);
		}
		btn_open = get_cab_unlocked();
	}
	
	//if(set_state == false)
	{
		if(btn_rear_open != get_rear_door_unlocked())
		{
			u8 door = get_rear_door_unlocked();
		
			if(door == true)
			{
				btn_rear_open_count=0;
				buzzer_on(true);
			}
			else
			{
				//gBuzzer = false;
			}
			btn_rear_open = door;
		}
	}
	
	//if(set_state == false)
	{
		if(btn_side_open != get_side_door_unlocked())
		{
			if(get_side_door_unlocked() == true)
			{
				btn_side_open_count=0;
				buzzer_on(true);
			}
			else
			{
				//gBuzzer = false;
			}
			btn_side_open = get_side_door_unlocked();
		}
	}
	
	if(btn_set != get_security_set())
	{
		if(get_security_set() == true)
		{
			btn_set_count=0;
			buzzer_on(true);
		}
		else
		{
			//gBuzzer = false;
		}
		btn_set = get_security_set();
	}
	
	if(btn_reset != get_security_reset())
	{
		
		if(get_security_reset() == true)
		{
			btn_reset_count=0;
			buzzer_on(true);
		}
		else
		{
			//gBuzzer = false;
		}
		btn_reset = get_security_reset();
	}
	
	
	if(btn_emg_count == -1 && btn_open_count == -1 && btn_rear_open_count == -1 && btn_side_open_count == -1 && btn_set_count == -1 && btn_reset_count == -1)
	{
		buzzer_on(false);
	}
	
}



void button_process(void)
{
	if(btn_emg_count > 0)
	{
		if(get_emergency_button() == true)
		{
			if(emg_state == false && (btn_emg_count > gEnv.emg_btn_set - 1))
			{
				// emg_set
				//set_emg_button_led(true);
				emg_set_process();
				btn_emg_count = -1;
				
			}
			else if(emg_state == true && (btn_emg_count > gEnv.emg_btn_reset -1))
			{
				#if 0
				// emg_reset
				
				btn_emg_count = -1;
				//set_emg_button_led(false);
				emg_reset_process();
				#endif
			}
			else if(emg_state != true && emg_state != false && btn_emg_count > 6)
			{
				emg_state = false;
			}
		}
		else
		{
			btn_emg_count = -1;
		}
	}
	
	
	if(btn_open_count > 0)
	{
		if(get_cab_unlocked() == true)
		{
			if(btn_open_count > gEnv.set_time -1)
			{
				// driver door open
				btn_open_count = -1;
				emg_driver_door_process();
			}
		}
		else
		{
			btn_open_count = -1;
		}
	}
	
	
	if(btn_rear_open_count > 0)
	{
		if(get_rear_door_unlocked() == true)
		{
			if(btn_rear_open_count > gEnv.set_time -1)
			{
				// driver door open
				btn_rear_open_count = -1;
				emg_rear_door_process();
		
			}
		}
		else
		{
			btn_rear_open_count = -1;
		}
	}
	
	
	if(btn_side_open_count > 0)
	{
		if(get_side_door_unlocked() == true)
		{
			if(btn_side_open_count > gEnv.set_time -1)
			{
				// driver door open
				btn_side_open_count = -1;
				emg_side_door_process();
			}
		}
		else
		{
			btn_side_open_count = -1;
		}
	}
	
	if(btn_set_count > 0)
	{
		//if(temp_reset_count > -1 && ((door_open_msg_cancel == -1 && set_state == TEMP_RESET) || (door_open_msg_cancel == -1 && set_state == RESET)))
		//if(gPacket.hdr.payload == PAYLOAD_DRIVER_DOOR_ERR_REPORT && ((door_open_msg_cancel == -1 && set_state == TEMP_RESET) || (door_open_msg_cancel == -1 && set_state == RESET)))
		if(gPacket.hdr.payload == PAYLOAD_DRIVER_DOOR_ERR_REPORT && door_open_msg_cancel == -1)
		{
			if(get_security_set() == true)
			{
				if(btn_set_count > gEnv.set_time -1)
				{
					door_open_msg_cancel++;
					for(int i=0;i<6;i++)
					{
						buzzer_on(true);
						delay_ms(100);
						buzzer_on(false);
						delay_ms(100);
					}
				}
			}
			else
			{
				btn_set_count = -1;
			}		
		}
		else
		{
			if(get_security_set() == true)
			{
				if(btn_set_count > gEnv.set_time -1)
				{
					// driver door open
					btn_set_count = -1;
					set_process(SET);
					buzzer_on(false);
				}
			}
			else
			{
				btn_set_count = -1;
			}		
		}
	}
	
	
	if(btn_reset_count > 0)
	{
		if(get_security_reset() == true)
		{
			
			if(btn_reset_count > gEnv.reset_time -1)
			{
				// driver door open
				btn_reset_count = -1;
				set_process(RESET);
				
				set_emg_led(false);
				set_emg_button_led(false);
				remocon_emg = false;
				
				buzzer_on(false);
			}
		}
		else
		{
				btn_reset_count = -1;
		}
	}

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
	
	//_delay_ms(300);
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
set_state_ack
*/

void emg_ack(void)
{
	emg_t emg;
	
	emg.hdr.master_id = gMasterId;
	emg.hdr.slave_id = gEnv.id;
	emg.hdr.payload = PAYLOAD_EMG;
	
	set_freq();
	cc1101_tx(&emg,sizeof(emg_t),1);
	
}


/*

*/

void emg_door_ack(void)
{
	emg_door_t emg;
	
	emg.hdr.master_id = gMasterId;
	emg.hdr.slave_id = gEnv.id;
	emg.hdr.payload = PAYLOAD_EMG_DOOR;
	
	set_freq();
	cc1101_tx(&emg,sizeof(emg_door_t),1);
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
	//_delay_ms(300);
	set_freq();
	cc1101_tx(&setup,sizeof(setup_t),1);
}



void driver_door_err_report_ack(void)
{
	set_t set;
	
	set.hdr.master_id = gMasterId;
	set.hdr.slave_id = gEnv.id;
	set.hdr.payload = PAYLOAD_DRIVER_DOOR_ERR_REPORT;
	
	set_freq();
	cc1101_tx(&set,sizeof(set_t),1);
}






void wait_input_freq_adjust(void)
{
	int time_cnt=0;
	while(time_cnt < 300)
	{
		wdt_reset();
		delay_ms(10);
		time_cnt++;

		if(get_security_set()==true)
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
			
			set_security_mode_led(true);
			delay_ms(100);
			set_security_mode_led(false);
		}
		else if(get_security_reset()==true)
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
			set_emg_led(true);
			delay_ms(100);
			set_emg_led(false);
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
			set_emg_led(1);
			_delay_ms(50);
			set_emg_led(0);
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



void test_sound_message_mode(void)
{
	buzzer_on(false);

	while(1)
	{
		for(int i=1;i<9;i++)
		{

			set_voice(i);
			while(PORTA.IN & 0x02)
			{
				wdt_reset();
				_delay_ms(1000);
			}
		}
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

int g_length;
u8 gtemp; 
//packet_t gPacket;
u16 adc_value;


int main(void)
{
	u8 rssi,lqi;
	int length;
	u8 test_mode=0;
	volatile u16 temp;
	
	
	setClockTo32MHz();	
	
	port_init();
	spi_init();
	env_init();
	tele_env_init();
	timer_setup();
	TCC0_INTCTRLA = 0x03;				// enable OVF interrupt with high level
	
	set_speaker_power(true);
	//set_speaker_power(false);
	
	set_voice(0xff);
	
	gEnv.emg_btn_set = 4;
	gEnv.emg_btn_reset = 5;
	gEnv.set_time = 1;
	gEnv.reset_time = 1;
	gEnvDown.emg_btn_set = 4;
	gEnvDown.emg_btn_reset = 5;
	gEnvDown.set_time = 1;
	gEnvDown.reset_time = 1;
	
	cc1100_init(delay_ms,spi_write_single,spi_read_single,spi_write,spi_read,spi_select,spi_deselect,spi_read_somi);
	cc1101_init_reg();
	cc1101_8PATABLE_write_reg();

	wdt_enable(0x26);		//wdt timeout 4sec enable
	//wdt_enable(9);
	//WDT.CTRL = 0x11;		//wdt_setting
	//WDT.CTRL |= 0x03;		//wdt_enable
	wdt_reset();
	
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	sei();	
	

	btn_emg = get_emergency_button();
	btn_open = get_cab_unlocked();
	btn_rear_open = get_rear_door_unlocked();
	btn_side_open = get_side_door_unlocked();
	btn_set = get_security_set();
	btn_reset = get_security_reset();

	
	test_mode = ~get_test() & 0x0f;

	if((~get_dly() & 0x0f) == 0x04)
	{
		gDriver = true;
	}
	else if((~get_dly() & 0x0f) == 0x0c)
	{
		gDriver = false;
	}
	else
	{
		gDriver = false;
		mentenece_mode = 0;
	}
	
	
	if((test_mode != 0x00) && (test_mode != 0x01))
	{
		mentenece_mode = 0;
	}
	else if(test_mode == 0x01)
	{
		sound_mute = 0;
	}
	
	
	for(int i=0;i<3;i++)
	{
		wdt_reset();
		sync_version_send();
	}

	sync_rssi_send(&rssi,&lqi);
	sync2_send();

	//sound IC reset
	{
		PORTD.OUTCLR = PIN7_bm;
		_delay_ms(200);
		PORTD.OUTSET = PIN7_bm;
	}


    while(1)
    {

		
		
		set_freq();
		#if 0		
		test_wave_mode();
		
		continue;
		#endif 		

		wdt_reset();

		if(temp_reset_count > 5)
		{
			temp_reset_count=-1;
			set_security_mode_led(false);
		}
		else
		{
			#if 0
			if(temp_reset_count > -1)
			{
				set_security_mode_led(temp_reset_led_toggle);
			}
			#endif
		}
		
		
		if(set_state != TEMP_RESET && set_state != RESET)
		{
			if(door_open_msg_cancel != -1)
			{
				door_open_msg_cancel = -1;
			}
		}
		



		if((btn_emg_count > -1) || (btn_open_count > -1) || (btn_rear_open_count > -1) || (btn_side_open_count > -1) || (btn_set_count > -1) || (btn_reset_count > -1) || (btn_powerdown_count > -1) || (sleep_cansel_count > -1) || (btn_ctl_deny_timeout_count > -1) || (PORTA.IN & 0x02) || mentenece_mode > -1 || temp_reset_count > -1)
		{
			g_length = cc1101_rx((u8 *)&gPacket,&rssi,&lqi,RX_NOMAL);
		}
		else
		{
			g_length = cc1101_rx((u8 *)&gPacket,&rssi,&lqi,RX_WOR);
			//if(g_length != 0)
			{
				sleep_cansel_count = 0;				
			}
	
		}

		temp = read_adc();

		//if(temp < 0x068f)	//batt volt less than 3.7v
		//if(temp < 0x0731)	//batt volt less than 3.7v
		//if(temp < 0x0739 && batt_low_voice == -1)	//batt volt less than 3.7v
		//if(temp < 0x0744 && batt_low_voice == -1)	//batt volt less than 3.7v
		//if(temp < 0x0750 && batt_low_voice == -1)	//batt volt less than 3.7v
		//if(temp < 0x0762 && batt_low_voice == -1)	//batt volt less than 3.7v
		//if(temp < 0x06f8 && batt_low_voice == -1)	//batt volt less than 3.7v		
		//if(temp < 0x071a && batt_low_voice == -1)	//batt volt less than 3.7v
		if(temp < 0x072c && batt_low_voice == -1)	//batt volt less than 3.7v
		{
			for(int i=0;i<3;i++)
			{
				set_bat_led(true);
				_delay_ms(100);
				set_bat_led(false);
				_delay_ms(100);
			}
			
			if(batt_low_voice == -1)
			{
				set_voice(7);
				batt_low_voice=0;
			}

		}
		

		if(g_length == sizeof(packet_t))
		{
			gMasterId = gPacket.hdr.master_id;
			//gtemp = ~get_dly() & 0x0c;
			gtemp = ~get_test() & 0x0f;
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
						set_emg_led(1);
						_delay_ms(100);
						set_emg_led(0);
						_delay_ms(100);
					}
				}
			}
			else if(gPacket.hdr.slave_id == gEnv.id)
			{
				switch(gPacket.hdr.payload)
				{
					case PAYLOAD_SYNC:
						sync_ack();
						
						gEnv.emg_btn_set = gPacket.payload[1];
						gEnv.emg_btn_reset = gPacket.payload[2];
						gEnv.set_time=gPacket.payload[3];
						gEnv.reset_time=gPacket.payload[3];
						gEnvDown.emg_btn_set = gPacket.payload[1];
						gEnvDown.emg_btn_reset = gPacket.payload[2];
						gEnvDown.set_time=gPacket.payload[3];
						gEnvDown.reset_time=gPacket.payload[3];
						emg_state = gPacket.payload[4] ;
						set_state = gPacket.payload[5] ;

						break;
					case SET_STATE:
					{
						set_t * set;
						
						set_state_ack();

						set = (set_t *)	&gPacket;
						
						
						if(set_state != set->set)
						{
							set_state = set->set ;
							//set_security_mode_led(set->set);
						
							//if(set->set == true)
							//if(set->set == 1 || set->set == 2)
							if(set->set == SET)
							{
								set_security_mode_led(true);
								set_voice(1);
								temp_reset_count=-1;
								door_open_msg_cancel=-1;
							}
							else if(set->set == TEMP_RESET)
							{
								set_security_mode_led(false);
								if(sound_mute == -1)
								{
									set_voice(2);
								}

								set_car_emergency_led(0);
								set_emg_led(false);
								temp_reset_count=0;
																
							}
							else
							{
								set_security_mode_led(false);
								if(sound_mute == -1)
								{
									set_voice(2);
								}
								set_car_emergency_led(0);
								set_emg_led(false);
								set_emg_button_led(false);
								temp_reset_count=-1;
								//set_emg_button_led(false);
							}
							
						}
						
						break;
					}
					case PAYLOAD_EMG:
					{
						emg_t * emg;
						
						emg_ack();

						emg = (emg_t *)	&gPacket;
						
						
						
						emg_state = emg->emg ;
						
						if(remocon_emg == true)
						{
							remocon_emg = false;
							break;
						}
						
						set_emg_led(emg_state);
						
						if(emg_state == true)
						{
							//set_voice(1);
							main_emg = true;
							set_voice(5);
							// 
						}
						else
						{
							main_emg = false;
							set_emg_button_led(false);
							set_car_emergency_led(false);
						// buzzer 3 sec
							
						}
						break;
					}
						
					case PAYLOAD_EMG_DOOR:
					{
						emg_door_t * emg;
						
						emg_door_ack();

						emg = (emg_door_t *)	&gPacket;
						
					
						set_car_emergency_led(emg->emg);
						
						emg_state = emg->emg ;
						set_emg_led(emg_state);
						set_emg_button_led(false);
						
						if(emg->emg == true)
						{
							//set_voice(1);
							set_voice(4);
						}
						else
						{
							// buzzer 3 sec
							
						}
					
						break;
					}
					
					case PAYLOAD_DRIVER_DOOR_ERR_REPORT:
					{
						set_t * set;
						
						driver_door_err_report_ack();
						

						set = (set_t *)	&gPacket;						
						
						if(door_open_msg_cancel == -1 || door_open_msg_cancel > 18)
						{
							set_voice(6);
							buzzer_error();
							
							if(door_open_msg_cancel != -1)
							{
								door_open_msg_cancel = 0;
							}
						}
						else
						{
							door_open_msg_cancel++;
						}
						
						temp_reset_count=0;
						break;
					}
					
				}
				comm_led();

			} 
		}
		cc1100_cmd_idle();
		
		if(get_dly() & 0x01)
		{
			gEnv.emg_btn_set = gEnvDown.emg_btn_set;
			gEnv.emg_btn_reset = gEnvDown.emg_btn_reset;
			gEnv.set_time = gEnvDown.set_time;
			gEnv.reset_time = gEnvDown.set_time;
		}
		else
		{
			gEnv.emg_btn_set = 4;
			gEnv.emg_btn_reset = 5;
			gEnv.set_time = 1;
			gEnv.reset_time = 1;
		}
		
		if(btn_ctl_deny_timeout_count == -1)
		{	
			button_check();
			button_process();
		}






		if(get_security_set()==true && get_security_reset()==true)
		{
			btn_powerdown_count = 0;
			buzzer_on(true);
			
			while(1)
			{
				wdt_reset();
				
				if(get_security_set() != true || get_security_reset() != true)
				{
					btn_powerdown_count = -1;
					break;
				}
				
				
				if(btn_powerdown_count > 2)
				{

					//btn_powerdown_count = -1;
					buzzer_on(false);
					for(int i=0;i<10;i++)
					{
						wdt_reset();
						set_bat_led(true);
						set_communication_led(true);
						set_car_emergency_led(true);
						set_emg_led(true);
						set_emg_button_led(true);
						set_security_mode_led(true);
						_delay_ms(300);
						set_bat_led(false);
						set_communication_led(false);
						set_car_emergency_led(false);
						set_emg_led(false);
						set_emg_button_led(false);
						set_security_mode_led(false);
						_delay_ms(300);
										
					}

					#if 0
										
					while(btn_powerdown_count < 10);
					set_communication_led(true);
					_delay_ms(300);
					set_communication_led(false);
					
					
					

					for(int i=0;i<8;i++)
					{
						//sync_rssi_send(&rssi,&lqi);
						
						int length;
						volatile sync_version_t sync_version;
						//u8 rssi,lqi;
						
						wdt_reset();
						sync_version.hdr.slave_id = gEnv.id;
						sync_version.hdr.payload = PAYLOAD_RSSI_CHECK;
						sync_version.ver_major = REMOCON_VERSION_MAJOR;
						sync_version.ver_sub = REMOCON_VERSION_SUB;
						sync_version.ver_sub_sub = REMOCON_VERSION_SUB_SUB;
						sync_version.hdr.rssi = rssi;
						sync_version.hdr.lqi = lqi;
						
						set_freq();
						cc1101_tx(&sync_version,sizeof(sync_version_t),1);
						
						send_timer = 0;	
						
						while(1)
						{

							set_freq();
							length = cc1101_rx((u8 *)&sync_version,rssi,lqi,RX_NOMAL);
							if(length > 0 )
							{

								//if(sync_version.hdr.slave_id == gEnv.id)
								//if((sync_version.hdr.slave_id == gEnv.id) && (sync_version.hdr.payload == PAYLOAD_RSSI_CHECK))
								{
									//rssi_fixed = rssi;
									send_timer = -1;
									break;
								}
							}
							cc1100_cmd_idle();
							
							if(send_timer > 2000)
							{
								send_timer = -1;
								break;
							}
						}	
						
						
					}
					#endif
					
					
					for(int i=0;i<3;i++)
					{
						wdt_reset();
						sync_version_send();
					}
					
					set_speaker_power(false);

					PORTC.INT1MASK=0x04;
					PORTC.INTCTRL=0x0C;
					PORTA.INT1MASK=0xf8;
					PORTA.INTCTRL=0x0C;
				
					cc1100_strobe_cmd(CC1100_STROBE_SPWD);
					
					SLEEP_CTRL |= SLEEP_SMODE_PDOWN_gc;
					SLEEP_CTRL |= SLEEP_SEN_bm;
	
	
					while(1)
					{
						wdt_disable();
						
						sleep();
						
						wdt_enable(0x26);		//wdt timeout 4sec enable
						
						if(get_security_reset() || get_security_set() || get_side_door_unlocked() || get_rear_door_unlocked() || get_emergency_button() || get_cab_unlocked())
						{
							btn_powerdown_count = 0;
							
							#if 0
							//sound IC reset
							{
								PORTD.OUTCLR = PIN7_bm;
								_delay_ms(200);
								PORTD.OUTSET = PIN7_bm;
							}
							#endif
							
							while(1)
							{
								if(btn_powerdown_count > 2)
								{
	
									for(int i=0;i<10;i++)
									{
										wdt_reset();
										set_bat_led(true);
										set_communication_led(true);
										set_car_emergency_led(true);
										set_emg_led(true);
										set_emg_button_led(true);
										set_security_mode_led(true);
										_delay_ms(100);
										set_bat_led(false);
										set_communication_led(false);
										set_car_emergency_led(false);
										set_emg_led(false);
										set_emg_button_led(false);
										set_security_mode_led(false);
										_delay_ms(100);
									}
	
									//wdt_enable(WDTO_1S);
									
									while(1);
								
								
								}
								
								if(!get_security_reset() && !get_security_set() && !get_side_door_unlocked() && !get_rear_door_unlocked() && !get_emergency_button())
								{
									btn_powerdown_count = -1;
									break;
								}
								
							}
						}					
					}

					
					PORTC.INT1MASK=0x00;
					PORTA.INT1MASK=0x00;
					
					break;
				}
				
				if(get_security_reset()==false || get_security_set()==false)
				{
					break;
				}
				

			}
			
			buzzer_on(false);
			btn_powerdown_count = -1;
		}	

		
		
		if(test_mode == 0x0d || test_mode == 0x0e)
		{
			test_wave_mode(test_mode);
		}
		
		
		
		if(test_mode == 0x02)
		{
			test_sound_message_mode();
		}
		
		
		
    }
}