/*
 * 
 *
 * 
 * 
 */ 


#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <includes.h>
#include "lcd.h"
#include "key.h"
#include "spi.h"
#include "env.h"
#include "menu.h"
#include "Oper.h"


static OS_STK TaskStartStk[APP_CFG_TASK_STK_SIZE];
static OS_STK TaskStk[APP_CFG_N_TASKS][APP_CFG_TASK_STK_SIZE];

//void MenuTask(void *p_arg);
void RFRecvTask(void *p_arg);
void OperTask(void *p_arg);
void EventTask(void *p_arg);
void SoundPlayTask(void *p_arg);
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
void uartRxTask(void *p_arg);


>>>>>>> parent of f8ff870... test063003
=======
void uartRxTask(void *p_arg);


>>>>>>> parent of f8ff870... test063003
=======
void uartRxTask(void *p_arg);


>>>>>>> parent of f8ff870... test063003

KEY_DATA	stKey;
OS_EVENT    *KeyWait;

u8 g_key_status;


void setClockTo32MHz()
{
	CCP = CCP_IOREG_gc;              // disable register security for oscillator update
	OSC.CTRL = OSC_RC32MEN_bm;       // enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
	CCP = CCP_IOREG_gc;              // disable register security for clock update
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock
}


// Disable a Timer/Counter type 0
void tc0_disable(TC0_t *ptc)
{
	// Timer/Counter off
	ptc->CTRLA=(ptc->CTRLA & (~TC0_CLKSEL_gm)) | TC_CLKSEL_OFF_gc;
	// Issue a reset command
	ptc->CTRLFSET=TC_CMD_RESET_gc;
} // tc0_disable()

// Disable a Timer/Counter type 1
void tc1_disable(TC1_t *ptc)
{
	// Timer/Counter off
	ptc->CTRLA=(ptc->CTRLA & (~TC1_CLKSEL_gm)) | TC_CLKSEL_OFF_gc;
	// Issue a reset command
	ptc->CTRLFSET=TC_CMD_RESET_gc;
} // tc1_d


void ClockTickStart(void)
{
	// Local variables
	unsigned char s;
	uint32_t  clk_per_freq;
	uint32_t  period;
	// Save interrupts enabled/disabled state
	s=SREG;
	// Disable interrupts
	asm("cli");
	// Disable and reset the timer/counter just to be sure
	tc1_disable(&TCC1);
	// Clock source: Peripheral Clock/1
	TCC1.CTRLA=(TCC1.CTRLA & (~TC1_CLKSEL_gm)) | TC_CLKSEL_DIV1_gc;
	// Mode: Normal Operation, Overflow Int./Event on TOP
	// Compare/Capture on channel A: Off
	// Compare/Capture on channel B: Off
	TCC1.CTRLB=(TCC1.CTRLB & (~(TC1_CCAEN_bm | TC1_CCBEN_bm | TC1_WGMODE_gm))) |
	TC_WGMODE_NORMAL_gc;
	// Capture event source: None
	// Capture event action: None
	TCC1.CTRLD=(TCC1.CTRLD & (~(TC1_EVACT_gm | TC1_EVSEL_gm))) |
	TC_EVACT_OFF_gc | TC_EVSEL_OFF_gc;
	// Overflow interrupt: High Level
	// Error interrupt: Disabled
	TCC1.INTCTRLA=(TCC1.INTCTRLA & (~(TC1_ERRINTLVL_gm | TC1_OVFINTLVL_gm))) |
	TC_ERRINTLVL_OFF_gc | TC_OVFINTLVL_HI_gc;
	// Compare/Capture channel A interrupt: Disabled
	// Compare/Capture channel B interrupt: Disabled
	TCC1.INTCTRLB=(TCC1.INTCTRLB & (~(TC1_CCBINTLVL_gm | TC1_CCAINTLVL_gm))) |
	TC_CCBINTLVL_OFF_gc | TC_CCAINTLVL_OFF_gc;
	// High resolution extension: Off
	HIRESC.CTRLA&= ~HIRES_HREN1_bm;
	// Clear the interrupt flags
	TCC1.INTFLAGS=TCC1.INTFLAGS;
	// Set counter register
	TCC1.CNT=0x0000;
	// Calculate period
	// Note: The following equation to calculate the period assumes prescaler=CLK/1
	clk_per_freq   =  F_CPU;
	period         =  (uint32_t)(((2 * clk_per_freq) + (1 * 2 * (uint32_t)OS_TICKS_PER_SEC))
	/             ((1 * 2 * (uint32_t)OS_TICKS_PER_SEC)));
	// Set timer period register (maximum value=(2^16)-1=65535)
	TCC1.PER=period;
	// Set channel A Compare/Capture register
	TCC1.CCA=0x0000;
	// Set channel B Compare/Capture register
	TCC1.CCB=0x0000;
	// Restore interrupts enabled/disabled state
	SREG=s;
} // ClockTickStart()



