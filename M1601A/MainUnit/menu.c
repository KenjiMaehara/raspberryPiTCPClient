/*
 * 
 *
 * 
 * 
 */

#include <stdlib.h>
#include <avr/io.h> 
#include <avr/eeprom.h>
#include "menu.h"
#include "key.h"
#include "env.h"
#include "oper.h"
#include "event.h"



#ifdef NEW_TEST_BUZZER_LIMIT
u32 gOs_BuzzerTimeLimit_GetTime;
#endif



scr_mode_t	scr_mode;
u8			menu_mode; // user , mel
u8			radio_mode,old_radio_mode;
u8			exe_state;
u8			devicePairingNumber;

u8 menu_nokey(u8 init)
{
	static int nokey=0;
	
	if(init == false)
	{
		nokey = 0;
	}
	else
	{
		nokey++;
		if(nokey > 2500)
		{
			nokey = 0;
			return true;
		}
	}
	
	return false;
}


u8 menu_nokey2(u8 init)
{
	static int nokey=0;
	
	if(init == false)
	{
		nokey = 0;
	}
	else
	{
		nokey++;
		if(nokey > 9)
		{
			nokey = 0;
			return true;
		}
	}
	
	return false;
}



SCREEN_MODE menu_get_mode(void)
{
	return scr_mode.screen;
}

void MenuTask(void *p_arg)
{
	int ret;
	
	lcd_clear();

	scr_mode.mode = 0;
	scr_mode.screen = LIVE_SCR;
	scr_mode.update = false;

	while(1)
	{
		switch(scr_mode.screen)
		{
			case LIVE_SCR:
				menu_live_scr();
				break;
			case MENU_SCR:
				menu_nokey(true);
				main_menu(&scr_mode);
				lcd_clear();
				scr_mode.update = true;
				scr_mode.screen = LIVE_SCR;
				break;
			case VERSION_SCR:
				//menu_version_scr();
				scr_mode.update = true;
				scr_mode.screen = LIVE_SCR;
				break;
		}
	}
} 


void menu_change_screen(u8 scr,u8 mode)
{
	if(scr_mode.screen != MENU_SCR)
	{
		scr_mode.mode = mode;
		scr_mode.screen = scr;
		scr_mode.update = true;
	}
}



void key_long_press()
{
	{
		if(stKey.time > 100)
		{
			stKey.key |= 0x8000;
		}
	}
}

int	key_key2int(u16 key)
{
	switch(key)
	{
		case KEY_0: return 0;
		case KEY_1: return 1;
		case KEY_2: return 2;
		case KEY_3: return 3;
		case KEY_4: return 4;
		case KEY_5: return 5;
		case KEY_6: return 6;
		case KEY_7: return 7;
		case KEY_8: return 8;
		case KEY_9: return 9;
	}
	return -1;
}


u8	key_key2u8(u16 key)
{
	switch(key)
	{
		case KEY_0: return '0';
		case KEY_1: return '1';
		case KEY_2: return '2';
		case KEY_3: return '3';
		case KEY_4: return '4';
		case KEY_5: return '5';
		case KEY_6: return '6';
		case KEY_7: return '7';
		case KEY_8: return '8';
		case KEY_9: return '9';
		case KEY_A: return 'A';
		case KEY_B: return 'B';
		case KEY_C: return 'C';
		case KEY_D: return 'D';
		case KEY_E: return 'E';
		case KEY_F: return 'F';
		case KEY_SHARP: return '#';
		case KEY_STAR: return '*';
		//case KEY_LEFT: return 'L';
		//case KEY_RIGHT: return 'R';
		case KEY_UP: return 'U';
		case KEY_DOWN: return 'D';
		case KEY_EMG: return 'Z';
		case KEY_SAFETY: return 'S';
		case KEY_MEL: return 'M';
		case KEY_SECURITY: return 'E';
		case KEY_DOOR: return 'O';
		default:
		return 0;
	}
	return 0;
}

u8 oper_get_emg_state(void);
//extern u8 bOSStop;

//void key_proc(u8 * buf,int length)
u8 key_proc(u8 * buf,int length)
{
	u8              str[20];
	u8 *            pass;
	int             password,i;
	volatile u16				pass16;
	
	u8				value[20];
	int ret=0;

	sprintf((char *)str,"760928#");
	if(strstr((char *)buf,(char *)str) != 0)
	{
		menu_change_screen(MENU_SCR,1);
		return true;
	}

	sprintf((char *)str,"8888#");
	if(strstr((char *)buf,(char *)str) != 0)
	{
		bOSStop = true;
		return true;
	}


	if(strstr((char *)buf,"PDW") != 0)
	{
		// power down
		oper_insert(OPER_POWERDOWN,0);
		
		return true;
	}
	
	if(strstr((char *)buf,"RST") != 0)
	{
		// power down
		oper_insert(OPER_SYSRESET,0);
		
		return true;
	}

	if(strstr((char *)buf,"VER") != 0)
	{
		// power down
		oper_insert(OPER_VERSIONCHECK,0);
		
		return true;
	}

	if(strstr((char *)buf,"EE#") != 0)
	{
		menu_change_screen(MENU_SCR,1);
		return true;
	}
	
	if(strstr((char *)buf,"E#") != 0 && length==2)
	{
		oper_insert(OPER_SET,0);
		return true;
	}
	
	
	#if 0
	if(strstr((char *)buf,"E*") != 0)
	{
		// reset
		oper_insert(OPER_RESET,0);
		return true;
	}
	#endif
	
	if(strstr((char *)buf,"LSE") != 0)
	{
		// emg occur
		
		oper_insert(OPER_EMG,0);
		
		return true;
	}
	
	if(strstr((char *)buf,"LSA") != 0)
	{
		//oper_insert(OPER_CHECK,0);
		oper_insert(OPER_RESET,0);
		return true;
	}

	if(strstr((char *)buf,"SECUSET") != 0)
	{
		//oper_insert(OPER_CHECK,0);
		oper_insert(OPER_SET,0);
		return true;
	}

	
	if(strstr((char *)buf,"M#") != 0)
	{
		// emg reset
		oper_insert(OPER_AUDIO_MINUS,0);
		return true;
	}
	
	
	if(strstr((char *)buf,"M*") != 0)
	{
		// emg reset
		oper_insert(OPER_AUDIO_PLUS,0);
		return true;
	}
	
	
	//if(buf[0] == 'O' && buf[2] == '#' && (buf[1] == '0' || buf[1] == '1' || buf[1] == '2' || buf[1] == '3' || buf[1] == '4') )
	if(buf[0] == 'O' && buf[2] == '#' && (buf[1] == '7' || buf[1] == '1' || buf[1] == '2' || buf[1] == '3' || buf[1] == '4') )
	{
		oper_insert(OPER_DOOR,buf[1] - 0x030);
		return true;
	}
	
	
	pass16 = (u16)gEnv.main_set.engine_start;
	sprintf((char *)str,"%04d#",pass16);
	str[5] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		//if(event_get_state()==SET)
		if((event_get_state()==SET) || (event_get_state()==MAINTENCE))
		{
			if(event_get_state()==MAINTENCE)
			{
				event_set_state(SET);
			}
			
			oper_insert(OPER_ENG_START,0);
			return true;
		}
		else
		{
			return false;
		}

	}
	
	pass16 = (u16)gEnv.main_set.engine_start;
	sprintf((char *)str,"%04d*",pass16);
	str[5] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		oper_insert(OPER_ENG_RESET,0);
		return true;
	}	
	
	
	
	#if 1
	pass16 = (u16)gEnv.main_set.engine_start;
	sprintf((char *)str,"%04d%s",pass16,"Z");
	
	if(memcmp(buf,str,5)==0 && buf[9]=='#')
	{
		memcpy(value,&buf[5],4);
		value[4] = '\0';
		//value[4] = NULL;
		gEnv.main_set.engine_start = strtoul(value,NULL,10);
		env_save(&gEnv);
		return true;
	}
	
	#endif
	
	
	
	#if 0
	pass16 = (u16)gEnv.main_set.engine_start;
	sprintf((char *)str,"%04d",pass16);
	//sprintf((char *)str,"%04dZ",pass16);
	//str[5] = 0;
	
	memcpy(value,buf,4);
	
	if(strstr((char *)value,(char *)str) != 0)
	{
		if(buf[4] == 'Z' && buf[9] == '#')		
		{
			
			value[0] = buf[5];
			value[1] = buf[6];
			value[2] = buf[7];
			value[3] = buf[8];
			
			gEnv.main_set.engine_start = strtoul(value,NULL,10);
			env_save(&gEnv);
			//memcpy(result,value,5);	
			return true;
		}
		//oper_insert(OPER_ENG_RESET,0);

	}
	#endif
	
	
	pass16 = (u16)gEnv.main_set.service_code;
	sprintf((char *)str,"%04d#",pass16);
	str[5] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		oper_insert(OPER_SERVICE,0);
		return true;
	}
	
	
	pass16 = (u16)gEnv.main_set.service_code;
	sprintf((char *)str,"%04dE#",pass16);
	str[6] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		menu_change_screen(MENU_SCR,1);
		return true;
	}
	
	pass16 = (u16)gEnv.main_set.service_code;				//PARING_REMO_DRIVER
	sprintf((char *)str,"%04dM1#",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_PAIRING_REMO_DRIVER;
		oper_insert(OPER_PAIRING_REMO_DRIVER,1);
		return true;
	}


	pass16 = (u16)gEnv.main_set.service_code;				//PARING_REMO_SECURITY
	sprintf((char *)str,"%04dM2#",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_PARING_REMO_SECURITY;
		oper_insert(OPER_PARING_REMO_SECURITY,1);
		return true;
	}

	pass16 = (u16)gEnv.main_set.service_code;				//PARING_EX_REMO_DRIVER
	sprintf((char *)str,"%04dMS#",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_PAIRING_EX_REMO_DRIVER;
		oper_insert(OPER_PAIRING_EX_REMO_DRIVER,1);
		return true;
	}


	pass16 = (u16)gEnv.main_set.service_code;				//PARING_EX_REMO_SECURITY
	sprintf((char *)str,"%04dME#",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_PARING_EX_REMO_SECURITY;
		oper_insert(OPER_PARING_EX_REMO_SECURITY,1);
		return true;
	}



	pass16 = (u16)gEnv.main_set.service_code;				//PARING_DOORCTL_CABIN_RIGHT
	sprintf((char *)str,"%04dM3#",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_PARING_DOORCTL_CABIN_RIGHT;
		oper_insert(OPER_PARING_DOORCTL_CABIN_RIGHT,1);
		return true;
	}

	pass16 = (u16)gEnv.main_set.service_code;				//PARING_DOORCTL_CABIN_LEFT
	sprintf((char *)str,"%04dM4#",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_PARING_DOORCTL_CABIN_LEFT;
		oper_insert(OPER_PARING_DOORCTL_CABIN_LEFT,1);
		return true;
	}


	pass16 = (u16)gEnv.main_set.service_code;				//PARING_DOORCTL_SIDE
	sprintf((char *)str,"%04dM5#",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_PARING_DOORCTL_SIDE;
		oper_insert(OPER_PARING_DOORCTL_SIDE,1);
		return true;
	}


	pass16 = (u16)gEnv.main_set.service_code;				//PARING_DOORCTL_BACK
	sprintf((char *)str,"%04dM6#",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_PARING_DOORCTL_BACK;
		oper_insert(OPER_PARING_DOORCTL_BACK,1);
		return true;
	}


	pass16 = (u16)gEnv.main_set.service_code;				//
	sprintf((char *)str,"%04dM1*",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_NOTUSE_REMO_DRIVER;
		oper_insert(OPER_NOTUSE_REMO_DRIVER,1);
		return true;
	}


	pass16 = (u16)gEnv.main_set.service_code;				//
	sprintf((char *)str,"%04dM2*",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_NOTUSE_REMO_SECURITY;
		oper_insert(OPER_NOTUSE_REMO_SECURITY,1);
		return true;
	}

	pass16 = (u16)gEnv.main_set.service_code;				//
	sprintf((char *)str,"%04dMS*",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_NOTUSE_EX_REMO_DRIVER;
		oper_insert(OPER_NOTUSE_EX_REMO_DRIVER,1);
		return true;
	}


	pass16 = (u16)gEnv.main_set.service_code;				//
	sprintf((char *)str,"%04dME*",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_NOTUSE_EX_REMO_SECURITY;
		oper_insert(OPER_NOTUSE_EX_REMO_SECURITY,1);
		return true;
	}



	pass16 = (u16)gEnv.main_set.service_code;				//
	sprintf((char *)str,"%04dM3*",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_NOTUSE_DOORCTL_CABIN_RIGHT;
		oper_insert(OPER_NOTUSE_DOORCTL_CABIN_RIGHT,1);
		return true;
	}

	pass16 = (u16)gEnv.main_set.service_code;				//
	sprintf((char *)str,"%04dM4*",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_NOTUSE_DOORCTL_CABIN_LEFT;
		oper_insert(OPER_NOTUSE_DOORCTL_CABIN_LEFT,1);
		return true;
	}


	pass16 = (u16)gEnv.main_set.service_code;				//
	sprintf((char *)str,"%04dM5*",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_NOTUSE_DOORCTL_SIDE;
		oper_insert(OPER_NOTUSE_DOORCTL_SIDE,1);
		return true;
	}


	pass16 = (u16)gEnv.main_set.service_code;				//
	sprintf((char *)str,"%04dM6*",pass16);
	str[7] =0;
	if(strstr((char *)buf,(char *)str) != 0)
	{
		devicePairingNumber = OPER_NOTUSE_DOORCTL_BACK;
		oper_insert(OPER_NOTUSE_DOORCTL_BACK,1);
		return true;
	}

	if(strstr((char *)buf,"STOPPAIR") != 0)
	{
		//oper_insert(OPER_SET,0);
		Os_PairingDeviceOnTime = 0;
		event_set_state(RESET);
		radio_mode=RF_NOMAL_MODE;
		return true;
	}

	return false;
	
}


u8	key_buf[21];