#if 1
const char * strMenu[7]=
{
"1.シュウ?ュウ?ウ?(A)ID",
"2.ウンテンシュ(B)ID", 
"3.ケイジョウイン(C)",
"4.ウシロトビラ(D)ID",
"5.ヨコトビラ(D)ID", 
"6.キャビン?キ?(D)ID",
"7.キャビンヒ?(D)ID",
};

#endif

static void delay_ms(u16 time_ms)		/* time delay for ms(1~65535 ms) for 32MHz */
{
	OSTimeDlyHMSM(0,0,0,time_ms);
}


void system_reset(void)
{
	// softreset	
    CCP = CCP_IOREG_gc;
    RST.CTRL =  RST_SWRST_bm;	
}

#define sleep() __asm__ __volatile__ ("sleep")
void OS_Stop(void)
{
	
	TCC0_INTCTRLA = 0x00;				// enable OVF interrupt with high level
	SLEEP_CTRL |= SLEEP_SMODE_PSAVE_gc;
	//SLEEP_CTRL |= SLEEP_SMODE_PDOWN_gc;
	SLEEP_CTRL |= SLEEP_SEN_bm;
	
	sleep();
}


void portb_int(void)
{


}


u32 temp;
u8 bOSStop= false;


// StartTask: PRIORITY=2
static void MainTask(void *p_arg)
{
	u16	cur,old,key; 
	u8  status;	
	u32 Os_GetTime=0;
	u32 BackLight_GetTime=0;

	
	ClockTickStart();
	TCC0_INTCTRLA = 0x03;				// enable OVF interrupt with high level
	PMIC_CTRL = 0x07;				// enable HI, MED, LO level interrupt

	env_init();
	tele_env_init();

	KeyWait  = OSSemCreate(0);             
	
		
	// Create tasks
	TaskStartCreateTasks();
	
	cur = old = 0x00;  
	stKey.key = 0;
	stKey.time = 0;
	status = KEY_ST_NONE;
	g_key_status = KEY_ST_NONE;
	
	#if 0
	for (int i=0;i<27;i++)
	{
		SoundPlay(i);
		OSTimeDlyHMSM(0,0,6,0);
		
	}
	#endif
	
	
	if(gEnv.main_set.startDelayResetFlag < 2)
	{
		gEnv.main_set.startDelayResetFlag++;
		env_save(&gEnv);
		OSTimeDlyHMSM(0,0,5,0);
		system_reset();

	}
	else
	{
		gEnv.main_set.startDelayResetFlag = (u32)0;
		env_save(&gEnv);
	}
	
	

	
	while(1)
	{
		

		
		
		OSTimeDlyHMSM(0,0,0,50);
	}
} // TaskStart()