void menu_live_scr(void)
{

	u8			err;
	u8			key_index;

	key_index =  0;

	while(1)
	{
		OSSemPend(KeyWait, 100, &err);

		if(err != 0)
		{
			if(scr_mode.update == true) break;

			continue;
		}
		key_long_press();


		switch(stKey.key)
		{
			case L_KEY_SECURITY:
				key_buf[0] = 'S';
				key_buf[1] = 'E';
				key_buf[2] = 'C';
				key_buf[3] = 'U';
				key_buf[4] = 'S';
				key_buf[5] = 'E';
				key_buf[6] = 'T';
				key_buf[7] = 0;
				key_proc(key_buf,7);
				key_index = 0;	
				break;			
			case KEY_MEL:
				key_buf[key_index++] = 'M';
				break;
			case L_KEY_MEL:
				
				#if 0
				key_buf[0] = 'P';
				key_buf[1] = 'D';
				key_buf[2] = 'W';
				key_buf[3] = 0;
				key_proc(key_buf,3);
				key_index = 0;
				#endif
				
				break;
			case L_KEY_STAR:
				if(event_get_state()!=DEVICE_PAIRING)
				{
					key_buf[0] = 'R';
					key_buf[1] = 'S';
					key_buf[2] = 'T';
					key_buf[3] = 0;
					key_proc(key_buf,3);
					key_index = 0;
				}
				else
				{
					key_buf[0] = 'S';
					key_buf[1] = 'T';
					key_buf[2] = 'O';
					key_buf[3] = 'P';
					key_buf[4] = 'P';
					key_buf[5] = 'A';
					key_buf[6] = 'I';
					key_buf[7] = 'R';
					key_buf[8] = 0;
					key_proc(key_buf,8);
					key_index = 0;					
				}
				break;
			case L_KEY_SHARP:
				key_buf[0] = 'V';
				key_buf[1] = 'E';
				key_buf[2] = 'R';
				key_buf[3] = 0;
				key_proc(key_buf,3);
				key_index = 0;
				break;
			case L_KEY_EMG:	
				key_buf[0] = 'L';
				key_buf[1] = 'S';
				key_buf[2] = 'E';
				key_buf[3] = 0;
				key_proc(key_buf,3);
				key_index = 0;
				break;
			case L_KEY_SAFETY:
				key_buf[0] = 'L';
				key_buf[1] = 'S';
				key_buf[2] = 'A';
				key_buf[3] = 0;
				key_proc(key_buf,3);
				key_index = 0;
				break;
			case KEY_SHARP:
			case KEY_STAR:
				key_buf[key_index++] = key_key2u8(stKey.key);
				key_buf[key_index]=0;
				if(stKey.key==KEY_SHARP)
				{
					if(key_proc(key_buf,key_index)==true)
					{
						
						#ifdef NEW_TEST_BUZZER_LIMIT
						gOs_BuzzerTimeLimit_GetTime = OSTimeGet() + OS_TICKS_PER_SEC * 5;
						#endif
						
						set_buzzer_drv(false);
						OSTimeDlyHMSM(0,0,0,50);
						set_buzzer_drv(true);
						OSTimeDlyHMSM(0,0,0,50);
						set_buzzer_drv(false);
						OSTimeDlyHMSM(0,0,0,50);
						set_buzzer_drv(true);
						OSTimeDlyHMSM(0,0,0,50);
						set_buzzer_drv(false);
						
						#ifdef NEW_TEST_BUZZER_LIMIT
						gOs_BuzzerTimeLimit_GetTime = 0;
						#endif
					}
					else
					{
						#ifdef NEW_TEST_BUZZER_LIMIT
						gOs_BuzzerTimeLimit_GetTime = OSTimeGet() + OS_TICKS_PER_SEC * 5;
						#endif
						set_buzzer_drv(false);
						OSTimeDlyHMSM(0,0,0,50);
						set_buzzer_drv(true);
						OSTimeDlyHMSM(0,0,0,50);
						set_buzzer_drv(false);
						OSTimeDlyHMSM(0,0,0,50);
						set_buzzer_drv(true);
						OSTimeDlyHMSM(0,0,0,700);
						set_buzzer_drv(false);
						
						#ifdef NEW_TEST_BUZZER_LIMIT
						gOs_BuzzerTimeLimit_GetTime = 0;
						#endif
					}
				}
				else
				{
					key_proc(key_buf,key_index);
				}
				key_index = 0;
				break;
			default:
				key_buf[key_index++] = key_key2u8(stKey.key);
				if(key_index >= 20)
				key_index = 0;
				break;
		}
	}

	scr_mode.update = false;
}


#define MAX_MENU 80

typedef struct menu_type
{
	char * str;
	u8	   type;	
}menu_type_t;