void TaskStartCreateTasks(void)
{
	
	#if 0
	// Task1: PRIORITY=3
	OSTaskCreateExt((void (*)(void *)) MenuTask,
	(void           *) 0,
	(OS_STK         *)&TaskStk[0][APP_CFG_TASK_STK_SIZE - 1],
	(INT8U           ) 3,
	(INT16U          ) 3,
	(OS_STK         *)&TaskStk[0][0],
	(INT32U          ) APP_CFG_TASK_STK_SIZE , 
	(void           *) 0,
	(INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	#endif
	


	#if 0
	// Task2: PRIORITY=4
	OSTaskCreateExt((void (*)(void *)) RFRecvTask,
	(void           *) 0,
	(OS_STK         *)&TaskStk[1][APP_CFG_TASK_STK_SIZE - 1],
	(INT8U           ) 4,
	(INT16U          ) 4,
	(OS_STK         *)&TaskStk[1][0],
	(INT32U          ) APP_CFG_TASK_STK_SIZE,
	(void           *) 0,
	(INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	#endif
	
	
	
	#if 0
	// Task2: PRIORITY=4
	OSTaskCreateExt((void (*)(void *)) OperTask,
	(void           *) 0,
	(OS_STK         *)&TaskStk[2][APP_CFG_TASK_STK_SIZE - 1],
	(INT8U           ) 5,
	(INT16U          ) 5,
	(OS_STK         *)&TaskStk[2][0],
	(INT32U          ) APP_CFG_TASK_STK_SIZE,
	(void           *) 0,
	(INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	#endif
	
	
	// Task2: PRIORITY=6
	OSTaskCreateExt((void (*)(void *)) EventTask,
	(void           *) 0,
	(OS_STK         *)&TaskStk[3][APP_CFG_TASK_STK_SIZE - 1],
	(INT8U           ) 6,
	(INT16U          ) 6,
	(OS_STK         *)&TaskStk[3][0],
	(INT32U          ) APP_CFG_TASK_STK_SIZE,
	(void           *) 0,
	(INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	
	#if 0
	// Task2: PRIORITY=7
	OSTaskCreateExt((void (*)(void *)) SoundPlayTask,
	(void           *) 0,
	(OS_STK         *)&TaskStk[4][APP_CFG_TASK_STK_SIZE - 1],
	(INT8U           ) 7,
	(INT16U          ) 7,
	(OS_STK         *)&TaskStk[4][0],
	(INT32U          ) APP_CFG_TASK_STK_SIZE,
	(void           *) 0,
	(INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	#endif 
	
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
} 


//void uart_init(void);
=======
=======
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003
	#if 1
	// Task2: PRIORITY=7
	OSTaskCreateExt((void (*)(void *)) uartRxTask,
	(void           *) 0,
	(OS_STK         *)&TaskStk[4][APP_CFG_TASK_STK_SIZE - 1],
	(INT8U           ) 7,
	(INT16U          ) 7,
	(OS_STK         *)&TaskStk[4][0],
	(INT32U          ) APP_CFG_TASK_STK_SIZE,
	(void           *) 0,
	(INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	#endif
} 


void uart_init(void);
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003
void SoundPlay(u8 number);


int main(void)
{
	setClockTo32MHz();
	port_init();
	spi_init();
	
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======


>>>>>>> parent of f8ff870... test063003
=======


>>>>>>> parent of f8ff870... test063003
=======


>>>>>>> parent of f8ff870... test063003
	lcd_start();
	lcd_init();
	
	lcd_clear();
	lcd_position(0,0);
	lcd_prString("ウンテンセキトビラ");
	//uart_init();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
	uart_init();
	
>>>>>>> parent of f8ff870... test063003
=======
	uart_init();
	
>>>>>>> parent of f8ff870... test063003
=======
	uart_init();
	
>>>>>>> parent of f8ff870... test063003
	
	//set_mp3_active(true);
	//sendChar(0xef);					//sound stop	
	//SoundPlay(1);
    // Initialize uC/OS-II
    OSInit();
    // Create the start task
    OSTaskCreateExt((void (*)(void *)) MainTask,
    (void           *) 0,
    (OS_STK         *)&TaskStartStk[APP_CFG_TASK_STK_SIZE - 1],
    (INT8U           ) APP_CFG_TASK_START_PRIO,
    (INT16U          ) APP_CFG_TASK_START_PRIO,
    (OS_STK         *)&TaskStartStk[0],
    (INT32U          )(APP_CFG_TASK_STK_SIZE),
    (void           *) 0,
    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    // Create semaphores
    // Start multi-tasking
	
    OSStart();
}