const menu_type_t Menu[MAX_MENU]=
{
	{"1.ｼｭｳﾁｭｳｿｳﾁ(A)ID",1},				//"1.ｼｭｳﾁｭｳｿｳﾁ(A)ID"		//gEnv.id.main								used
	{"2.ｳﾝﾃﾝｼｭ(B)ID", 1},				//"2.ｳﾝﾃﾝｼｭ(B)ID"			//gEnv.id.driver							used
	{"3.ｹｲｼﾞｮｳｲﾝ(C)ID",1},				//"3.ｹｲｼﾞｮｳｲﾝ(C)ID"			//gEnv.id.security							used
	{"4.ex_driver(ex)ID", 1},			//"2.ｳﾝﾃﾝｼｭ(B)ID"			//gEnv.id.driver							used
	{"5.ex_secu(ex)ID",1},				//"3.ｹｲｼﾞｮｳｲﾝ(C)ID"			//gEnv.id.security							used
	{"6.ｳｼﾛﾄﾋﾞﾗ(D)ID",1},				//"4.ｳｼﾛﾄﾋﾞﾗ(D)ID"			//gEnv.id.b_door							used
	{"7.ﾖｺﾄﾋﾞﾗ(E)ID",1},				//"5.ﾖｺﾄﾋﾞﾗ(E)ID"			//gEnv.id.s_door							used
	{"8.ｷｬﾋﾞﾝR(F)ID",1},				//"6.ｷｬﾋﾞﾝﾐｷﾞ(F)ID"			//gEnv.id.cabin_right						used
	{"9.ｷｬﾋﾞﾝL(G)ID",1},				//"7.ｷｬﾋﾞﾝﾋﾀﾞﾘ(G)ID"		//gEnv.id.cabin_left						used
	{"10.ｳﾝﾃﾝｼｭﾘﾓｺﾝ USE",2},			//"8.ｳﾝﾃﾝｼｭﾘﾓｺﾝ"			//gEnv.use.driver							used
	{"11.ｹｲｼﾞｮｳｲﾝﾘﾓｺﾝ USE",2},			//"9.ｹｲｼﾞｮｳｲﾝﾘﾓｺﾝ"			//gEnv.use.security							used
	{"12.ex_driver(ex)_USE", 2},		//"2.ｳﾝﾃﾝｼｭ(B)ID"			//gEnv.id.driver							used
	{"13.ex_secu(ex)_USE",2},			//"3.ｹｲｼﾞｮｳｲﾝ(C)ID"			//gEnv.id.security							used
	{"14.ｳｼﾛﾄﾋﾞﾗｾｲｷﾞｮ USE",2},			//"10.ｳｼﾛﾄﾋﾞﾗｾｲｷﾞｮ"			//gEnv.use.b_door							used
	{"15.ﾖｺﾄﾋﾞﾗｾｲｷﾞｮ USE",2},			//"11.ﾖｺﾄﾋﾞﾗｾｲｷﾞｮ"			//gEnv.use.s_door							used
	{"16.ｷｬﾋﾞﾝﾐｷﾞ USE",2},				//"12.ｷｬﾋﾞﾝﾐｷﾞ"				//gEnv.use.cabin_right						used
	{"17.ｷｬﾋﾞﾝﾋﾀﾞﾘ USE",2},				//"13.ｷｬﾋﾞﾝﾋﾀﾞﾘ"			//gEnv.use.cabin_left						used
	{"18.ﾘﾓｺﾝEMG_D_ON",3},				//"14.EMG_D_ON Time"		//gEnv.remocon.emg_on						used			2----5sec
	{"19.ﾘﾓｺﾝEMG_D_OFF",3},				//"15.EMG_D_OFF Time"		//gEnv.remocon.emg_off						used			2----5sec
	{"20.ﾘﾓｺﾝButton ON",3},				//"16.Button ON Time"		//gEnv.remocon.button_on					used			2----5sec
	{"21.xｳｼﾛINﾄﾋﾞﾗ USE",2},			//gEnv.back_door_set.in_used				not used
	{"22.xｳｼﾛINﾄﾋﾞﾗ ONｼﾞｶﾝ",4},			//gEnv.back_door_set.in_on_time				not used
	{"23.xｳｼﾛINﾄﾋﾞﾗ OFFｼﾞｶﾝ",5},		//gEnv.back_door_set.in_off_time			not used
	{"24.xｳｼﾛEXﾄﾋﾞﾗ USE",2},			//gEnv.back_door_set.out_used				not used
	{"25.xｳｼﾛEXﾄﾋﾞﾗ ONｼﾞｶﾝ",4},			//gEnv.back_door_set.out_on_time			not used
	{"26.xｳｼﾛEXﾄﾋﾞﾗ OFFｼﾞｶﾝ",6},		//gEnv.back_door_set.out_off_time			not used
	{"27.xｳｼﾛ LOCKｼﾞｶﾝ",7},				//gEnv.back_door_set.lock_time				not used
	{"28.xｳｼﾛ UNLOCKｼﾞｶﾝ",7},			//gEnv.back_door_set.unlock_time			not used
	{"29.xｻｲﾄﾞINﾄﾋﾞﾗ USE",2},			//gEnv.side_door_set.in_used				not used
	{"30.xｻｲﾄﾞINﾄﾋﾞﾗ ONｼﾞｶﾝ",4},		//gEnv.side_door_set.in_on_time				not used
	{"31.xｻｲﾄﾞINﾄﾋﾞﾗ OFFｼﾞｶﾝ",5},		//gEnv.side_door_set.in_off_time			not used
	{"32.xｻｲﾄﾞEXﾄﾋﾞﾗ USE",2},			//gEnv.side_door_set.out_used				not used
	{"33.xｻｲﾄﾞEXﾄﾋﾞﾗ ONｼﾞｶﾝ",4},		//gEnv.side_door_set.out_on_time			not used
	{"34.xｻｲﾄﾞEXﾄﾋﾞﾗ OFFｼﾞｶﾝ",6},		//gEnv.side_door_set.out_off_time			not used
	{"35.xｻｲﾄﾞLOCKｼﾞｶﾝ",7},				//gEnv.side_door_set.lock_time				not used
	{"36.xｻｲﾄﾞUNLOCKｼﾞｶﾝ",7},			//gEnv.side_door_set.unlock_time			not used
	{"37.xｷｬﾋﾞﾝR EXﾄﾋﾞﾗ",2},			//gEnv.cabin_right_set.out_used				not used
	{"38.xｷｬﾋﾞﾝR EXﾄﾋﾞﾗ",4},			//gEnv.cabin_right_set.out_on_time			not used
	{"39.xｷｬﾋﾞﾝR EXﾄﾋﾞﾗ",6},			//gEnv.cabin_right_set.out_off_time			not used
	{"40.xｷｬﾋﾞﾝR INﾄﾋﾞﾗ",2},			//gEnv.cabin_right_set.in_used				not used
	{"41.xｷｬﾋﾞﾝR INﾄﾋﾞﾗ",4},			//gEnv.cabin_right_set.in_on_time			not used
	{"42.xｷｬﾋﾞﾝR INﾄﾋﾞﾗ",5},			//gEnv.cabin_right_set.in_off_time			not used
	{"43.xｷｬﾋﾞﾝR LOCKｼﾞｶﾝ",7},			//gEnv.cabin_right_set.lock_time			not used
	{"44.xｷｬﾋﾞﾝR UNLOCKｼﾞｶﾝ",7},		//gEnv.cabin_right_set.unlock_time			not used
	{"45.xｷｬﾋﾞﾝL EXﾄﾋﾞﾗ",2},			//gEnv.cabin_left_set.out_used				not used
	{"46.xｷｬﾋﾞﾝL EXﾄﾋﾞﾗ",4},			//gEnv.cabin_left_set.out_on_time			not used
	{"47.xｷｬﾋﾞﾝL EXﾄﾋﾞﾗ",6},			//gEnv.cabin_left_set.out_off_time			not used
	{"48.xｷｬﾋﾞﾝL INﾄﾋﾞﾗ",2},			//gEnv.cabin_left_set.in_used				not used
	{"49.xｷｬﾋﾞﾝL INﾄﾋﾞﾗ",4},			//gEnv.cabin_left_set.in_on_time			not used
	{"50.xｷｬﾋﾞﾝL INﾄﾋﾞﾗ",5},			//gEnv.cabin_left_set.in_off_time			not used
	{"51.xｷｬﾋﾞﾝL LOCKｼﾞｶﾝ",7},			//gEnv.cabin_left_set.lock_time				not used
	{"52.xｷｬﾋﾞﾝL UNLOCKｼﾞｶﾝ",7},		//gEnv.cabin_left_set.unlock_time			not used
	{"53.xDRVRﾄﾋﾞﾗ",2},					//gEnv.driver_set.used						not used
	{"54.xDRVRﾄﾋﾞﾗ_ON",4},				//gEnv.driver_set.on_time					not used
	{"55.DRVRﾄﾋﾞﾗ_OFF", 6},				//gEnv.driver_set.off_time					used
	{"56.DRVR LOCKｼﾞｶﾝ",5},				//gEnv.driver_set.lock_time					used
	{"57.DRVR UNLOCKｼﾞｶﾝ",5},			//gEnv.driver_set.unlock_time				used
	{"58.xSECUﾄﾋﾞﾗ",2},					//gEnv.security_set.used					not used
	{"59.xSECUﾄﾋﾞﾗ_ON",4},				//gEnv.security_set.on_time					not used
	{"60.SECUﾄﾋﾞﾗ_OFF",6},				//gEnv.security_set.off_time				used
	{"61.SECU LOCKｼﾞｶﾝ",5},				//gEnv.security_set.lock_time				used
	{"62.SECU UNLOCKｼﾞｶﾝ",5},			//gEnv.security_set.unlock_time				used
	{"63.xｸﾙﾏEMG",3},					//gEnv.main_set.emg_on_time					not used
	{"64.xｱﾝﾋﾟ",3},						//gEnv.main_set.safety_on_time				used
	{"65.ﾊｻﾞｰﾄﾞﾌﾗｯｼｭEMG.",2},			//gEnv.main_set.emg_hazard_used				used
	{"66.ﾘｾｯﾄ&ｶｲｼﾞｮｳ(b_s).",2},			//gEnv.main_set.reset_with_unlock_b_s_door_used		used
	{"67.ﾄﾞｱﾋﾞｰﾌﾟﾁｪｯｸ(b_s).",2},		//gEnv.main_set.beep_b_s_door_used			used
	{"68.ｴﾝｼﾞﾝｽﾀｰﾄ NO.",8},				//gEnv.main_set.engine_start				used
	{"69.ｾｲﾋﾞNO.",8},					//gEnv.main_set.service_code				used
	{"70.ﾃｽﾄﾓｰﾄﾞ.",9},
	{"71.ｼｮｷｶ.",10},
	{"72.ｾｲﾋﾞｶｲｼﾞｮ.",11},
	{"73.ｹｲｼﾞｮｳｶｲｼﾞｮ",12},				//"69.ｹｲｼﾞｮｳｶｲｼﾞｮ"			//gEnv.main_set.security_reset_active				used
	{"74.ｷﾝｺRV_USE",12},				//"70.ｷﾝｺRV_USE"		//gEnv.main_set.unlockRootvanKinkoWaveUsed				used
	{"75.DｷｬﾌﾞRV_USE",12},				//"71.DｷｬﾌﾞRV_USE"		//gEnv.main_set.unlockRootvanKinkoWaveUsed				used
	{"76.ﾄﾞｱﾀｲﾌﾟb_door",13},
	{"77.ﾄﾞｱﾀｲﾌﾟs_door",13},
	{"78.ﾄﾞｱﾀｲﾌﾟR_cabin",13},
	{"79.ﾄﾞｱﾀｲﾌﾟL_cabin",13},
};

















void menu_draw_menu(int index)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,0);
	lcd_prCString(Menu[index + 1].str);
	lcd_set_cursor(0,0);
	lcd_blink_enable(true);
}


u8 rf_setup_id(u32 id);

int main_menu(void * handle)
{
	u8				err;
	scr_mode_t * 	scr = (scr_mode_t *)handle;
	int				menu_cnt;
	int 			i;
	int				index = 0;
	int				envIndex = 0;
	char			result[10];
	u8				cancelFlag = false;


	menu_draw_menu(0);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 1;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_UP:
				if(--index < 0)
				{
					//index = MAX_MENU - 1;
					index = MAX_MENU - 2;
				}
				break;
			case KEY_DOWN:
				if(++index >= MAX_MENU - 1 )
				{
					index = 0;
				}
				break;
			case KEY_LEFT:
				lcd_scroll_left();
				break;
			case KEY_RIGHT:
				lcd_scroll_right();
				break;

			case KEY_SHARP:
				{
					int ret;
					u32 * pEnv = (u32 *)&gEnv;
					int initial_done = 0;
					
					if(index == 3 || index == 4)
					{
						envIndex = index + 69 - 1;
					}
					else if(index == 11 | index == 12)
					{
						envIndex = index + 63 - 1;
					}
					else
					{
						if(index > 12)
						{
							envIndex = index - 4;
						}
						else if(index > 4 && index <= 10)
						{
							
							envIndex = index - 2;
						}
						else
						{
							envIndex = index;
						}
					}
					
					
					
					switch(Menu[index].type)
					{
						case 1:
							sprintf(result,"%08lx",pEnv[envIndex]);
							ret = menu_type_1(index,result);
							if(ret == 1)
							{
								pEnv[envIndex] = strtoul(result,NULL,16);
								env_save(&gEnv);
								
								for(int i=0;i<2;i++)
								{
									#ifdef NEW_TEST_BUZZER_LIMIT
									gOs_BuzzerTimeLimit_GetTime = OSTimeGet() + OS_TICKS_PER_SEC * 5;
									#endif
									
									
									set_buzzer_drv(true);
									OSTimeDlyHMSM(0,0,0,100);
									set_buzzer_drv(false);
									OSTimeDlyHMSM(0,0,0,100);
									
									#ifdef NEW_TEST_BUZZER_LIMIT
									gOs_BuzzerTimeLimit_GetTime = 0;
									#endif
								}

							
								while(1)
								{
									OSSemPend(KeyWait, 300, &err);

									//err = 0;
									if(menu_nokey2(err) == true)
									{
										break;
									}
									else
									{
										if(err != 0)continue;
									}
									
									switch(stKey.key)
									{
										case KEY_STAR:
											//env_save(&gEnv);
											//lcd_clear();
											//radio_mode = RF_NOMAL_MODE;
											//return 1;	
											cancelFlag = true;
											//break;
									}
									
									if(cancelFlag == true)
									{
										break;
									}
									
								}
							
								if(index > 0  && index < 7 && cancelFlag == false)
								{
									// set id;
									
									//for(int i=0;i<14;i++)
									{
										if(rf_setup_id(pEnv[envIndex]) == true)
										{
											#ifdef NEW_TEST_BUZZER_LIMIT
											gOs_BuzzerTimeLimit_GetTime = OSTimeGet() + OS_TICKS_PER_SEC * 5;
											#endif
																						
											set_buzzer_drv(true);
											OSTimeDlyHMSM(0,0,0,100);
											set_buzzer_drv(false);
											OSTimeDlyHMSM(0,0,0,100);
											set_buzzer_drv(true);
											OSTimeDlyHMSM(0,0,0,100);
											set_buzzer_drv(false);
											OSTimeDlyHMSM(0,0,0,100);
											set_buzzer_drv(true);
											OSTimeDlyHMSM(0,0,0,100);
											set_buzzer_drv(false);
											OSTimeDlyHMSM(0,0,0,100);
											set_buzzer_drv(true);
											OSTimeDlyHMSM(0,0,0,100);
											set_buzzer_drv(false);
											
											#ifdef NEW_TEST_BUZZER_LIMIT
											gOs_BuzzerTimeLimit_GetTime = 0;
											#endif
											
											break;
										}
										else
										{
											#ifdef NEW_TEST_BUZZER_LIMIT
											gOs_BuzzerTimeLimit_GetTime = OSTimeGet() + OS_TICKS_PER_SEC * 5;
											#endif
											set_buzzer_drv(true);
											OSTimeDlyHMSM(0,0,1,100);
											set_buzzer_drv(false);
											
											#ifdef NEW_TEST_BUZZER_LIMIT
											gOs_BuzzerTimeLimit_GetTime = 0;
											#endif
										}									
									}
								}
								else
								{
									cancelFlag = false;
								}
							}
							break;
						case 2:
							sprintf(result,"%01d",pEnv[envIndex]);
							ret = menu_type_2(index,result);
							if(ret == 1)
							{
								pEnv[envIndex] = strtoul(result,NULL,10);
							}
							break;
						case 3:
							sprintf(result,"%01d",pEnv[envIndex]);
							ret = menu_type_3(index,result);
							if(ret == 1)
							{
								pEnv[envIndex] = strtoul(result,NULL,10);
								
							}
							break;
						case 4:
							sprintf(result,"%01d",pEnv[envIndex]);
							ret = menu_type_4(index,result);
							if(ret == 1)
							{
								pEnv[envIndex] = strtoul(result,NULL,10);
							}
							break;
						case 5:
							sprintf(result,"%01d",pEnv[envIndex]);
							ret = menu_type_5(index,result);
							if(ret == 1)
							{
								pEnv[envIndex] = strtoul(result,NULL,10);
								
							}
							break;
						case 6:
							sprintf(result,"%02d",pEnv[envIndex]);
							ret = menu_type_6(index,result);
							if(ret == 1)
							{
								pEnv[envIndex] = strtoul(result,NULL,10);
								
							}
							break;
						case 7:
							sprintf(result,"%01d",pEnv[envIndex]);
							ret = menu_type_7(index,result);
							if(ret == 1)
							{
								pEnv[envIndex] = strtoul(result,NULL,10);
								
							}
							break;
						case 8:
							sprintf(result,"%04d",pEnv[envIndex]);
							ret = menu_type_8(index,result);
							if(ret == 1)
							{
								pEnv[envIndex] = strtoul(result,NULL,10);
							}
							break;
						case 9:
							sprintf(result,"%01d",radio_mode);				//radio test mode 20150116 add
							ret = menu_type_9(index,result);
							if(ret == 1)
							{
								radio_mode = strtoul(result,NULL,10);
								
								#if 0
								if(radio_mode != RF_NOMAL_MODE)
								{
									rf_unlock();
								}
								#endif

							}
							break;
						case 10:
							sprintf(result,"%01d",initial_done);			//radio test mode 20150116 add
							ret = menu_type_10(index,result);
							if(ret == 1)
							{
								initial_done = strtoul(result,NULL,10);
							


							}
							break;
							
						case 11:
							sprintf(result,"%01d",initial_done);				//maintainance mode reset 20150908 add
							ret = menu_type_11(index,result);
							if(ret == 1)
							{
								initial_done = strtoul(result,NULL,10);

							}
							break;
						
						case 12:
							if(index == 72)
								sprintf(result,"%01d",gEnv.main_set.security_reset_active);
							else if(index == 73)
								sprintf(result,"%01d",gEnv.main_set.unlockRootvanKinkoWaveUsed);
							else if(index == 74)
								sprintf(result,"%01d",gEnv.main_set.unlockRootvanDCabWaveUsed);
							
							
							//sprintf(result,"%01d",pEnv[envIndex-2]);
							ret = menu_type_2(index,result);
							if(ret == 1)
							{
								if(index == 72)
									gEnv.main_set.security_reset_active = strtoul(result,NULL,10);
								else if(index == 73)
									gEnv.main_set.unlockRootvanKinkoWaveUsed = strtoul(result,NULL,10);
								else if(index == 74)
									gEnv.main_set.unlockRootvanDCabWaveUsed = strtoul(result,NULL,10);
								//pEnv[envIndex-2] = strtoul(result,NULL,10);
							}
							break;						
							
						case 13:
							if(index == 75)
								sprintf(result,"%01d",gEnv.door_Type.b_door);
							else if(index == 76)
								sprintf(result,"%01d",gEnv.door_Type.s_door);
							else if(index == 77)
								sprintf(result,"%01d",gEnv.door_Type.cabin_right);
							else if(index == 78)
								sprintf(result,"%01d",gEnv.door_Type.cabin_left);
						
							//sprintf(result,"%01d",pEnv[envIndex]);
							ret = menu_doorType(index,result);
							if(ret == 1)
							{
								
								if(index == 75)
									gEnv.door_Type.b_door = strtoul(result,NULL,10);
								else if(index == 76)
									gEnv.door_Type.s_door = strtoul(result,NULL,10);
								else if(index == 77)
									gEnv.door_Type.cabin_right = strtoul(result,NULL,10);
								else if(index == 78)
									gEnv.door_Type.cabin_left = strtoul(result,NULL,10);
							
								
								//pEnv[envIndex] = strtoul(result,NULL,10);
							}
							break;
							
					}
					if(ret == 1)
					{
						env_save(&gEnv);					
					}
				}
				break;
			case KEY_STAR:
				env_save(&gEnv);
				lcd_clear();
				radio_mode = RF_NOMAL_MODE;
				return 1;
		}
		menu_draw_menu(index);		
	}
}

void menu_draw_menu1(int index,int pos,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,0);
	lcd_prCString(value);
	lcd_blink_enable(true);
	lcd_set_cursor(pos,1);
}


int menu_type_1(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,8);
	value[8]= 0;

	menu_draw_menu1(cur,0,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_0	:
			case KEY_1	:
			case KEY_2	:
			case KEY_3	:
			case KEY_4	:
			case KEY_5	:
			case KEY_6	:
			case KEY_7	:
			case KEY_8	:
			case KEY_9	:
			case KEY_A	:
			case KEY_B	:
			case KEY_C	:
			case KEY_D	:
			case KEY_E	:
			case KEY_F	:
				if(index >= 8)			
				{
					index =0;
					memset(value,0,9);
				}
				value[index] =key_key2u8(stKey.key);
				index++;
				
				break;
				

			break;			
			case KEY_SHARP:
				if(index == 8)
				{
					memcpy(result,value,9);					
					
					return 1;
				}
				break;
			case KEY_STAR:
				return -1;
		}
		menu_draw_menu1(cur,index,value);		
	}
}

void menu_draw_menu2(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,3);
	if(value[0] == '0')
		lcd_prCString("シヨウ:シナイ");
	else
		lcd_prCString("シヨウ:スル");
	
	lcd_blink_enable(false);
}


int menu_type_2(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);

	menu_draw_menu2(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_UP	:
			case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:
				if(value[0] == '0')
					value[0] = '1';
				else
					value[0] = '0';
				break;
			case KEY_SHARP:
				memcpy(result,value,2);					
				return 1;
				break;
			case KEY_STAR:
				return -1;
		}
		menu_draw_menu2(cur,value);		
	}
}



void menu_draw_menu3(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,2);
	lcd_prCString("TIME:");
	lcd_prCString(value);
	lcd_blink_enable(true);
}


int menu_type_3(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);
	value[1] = 0;
	

	menu_draw_menu3(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_1	:
			case KEY_2	:
			case KEY_3	:
			case KEY_4	:
			case KEY_5	:
				value[0] = key_key2u8(stKey.key);;
				value[1] = 0;
				break;
			case KEY_SHARP:
				memcpy(result,value,2);					
				return 1;
				break;
			case KEY_STAR:
				return -1;
		}
		menu_draw_menu3(cur,value);		
	}
}


void menu_draw_menu4(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,2);
	lcd_prCString("ON Time:");
	lcd_prCString(value);
	lcd_blink_enable(true);
}


int menu_type_4(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);
	value[1] = 0;
	

	menu_draw_menu4(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_1	:
			case KEY_2	:
			case KEY_3	:
				value[0] = key_key2u8(stKey.key);;
				value[1] = 0;
				break;
			case KEY_SHARP:
				memcpy(result,value,2);					
				return 1;
				break;
			case KEY_STAR:
				return -1;
		}
		menu_draw_menu4(cur,value);		
	}
}

void menu_draw_menu5(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,2);
	lcd_prCString("OFF Time:");
	lcd_prCString(value);
	lcd_blink_enable(true);
}


int menu_type_5(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);
	value[1] = 0;
	

	menu_draw_menu5(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_1	:
			case KEY_2	:
			case KEY_3	:
			case KEY_4	:
			case KEY_5	:
				value[0] = key_key2u8(stKey.key);;
				value[1] = 0;
				break;
			case KEY_SHARP:
				memcpy(result,value,2);					
				return 1;
				break;
			case KEY_STAR:
				return -1;
		}
		menu_draw_menu5(cur,value);		
	}
}


void menu_draw_menu6(int index,int pos,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,0);
	lcd_prCString("OFF Time:");
	lcd_prCString(value);
	lcd_blink_enable(true);
	lcd_set_cursor(pos + 9,1);
}


int menu_type_6(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,3);
	value[3] = 0;

	menu_draw_menu6(cur,0,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_0	:
			case KEY_1	:
			case KEY_2	:
			case KEY_3	:
			case KEY_4	:
			case KEY_5	:
			case KEY_6	:
			case KEY_7	:
			case KEY_8	:
			case KEY_9	:
				if(index >= 2)			
				{
					index =0;
					memset(value,0,9);
				}
				value[index] =key_key2u8(stKey.key);
				index++;
				
				if(index >= 2)
				{
					int val;
					value[3] = 0;
					val = atoi(value);
					if(val > 28)
					{
						value[0] = '2';	
						value[1] = '8';	
					}
					else if(val < 1)
					{
						value[0] = '0';					
						value[1] = '1';					
					}
				}
				
				break;
			case KEY_SHARP:
				if(index == 2)
				{
					memcpy(result,value,3);					
					return 1;
				}
				break;
			case KEY_STAR:
				return -1;
		}
		menu_draw_menu6(cur,index,value);		
	}
}


void menu_draw_menu7(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,2);
	lcd_prCString("ON Time:");
	lcd_prCString(value);
	lcd_blink_enable(true);
}


int menu_type_7(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);
	value[1] = 0;
	

	menu_draw_menu7(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_1	:
			case KEY_2	:
			case KEY_3	:
			case KEY_4	:
				value[0] = key_key2u8(stKey.key);;
				value[1] = 0;
				break;
			case KEY_SHARP:
				memcpy(result,value,2);					
				return 1;
				break;
			case KEY_STAR:
				return -1;
		}
		menu_draw_menu7(cur,value);		
	}
}



void menu_draw_menu8(int index,int pos,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,0);
	lcd_prCString(value);
	lcd_blink_enable(true);
	lcd_set_cursor(pos,1);
}


int menu_type_8(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,5);
	value[5]= 0;

	menu_draw_menu8(cur,0,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_0	:
			case KEY_1	:
			case KEY_2	:
			case KEY_3	:
			case KEY_4	:
			case KEY_5	:
			case KEY_6	:
			case KEY_7	:
			case KEY_8	:
			case KEY_9	:
				if(index >= 4)			
				{
					index =0;
					memset(value,0,5);
				}
				value[index] =key_key2u8(stKey.key);
				index++;
				
				break;
			case KEY_SHARP:
				if(index == 4)
				{
					memcpy(result,value,5);					
					return 1;
				}
				break;
			case KEY_STAR:
				return -1;
		}
		menu_draw_menu8(cur,index,value);		
	}
}




void menu_draw_menu9(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,3);
	
	if(value[0] == '1')
		lcd_prCString("キャリア");
	else if(value[0] == '2')
		lcd_prCString("ヘンチョウハ");
	else if(value[0] == '3')
		lcd_prCString("fre adj");
	else if(value[0] == '4')
		lcd_prCString("ID check");
	else if(value[0] == '5')
		lcd_prCString("rssi check");
	else if(value[0] == '6')
		lcd_prCString("sound check");
	else
		lcd_prCString("no test");
				
	lcd_blink_enable(false);
}


int menu_type_9(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);

	menu_draw_menu9(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_1:
				value[0] = '1';
				break;
			case KEY_2:
				value[0] = '2';
				break;
			case KEY_3:
				value[0] = '3';
				break;
			case KEY_4:
				value[0] = '4';
				break;
			case KEY_5:
				value[0] = '5';
				break;				
			case KEY_6:
				value[0] = '6';
				break;
			case KEY_0		:
			case KEY_UP		:
			case KEY_DOWN	:
			case KEY_LEFT	:
			case KEY_RIGHT	:
				value[0] = '0';
				break;
			case KEY_SHARP:
				memcpy(result,value,2);
				if(value[0] == '0')
				{
					radio_mode = RF_NOMAL_MODE;
				}
				else if(value[0] == '1')
				{
					radio_mode = RF_TEST_CARRIER;
					menu_type_freq_adjust(index,result);
					radio_mode = RF_NOMAL_MODE;
				}
				else if(value[0] == '2')
				{
					radio_mode = RF_TEST_MODURATION;
					menu_type_freq_adjust(index,result);
					radio_mode = RF_NOMAL_MODE;
				}
				else if(value[0] == '3')
				{
					radio_mode = RF_TEST_ADJ_FREQ;
				}
				else if(value[0] == '4')
				{
					radio_mode = RF_TEST_ID_CHECK;
					menu_type_idcheck(index,result);
					radio_mode = RF_NOMAL_MODE;
				}
				else if(value[0] == '5')
				{
					radio_mode = RF_TEST_RSSI_CHECK;
					menu_type_rssicheck(index,result);
					radio_mode = RF_NOMAL_MODE;
				}
				else if(value[0] == '6')
				{
					//radio_mode = RF_TEST_RSSI_CHECK;
					menu_type_soundcheck(index,result);
					radio_mode = RF_NOMAL_MODE;
				}
				//return 1;
				break;
			case KEY_STAR:
				return -1;
		}
		menu_draw_menu9(cur,value);		
	}
}






void menu_draw_menu11(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	//lcd_prCString(Menu[index].str);
	if(gEnv.main_set.maintenance_active == true)
	{
		lcd_prCString("ｾｲﾋﾞｶｲｼﾞｮｼﾏｽｶ?");
		lcd_position(1,3);
		if(value[0] == '0')
		lcd_prCString("ｼﾅｲ");
		else
		lcd_prCString("ｽﾙ");
		
		lcd_blink_enable(false);		
	}
	else
	{
		lcd_prCString("ﾉｰﾏﾙﾓｰﾄﾞ");
		OSTimeDlyHMSM(0,0,2,0);
	}
}


int menu_type_11(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);

	menu_draw_menu11(cur,value);
	
	if(gEnv.main_set.maintenance_active == false)
	{
		return -1;
	}
	
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_UP	:
			case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:
			if(value[0] == '0')
			value[0] = '1';
			else
			value[0] = '0';
			break;
			case KEY_SHARP:
			//memcpy(result,value,2);
			if(value[0] == '1')
			{
				value[0] = '0';
				menu_type_mentainance_reset(cur,value);
			}
			return 1;
			break;
			case KEY_STAR:
			return -1;
		}
		menu_draw_menu11(cur,value);
	}
}







void menu_draw_menu_mentainance_reset(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	//lcd_prCString(Menu[index].str);
	lcd_prCString("ﾎﾝﾄｳﾆｼﾏｽｶ(@@)?");
	lcd_position(1,3);
	if(value[0] == '0')
	lcd_prCString("ｼﾅｲ");
	else
	lcd_prCString("ｽﾙ");
	
	lcd_blink_enable(false);
}


int menu_type_mentainance_reset(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];
	
	memcpy(value,result,2);


	//env_default(&gEnv);
	gEnv.main_set.maintenance_active = false;
	env_save(&gEnv);
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString("ｾｲﾋﾞｶｲｼﾞｮ done.");
	OSTimeDlyHMSM(0,0,2,0);
	event_set_state(RESET);


}







void menu_draw_menu10(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	//lcd_prCString(Menu[index].str);
	lcd_prCString("ｼｮｷｶｼﾏｽｶ?");
	lcd_position(1,3);
	if(value[0] == '0')
		lcd_prCString("ｼﾅｲ");
	else
		lcd_prCString("ｽﾙ");
	
	lcd_blink_enable(false);
}


int menu_type_10(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);

	menu_draw_menu10(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_UP	:
			case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:
			if(value[0] == '0')
			value[0] = '1';
			else
			value[0] = '0';
			break;
			case KEY_SHARP:
			//memcpy(result,value,2);
			if(value[0] == '1')
			{
				value[0] = '0';
				menu_type_initialdone(cur,value);
			}
			return 1;
			break;
			case KEY_STAR:
			return -1;
		}
		menu_draw_menu10(cur,value);
	}
}







void menu_draw_menu_initialdone(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	//lcd_prCString(Menu[index].str);
	lcd_prCString("ﾎﾝﾄｳﾆｼﾏｽｶ(@@)?");
	lcd_position(1,3);
	if(value[0] == '0')
		lcd_prCString("ｼﾅｲ");
	else
		lcd_prCString("ｽﾙ");
	
	lcd_blink_enable(false);
}


int menu_type_initialdone(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];
				
	memcpy(value,result,2);

	menu_draw_menu_initialdone(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_UP	:
			case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:
			if(value[0] == '0')
			value[0] = '1';
			else
			value[0] = '0';
			break;
			case KEY_SHARP:
			memcpy(result,value,2);
			//int test = strtoul(value,NULL,10);
			//if(test==0)
			if(value[0] == '1')
			{
					
				env_default(&gEnv);
				env_save(&gEnv);
				lcd_clear();
				lcd_position(0,0);
				lcd_prCString("ｼｮｷｶ done.");
				OSTimeDlyHMSM(0,0,2,0);
			}
			return 1;
			break;
			case KEY_STAR:
			return -1;
		}
		menu_draw_menu_initialdone(cur,value);
	}
}










void menu_draw_freq_adjust(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString("Push the buttom");
	lcd_position(1,3);
	lcd_prCString("UP or Down");

	
	if(value[0] == 'U')
	{
		gTele_env.ctl_data.adj_freq++;
		
		tele_env_save(&gTele_env);		

	}
	else if(value[0] == 'D')
	{

		gTele_env.ctl_data.adj_freq--;
					
		tele_env_save(&gTele_env);		
	}

	lcd_blink_enable(false);
}





int menu_type_freq_adjust(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);

	menu_draw_freq_adjust(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_1:
			case KEY_2:
			case KEY_3:
			case KEY_4:
			case KEY_0:
			break;
			case KEY_UP:
				value[0] = 'U';
				break;
			case KEY_DOWN	:
				value[0] = 'D';
				break;			
			break;
			case KEY_LEFT	:
			case KEY_RIGHT	:
			break;
			case KEY_SHARP:
			memcpy(result,value,2);

			return 1;
			break;
			case KEY_STAR:
				radio_mode = RF_NOMAL_MODE;
				return -1;
		}
		menu_draw_freq_adjust(cur,value);
	}
}




void menu_draw_idcheck(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString("id_check mode");
	//lcd_position(1,3);
	//lcd_prCString("UP");



	lcd_blink_enable(false);
}





int menu_type_idcheck(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);

	menu_draw_idcheck(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_1:
			case KEY_2:
			case KEY_3:
			case KEY_4:
			case KEY_0:
			case KEY_UP:
			case KEY_DOWN	:
			case KEY_LEFT	:
			case KEY_RIGHT	:
			case KEY_SHARP:
			break;
			case KEY_STAR:
				radio_mode = RF_NOMAL_MODE;
				lcd_clear();
				return -1;
		}
		menu_draw_idcheck(cur,value);
	}
}






void menu_draw_rssicheck(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString("rssi_check mode");
	//lcd_position(1,3);
	//lcd_prCString("UP");



	lcd_blink_enable(false);
}




int menu_type_rssicheck(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);

	menu_draw_rssicheck(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_1:
			case KEY_2:
			case KEY_3:
			case KEY_4:
			case KEY_0:
			case KEY_UP:
			case KEY_DOWN	:
			case KEY_LEFT	:
			case KEY_RIGHT	:
			case KEY_SHARP:
			break;
			case KEY_STAR:
			radio_mode = RF_NOMAL_MODE;
			lcd_clear();
			return -1;
		}
		menu_draw_idcheck(cur,value);
	}
}





void menu_draw_soundcheck(int index,int pos,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	//lcd_prCString(Menu[index].str);
	lcd_prCString("sound check");
	lcd_position(1,0);
	lcd_prCString("Sound_num:");
	lcd_prCString(value);
	lcd_blink_enable(true);
	//lcd_set_cursor(pos + 9,1);
	lcd_set_cursor(pos + 10,1);
}


int menu_type_soundcheck(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	//memcpy(value,result,3);
	memcpy(value,"1",3);
	value[3] = 0;

	menu_draw_soundcheck(cur,0,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_0	:
			case KEY_1	:
			case KEY_2	:
			case KEY_3	:
			case KEY_4	:
			case KEY_5	:
			case KEY_6	:
			case KEY_7	:
			case KEY_8	:
			case KEY_9	:
			if(index >= 2)
			{
				index =0;
				memset(value,0,9);
			}
			value[index] = key_key2u8(stKey.key);
			index++;
			
			if(index >= 2)
			{
				int val;
				value[3] = 0;
				val = atoi(value);
				if(val > 43)
				{
					value[0] = '4';
					value[1] = '2';
				}
				else if(val < 1)
				{
					value[0] = '0';
					value[1] = '1';
				}
			}
			
			break;
			case KEY_SHARP:
			{
				int val02;
				value[3] = 0;
				val02 = atoi(value);			
			
				SoundPlay(val02);
			
				#if 0
				if(index == 2)
				{
					memcpy(result,value,3);
					return 1;
				}
				#endif
			}
			break;
			case KEY_STAR:
				return -1;
		}
		menu_draw_soundcheck(cur,index,value);
	}
}





void menu_doorType_menu(int index,char * value)
{
	lcd_clear();
	lcd_position(0,0);
	lcd_prCString(Menu[index].str);
	lcd_position(1,2);
	lcd_prCString("Door TYPE:");
	lcd_prCString(value);
	lcd_blink_enable(true);
}


int menu_doorType(int cur,char * result)
{
	u8				err;
	int				index = 0;
	char			value[9];

	memcpy(value,result,2);
	value[1] = 0;
	

	menu_doorType_menu(cur,value);
	while(1)
	{
		OSSemPend(KeyWait, 300, &err);

		//err = 0;
		if(menu_nokey(err) == true)
		{
			return 0;
		}
		else
		{
			if(err != 0)continue;
		}


		switch(stKey.key)
		{
			case KEY_1	:
			case KEY_2	:
			value[0] = key_key2u8(stKey.key);;
			value[1] = 0;
			break;
			case KEY_SHARP:
			memcpy(result,value,2);
			return 1;
			break;
			case KEY_STAR:
			return -1;
		}
		menu_doorType_menu(cur,value);
	}
}



