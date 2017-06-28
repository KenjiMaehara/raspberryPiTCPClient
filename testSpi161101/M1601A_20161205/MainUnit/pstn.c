/*
 * CFile1.c
 *
 * Created: 2016-11-08 18:40:01
 *  Author: kmaehara
 */ 

#include <avr/io.h>
#include <mytype.h>

#include "port.h"
#include "menu.h"
#include "Oper.h"


//#include "all_includes.h"

OS_EVENT *  pstn_lock=NULL;
u8	pstn_buf[30];
int bell_cnt = 0;
int ghandsfree=0;
u8	g_line_call = false;
u8	g_reverse=0x00;

int pstn_buf_count()
{

	return 0;
}


void pstn_realy_proc(u8 onoff)
{
	if(onoff == ON)
	{
		//LINE_CHANGE = 1;
		set_pstn_LNchange(true);
	}
	else
	{
		//LINE_CHANGE = 0;
		set_pstn_LNchange(false);
	}
}


u8 g_line_error;
u8 g_line_busy=false;
u8 g_dispatch = false;
u8 g_line_error_num=0;

int pstn_info_call(u8 mode,u8 *tel,u8 * data)
{
	int retry = 2;
	int cretry=0;
	int ret,i,j;
	u8  res;
	u8  reverse;
	u8	dispatch=false;
	u8	line_used=false;
	u8	status;
	// Line off

	g_line_call = true;
	task_lock(pstn_lock);

	if(mode == MEL1 || mode == MEL2)
		retry = 2;
	else if(mode == MEL3 || mode == MEL4)
		retry = 1;
	else
		retry = 5;


	do
	{
		status = LINE_STATUS;
		OSTimeDly(10);

		if(status != 0x03  && (g_dispatch == true || pEnv->comm.pstn.setup.force_call == true))
		{
			line_used = true;
			break;
		}
	}while(status != 0x03);


	OSTimeDly(500);

CALL_RETRY:
	if(line_used == true)
	{
		dispatch = true;		
		//LINE_CHANGE  = 1;
		set_pstn_LNchange(true);
		OSTimeDly(4000); 
		//LINE_HOLD = 0;
		set_pstn_HLD_Pdial(false);
	}

	//misc_other_proc(CON_LED,ON);
	
	//LINE_HOLD = 0;
	set_pstn_HLD_Pdial(false);
	//LINE_CHANGE = 0;
	set_pstn_LNchange(false);
	//LINE_MUTE = 0;
	//HANDS_EN = 0;
	set_pstn_ReceiveDataEnable(false);
	OSTimeDly(500);


	#if 0
	
	cmx_reset();
	cmx_read(STATUS);
	cmx_write(GENERALCONTROL,(u16)0x0180);
	OSTimeDly(100);
	cmx_write(GENERALCONTROL,(u16)0x0100);

	#endif

	//INTC.IER.BIT.IRQ6E = 1;
	for(i = 0 ; i < 4 ; i++)
	{
		if(cmx_write_pgm_tone(mode) != -1) 
			break;
 	}

	if(i == 4)
	{
		OSTimeDly(2000);
		g_line_call = false;
		task_unlock(pstn_lock);
		return -1;
	}
	//cmx_write(GENERALCONTROL,(u16)G_CTL_DATA);

	//LINE_HOLD = 1;
	set_pstn_HLD_Pdial(true);
	//LINE_CHANGE = 1;
	set_pstn_LNchange(true);
	//LINE_MUTE = 0;
	OSTimeDly(500);


	status = LINE_STATUS;

	if(status == 0x03)
	{
		//misc_other_proc(ERR_LED,ON);
		if(g_line_error == false)
		{
			g_line_error = true;
			for(i=0 ; i < 8 ; i++)
				if(pEnv->comm.pstn.dest[i].tel == tel)
				{
					g_line_error_num = i;
					//Jur_Add_Comm(i,JUR_COMM_LINE_ERROR);
				}
		}
		ret= 0;
		goto OFF_LINE;
	}


	ret = cmx_callprogress_detect();

	if(ret != 1 )
	{
		if(line_used == true)
		{
			//misc_other_proc(ERR_LED,ON);
			//LINE_HOLD = 1;
			set_pstn_HLD_Pdial(true);
			if(cretry++ < 2)
				goto CALL_RETRY;
			else
			{
				g_line_error = true;
				goto OFF_LINE;
			}
		}
	}


	//misc_other_proc(ERR_LED,OFF);

	//LINE_MUTE = 1;
	//LINE_PWR = 0; // add 06/29

	//cmx_write(RXMODE,(u16)0x0000 );

	// 0 갋쒖떊 갋쟻E� check
	if(pEnv->comm.pstn.setup.zero_call == true)
	{
		OSTimeDly(2000);
		if(pEnv->comm.pstn.setup.pb == 1)
		{	
			cmx_tx_dial_no_end(0);
			OSTimeDly(150);
			cmx_tx_dial_end();
		}
		else
		{
			cmx_tx_plus_dial(0);
		}
	}

	OSTimeDly(1000);
	reverse = LINE_STATUS;
	if(reverse == 0x03 )
	{
		// led
		//misc_other_proc(FAIL_LED,ON);
		if(g_line_error == false)
		{
			g_line_error = true;
			for(i=0 ; i < 8 ; i++)
				if(pEnv->comm.pstn.dest[i].tel == tel)
				{
					g_line_error_num = i;
					//Jur_Add_Comm(i,JUR_COMM_LINE_ERROR);
				}
		}
		ret = 0;
		goto OFF_LINE;
	}

	#if 0
	if(g_line_busy == true){
		g_line_busy = false;	
		for(i=0 ; i < 8 ; i++)
			if(pEnv->comm.pstn.dest[i].tel == tel)
				Jur_Add_Comm(i,JUR_COMM_LINE_BUSY_OK);
	}	
	#endif 

	if(g_line_error == true)
	{
		g_line_error = false;	
		//Jur_Add_Comm(g_line_error_num,JUR_COMM_LINE_ERROR_OK);
	}	


	for(i = 0 ; i < 20 ; i++)
	{
		if(tel[i]== 0x00)
		{
			if(pEnv->comm.pstn.setup.isdn_used == true)
				cmx_tx_dial(0x0c);
			 break;
		}
		if(pEnv->comm.pstn.setup.pb == 1)
			cmx_tx_dial(tel[i] - 0x30);
		else
			cmx_tx_plus_dial(tel[i] - 0x30);

	}
 
	//LINE_PWR = 0; // add 06/29
	//LINE_MUTE = 0;
	
	if(mode != MEL5 && pEnv->comm.pstn.setup.reverse == false)
		ret = cmx_check_ring_end(0x00);
	else
	{
		if(mode == MEL1 || mode == MEL2 || mode == MEL3 || mode == MEL4 || mode == MEL5)
		{
			ret = cmx_reverse_ring_end_check(reverse);
		}
		else
		{
			ret = cmx_check_ring_end_msg(reverse);
		}
	}
	printf("Ring Wait %s\n\r",ret == 1 ? "OK" : "FAIL");

	if(ret != 1)
	{
		for(i=0 ; i < 8 ; i++)
			if(pEnv->comm.pstn.dest[i].tel == tel)
				//Jur_Add_Comm(i,JUR_COMM_LINE_BUSY);
	}

	if(ret == 1)
	{
		ret = 0;
		if(mode < MEL5)
		{
			if(mode == MEL1 )
			{

				res = cmx_tone_wait_time(5000);
			}
			else
				res = 0x20;
			

			if(!(mode == MEL1) && (res & 0x20))
			{
				res = cmx_wait_pgm_tone(mode,8000);
			}
		}
		else
		{
			res = 0x20;
		}

		OSTimeDly(700);
		if(res & 0x20)
		{
			int len;

			//LINE_MUTE = 1;

			if(!(dispatch == true && mode == MEL1))
			{
				dispatch = false;	
			}

			OSTimeDly(20);
			for(j = 0 ; j < retry ; j++)
			{
				for(i = 0 ; i< MAX_PSTN_CMD_LEN ; i++)
				{
					if(data[i] == 0x00) break;
					if(dispatch == true && i == 12)
					{
						cmx_tx_data(mode,cmx_dtmf_code('5'));
						cmx_tx_data(mode,cmx_dtmf_code('6'));
					}
					cmx_tx_data(mode,cmx_dtmf_code(data[i]));
				}
			}
			//LINE_MUTE = 0;

			if(mode < MEL5)
			{
				//res = cmx_tone_wait_time(5000);
				OSTimeDly(20);

				if(mode == MEL1 )
				{
					res = cmx_tone_wait_time(5000);
				}
				else
				{
					res = cmx_wait_pgm_tone(mode,8000);
				}
			}
			else
			{
				res = 0x20;
			}

			if(res & 0x20)
			{
				ret = true;
			}
		}
	}
	g_dispatch = false;
OFF_LINE:
	//LINE_PWR = 1; // add 06/29
	//LINE_MUTE = 1;
	//LINE_HOLD = 0;
	set_pstn_HLD_Pdial(false);
	
	pstn_realy_proc(OFF);
	//misc_other_proc(CON_LED,OFF);
	OSTimeDly(2000);
	task_unlock(pstn_lock);
	g_line_call = false;
	return ret;
}


int pstn_busy_detect(int cnt)
{
	
	#if 0
	
	int i;

	if(bMessagePlay == false)
	{
		cmx_write(RXMODE,(u16)RX_MODE_DATA | cmx_recv_level | RX_CALL_PROG);
		cmx_write(GENERALCONTROL,(u16)0x0507);
	
		if(cmx_cpt_high_wait(2000) == 0) 
		{
			return 0;
		}

		for(i = 0 ; i < cnt ; i++)
		{
			if(cmx_cpt_status(0) != DTMF_BUSY)
				return 0;
		}
		return 1;
	}

	return 0;
	
	#endif
}



int current_msg_tel= -1;
#if 1
u8 pstn_message_oper(u8 * tel,u8 *audio,u8 cur)
{
	
	#if 0

	
	int cretry=0;
	int ret,i,j;
	u8  res,err;
	u8  reverse,e_revrs;
	u8	dispatch=false;
	u8	status;
	u8 	handsfree,bRing;
	u8 	Play = FALSE;
	int sharp_cnt=0;
	u8  bDetect=false;
	int cnt =0;
	u8	line_used=false;
	u8  org_ret = 0;
	u32	time;

	g_line_call = true;

	task_lock(pstn_lock);


	do
	{
		status = LINE_STATUS;
		OSTimeDly(10);

		if(status != 0x03  && (g_dispatch == true || pEnv->comm.pstn.setup.force_call == true))
		{
			line_used = true;
			break;
		}
	}while(status != 0x03);


	OSTimeDly(500);

CALL_RETRY2:
	if(line_used == true)
	{
		dispatch = true;		
		LINE_CHANGE  = 1;
		OSTimeDly(4000); 
		LINE_HOLD = 0;
	}

	misc_other_proc(CON_LED,ON);
	
	LINE_HOLD = 0;
	LINE_CHANGE = 0;
	LINE_MUTE = 0;
	HANDS_EN = 0;
	OSTimeDly(500);


	cmx_reset();
	cmx_read(STATUS);
	cmx_write(GENERALCONTROL,(u16)0x0180);
	OSTimeDly(100);
	cmx_write(GENERALCONTROL,(u16)0x0100);

	INTC.IER.BIT.IRQ6E = 1;
	cmx_write(GENERALCONTROL,(u16)G_CTL_DATA);

	LINE_HOLD = 1;
	LINE_CHANGE = 1;
	LINE_MUTE = 0;
	OSTimeDly(500);

	OSTimeDly(500);


	status = LINE_STATUS;

	if(status == 0x03)
	{
		misc_other_proc(ERR_LED,ON);
		if(g_line_error == false)
		{
			g_line_error = true;
			for(i=0 ; i < 8 ; i++)
				if(pEnv->comm.pstn.dest[i].tel == tel)
				{
					g_line_error_num = i;
					Jur_Add_Comm(i,JUR_COMM_LINE_ERROR);
				}
		}
		ret = 0;
		goto OFF_LINE2;
	}

	ret = cmx_callprogress_detect();

	if(ret != 1 )
	{
		if(line_used == true)
		{
			misc_other_proc(ERR_LED,ON);
			LINE_HOLD = 1;
			if(cretry++ < 2)
				goto CALL_RETRY2;
			else
			{
				g_line_error = true;
				goto OFF_LINE2;
			}
		}

		#if 0
		status = LINE_STATUS;

		if(status != 0x03)
		{
			misc_other_proc(FAIL_LED,ON);
			if(g_line_error == false)
			{
				g_line_error = true;
				for(i=0 ; i < 8 ; i++)
					if(pEnv->comm.pstn.dest[i].tel == tel)
					{
						g_line_error_num = i;
						Jur_Add_Comm(i,JUR_COMM_LINE_ERROR);
					}
			}
			goto OFF_LINE;
		}
		#endif 
	}


	misc_other_proc(ERR_LED,OFF);

	LINE_MUTE = 1;
	LINE_PWR = 0; // add 06/29




	// 0 발신 유무 check
	if(pEnv->comm.pstn.setup.zero_call == true)
	{
		OSTimeDly(2000);
		if(pEnv->comm.pstn.setup.pb == 1)
		{	
			cmx_tx_dial_no_end(0);
			OSTimeDly(150);
			cmx_tx_dial_end();
		}
		else
		{
			cmx_tx_plus_dial(0);
		}
	}
	OSTimeDly(1000);
	reverse = LINE_STATUS;
	if(reverse == 0x03 )
	{
		// led
		misc_other_proc(FAIL_LED,ON);
		if(g_line_error == false)
		{
			g_line_error = true;
			for(i=0 ; i < 8 ; i++)
				if(pEnv->comm.pstn.dest[i].tel == tel)
				{
					g_line_error_num = i;
					Jur_Add_Comm(i,JUR_COMM_LINE_ERROR);
				}
		}
		ret = 0;
		goto OFF_LINE2;
	}

	#if 0
	if(g_line_busy == true){
		g_line_busy = false;	
		for(i=0 ; i < 8 ; i++)
			if(pEnv->comm.pstn.dest[i].tel == tel)
				Jur_Add_Comm(i,JUR_COMM_LINE_BUSY_OK);
	}	
	#endif 

	if(g_line_error == true)
	{
		g_line_error = false;	
		Jur_Add_Comm(g_line_error_num,JUR_COMM_LINE_ERROR_OK);
	}	


	for(i = 0 ; i < 20 ; i++)
	{
		if(tel[i] == 0x00) break;
		if(pEnv->comm.pstn.setup.pb == 1)
			cmx_tx_dial(tel[i]-0x30);
		else
			cmx_tx_plus_dial(tel[i]-0x30);

	}

	// dial end

	LINE_MUTE = 0;
	LINE_PWR = 0; // add 06/29

	e_revrs = ~reverse & 0x03;
	ret = cmx_check_ring_end_msg(reverse);


	if(ret != 1)
	{
		for(i=0 ; i < 8 ; i++)
			if(pEnv->comm.pstn.dest[i].tel == tel)
				Jur_Add_Comm(i,JUR_COMM_LINE_BUSY);
	}
	current_msg_tel = cur;

	cmx_init_call_progress_init();		
	handsfree = 0;

	org_ret = ret;
	time = cur_time;
	if(ret == 1 )
	{

		OSTimeDly(330);

		if(audio != NULL)
		{
			audio_play_ext_task(audio,0,1);
		}
		else
		{
			memcpy((BYTE *)&gAudioPlayMsg[0],(BYTE *)&gAudioTelMsg[cur][0],sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);		
		}

		//cmx_init_call_progress_init();
		gAudioMessagePlaying = TRUE;	
		bMessagePlaying = FALSE;


		while(1)
		{
			if(handsfree == 0)
			{
				if(bMessagePlay == true)
				{
					cnt  = 0;
				}

				status = cmx_tone_wait_time(100) & 0x0f;

				if(status != 0x0f){
					if(status == 0x0c)
						sharp_cnt++;
					else
						sharp_cnt = 0;
				}

				if(sharp_cnt > 1 )
				{		
					printf("hans free\n\r");
					sharp_cnt = 0;
					gAudioMessagePlaying = false;
					if(audio == NULL)
					{
						memset(&gAudioTelMsg[cur][0],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);		
					}
					ghandsfree = 1;
					OSTimeDly(1000);		
					LINE_MUTE= 0; 		
					
					
					LINE_PWR = 0; // add 06/29
					
					//audio_amp_enable(AMP_INT,ON);
					INT_AMP_EN = ON;
					HANDS_EN = 1;
					handsfree = 1;
				}

				while(bMessagePlay == FALSE && cnt < 10)
				{
					if((cmx_reverse_check(e_revrs) == 1 ) && cmx_detect_call_progress_ring() == DTMF_BUSY)
					{
						ret =0;
						break;
					}
					status = cmx_tone_wait_time(100) & 0x0f;

					if(status != 0x0f){
						if(status == 0x0c)
							sharp_cnt++;
						else
							sharp_cnt = 0;
					}
					if(sharp_cnt > 0 )
					{		
						sharp_cnt = 0;
						gAudioMessagePlaying = false;
						if(audio == NULL)
						{
							memset(&gAudioTelMsg[cur][0],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);		
						}
						ghandsfree = 1;
						OSTimeDly(1000);		
						LINE_MUTE= 0; 		
						LINE_PWR = 0; // add 06/29
						//audio_amp_enable(AMP_INT,ON);
						INT_AMP_EN = ON;
						HANDS_EN = 1;
						handsfree = 1;
					}

					cnt++;
				}

				if(bMessagePlay == true) Play = true;

			}
			
			if(handsfree == 1)
			{
				LINE_PWR = 0; // add 06/29
				if(stKey.key == KEY_STAR && stKey.time > 3)
				{
					//audio_amp_enable(AMP_INT,OFF);
					goto OFF_LINE2;
				}
				else
				{
					audio_amp_enable(AMP_INT,ON);
				}
			}
			
			if((cmx_reverse_check(e_revrs) == 1  ) || cmx_detect_call_progress_ring() == DTMF_BUSY || ret == 0)
			{
				gAudioMessagePlaying = FALSE;

				if(audio == NULL)
				{
					memset(&gAudioTelMsg[cur][0],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);	
					memset((BYTE *)&gAudioPlayMsg[0],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);		
				}

				if(handsfree == 1)
				{
					audio_amp_enable(AMP_INT,OFF);
				}
				break;
			}

			if(cur_time - time > 180L)
			{
				gAudioMessagePlaying = FALSE;
				if(audio == NULL)
				{
					memset(gAudioTelMsg[cur],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);	
					memset((BYTE *)&gAudioPlayMsg[0],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);		
				}
				if(handsfree == 1)
				{
					audio_amp_enable(AMP_INT,OFF);
				}
				break;
			}
			OSTimeDly(5);		
		}
	}
	g_dispatch = false;

OFF_LINE2:
	current_msg_tel = -1;
	g_line_call = false;
	if(handsfree == 1)
	{
		audio_amp_enable(AMP_INT,OFF);
	}
	HANDS_EN = 0;
	LINE_PWR = 1; // add 06/29
	LINE_MUTE = 1;
	LINE_HOLD = 0;
	ghandsfree =0;
	pstn_realy_proc(OFF);
	misc_other_proc(CON_LED,OFF);
	//OSTimeDly(1000);
	task_unlock(pstn_lock);

	return org_ret;


	#endif

}
#else
 
u8 pstn_message_oper(u8 * tel,u8 *audio,u8 cur)
{
	#if 0

	int cretry=0;
	int ret,i,j;
	u8  res,err;
	u8  reverse,e_revrs;
	u8	dispatch=false;
	u8	status;
	u8 	handsfree,bRing;
	u8 	Play = FALSE;
	int sharp_cnt=0;
	u8  bDetect=false;
	int cnt =0;
	u8	line_used=false;
	u8  org_ret = 0;
	u32	time;

	g_line_call = true;

	task_lock(pstn_lock);


	do
	{
		status = LINE_STATUS;
		OSTimeDly(10);

		if(status != 0x03  && (g_dispatch == true || pEnv->comm.pstn.setup.force_call == true))
		{
			line_used = true;
			break;
		}
	}while(status != 0x03);


	OSTimeDly(500);

CALL_RETRY2:
	if(line_used == true)
	{
		dispatch = true;		
		LINE_CHANGE  = 1;
		OSTimeDly(4000); 
		LINE_HOLD = 0;
	}

	misc_other_proc(CON_LED,ON);
	
	LINE_HOLD = 0;
	LINE_CHANGE = 0;
	LINE_MUTE = 0;
	HANDS_EN = 0;
	OSTimeDly(500);


	cmx_reset();
	cmx_read(STATUS);
	cmx_write(GENERALCONTROL,(u16)0x0180);
	OSTimeDly(100);
	cmx_write(GENERALCONTROL,(u16)0x0100);

	INTC.IER.BIT.IRQ6E = 1;
	cmx_write(GENERALCONTROL,(u16)G_CTL_DATA);

	LINE_HOLD = 1;
	LINE_CHANGE = 1;
	LINE_MUTE = 0;
	OSTimeDly(500);

	OSTimeDly(500);


	status = LINE_STATUS;

	if(status == 0x03)
	{
		misc_other_proc(ERR_LED,ON);
		if(g_line_error == false)
		{
			g_line_error = true;
			for(i=0 ; i < 8 ; i++)
				if(pEnv->comm.pstn.dest[i].tel == tel)
				{
					g_line_error_num = i;
					Jur_Add_Comm(i,JUR_COMM_LINE_ERROR);
				}
		}
		ret = 0;
		goto OFF_LINE2;
	}

	ret = cmx_callprogress_detect();

	if(ret != 1 )
	{
		if(line_used == true)
		{
			misc_other_proc(ERR_LED,ON);
			LINE_HOLD = 1;
			if(cretry++ < 2)
				goto CALL_RETRY2;
			else
			{
				g_line_error = true;
				goto OFF_LINE2;
			}
		}

		#if 0
		status = LINE_STATUS;

		if(status != 0x03)
		{
			misc_other_proc(FAIL_LED,ON);
			if(g_line_error == false)
			{
				g_line_error = true;
				for(i=0 ; i < 8 ; i++)
					if(pEnv->comm.pstn.dest[i].tel == tel)
					{
						g_line_error_num = i;
						Jur_Add_Comm(i,JUR_COMM_LINE_ERROR);
					}
			}
			goto OFF_LINE;
		}
		#endif 
	}


	misc_other_proc(ERR_LED,OFF);

	LINE_MUTE = 1;
	LINE_PWR = 0; // add 06/29




	// 0 발신 유무 check
	if(pEnv->comm.pstn.setup.zero_call == true)
	{
		OSTimeDly(2000);
		if(pEnv->comm.pstn.setup.pb == 1)
		{	
			cmx_tx_dial_no_end(0);
			OSTimeDly(150);
			cmx_tx_dial_end();
		}
		else
		{
			cmx_tx_plus_dial(0);
		}
	}
	OSTimeDly(1000);
	reverse = LINE_STATUS;
	if(reverse == 0x03 )
	{
		// led
		misc_other_proc(FAIL_LED,ON);
		if(g_line_error == false)
		{
			g_line_error = true;
			for(i=0 ; i < 8 ; i++)
				if(pEnv->comm.pstn.dest[i].tel == tel)
				{
					g_line_error_num = i;
					Jur_Add_Comm(i,JUR_COMM_LINE_ERROR);
				}
		}
		ret = 0;
		goto OFF_LINE2;
	}

	#if 0
	if(g_line_busy == true){
		g_line_busy = false;	
		for(i=0 ; i < 8 ; i++)
			if(pEnv->comm.pstn.dest[i].tel == tel)
				Jur_Add_Comm(i,JUR_COMM_LINE_BUSY_OK);
	}	
	#endif 

	if(g_line_error == true)
	{
		g_line_error = false;	
		Jur_Add_Comm(g_line_error_num,JUR_COMM_LINE_ERROR_OK);
	}	


	for(i = 0 ; i < 20 ; i++)
	{
		if(tel[i] == 0x00) break;
		if(pEnv->comm.pstn.setup.pb == 1)
			cmx_tx_dial(tel[i]-0x30);
		else
			cmx_tx_plus_dial(tel[i]-0x30);

	}

	// dial end

	LINE_MUTE = 0;
	LINE_PWR = 0; // add 06/29

	e_revrs = ~reverse & 0x03;
	ret = cmx_check_ring_end_msg(reverse);


	if(ret != 1)
	{
		for(i=0 ; i < 8 ; i++)
			if(pEnv->comm.pstn.dest[i].tel == tel)
				Jur_Add_Comm(i,JUR_COMM_LINE_BUSY);
	}
	current_msg_tel = cur;

	cmx_init_call_progress_init();		
	handsfree = 0;

	org_ret = ret;
	time = cur_time;
	if(ret == 1 )
	{

		OSTimeDly(330);

		if(audio != NULL)
		{
			audio_play_ext_task(audio,0,1);
		}
		else
		{
			memcpy((BYTE *)&gAudioPlayMsg[0],(BYTE *)&gAudioTelMsg[cur][0],sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);		
		}

		//cmx_init_call_progress_init();
		gAudioMessagePlaying = TRUE;	
		bMessagePlaying = FALSE;


		while(1)
		{
			if(handsfree == 0)
			{
				if(bMessagePlay == true)
				{
					cnt  = 0;
				}

				status = cmx_detect_call_progress_dtmf();

				if(status & 0x80)
				{
					if(status != 0x0f){
						if(status == 0x0c)
							sharp_cnt++;
						else
							sharp_cnt = 0;
					}
				}
				if((cmx_reverse_check(e_revrs) == 1 ) && status == DTMF_BUSY)
				{
					ret =0;
					break;
				}


				if(sharp_cnt > 1 )
				{		
					printf("hans free\n\r");
					sharp_cnt = 0;
					gAudioMessagePlaying = false;
					if(audio == NULL)
					{
						memset(&gAudioTelMsg[cur][0],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);		
					}
					ghandsfree = 1;
					OSTimeDly(1000);		
					LINE_MUTE= 0; 		
					LINE_PWR = 0; // add 06/29
					audio_amp_enable(AMP_INT,ON);
					INT_AMP_EN = ON;
					HANDS_EN = 1;
					handsfree = 1;
				}

				while(bMessagePlay == FALSE /*&& cnt < 10*/)
				{
					if((cmx_reverse_check(e_revrs) == 1 ) && cmx_detect_call_progress_ring() == DTMF_BUSY)
					{
						ret =0;
						break;
					}
					status = cmx_tone_wait_time(100) & 0x0f;

					if(status != 0x0f){
						if(status == 0x0c)
							sharp_cnt++;
						else
							sharp_cnt = 0;
					}
					if(sharp_cnt > 0 )
					{		
						sharp_cnt = 0;
						gAudioMessagePlaying = false;
						if(audio == NULL)
						{
							memset(&gAudioTelMsg[cur][0],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);		
						}
						ghandsfree = 1;
						OSTimeDly(1000);		
						LINE_MUTE= 0; 		
						LINE_PWR = 0; // add 06/29
						audio_amp_enable(AMP_INT,ON);
						INT_AMP_EN = ON;
						HANDS_EN = 1;
						handsfree = 1;
					}

					cnt++;
				}

				if(bMessagePlay == true) Play = true;

			}
			
			if(handsfree == 1)
			{
				LINE_PWR = 0; // add 06/29
				if(stKey.key == KEY_STAR && stKey.time > 3)
				{
					//audio_amp_enable(AMP_INT,OFF);
					goto OFF_LINE2;
				}
				else
				{
					audio_amp_enable(AMP_INT,ON);
				}
			}
			
			if((cmx_reverse_check(e_revrs) == 1  ) || cmx_detect_call_progress_ring() == DTMF_BUSY || ret == 0)
			{
				gAudioMessagePlaying = FALSE;

				if(audio == NULL)
				{
					memset(&gAudioTelMsg[cur][0],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);	
					memset((BYTE *)&gAudioPlayMsg[0],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);		
				}

				if(handsfree == 1)
				{
					audio_amp_enable(AMP_INT,OFF);
				}
				break;
			}

			if(cur_time - time > 180L)
			{
				gAudioMessagePlaying = FALSE;
				if(audio == NULL)
				{
					memset(gAudioTelMsg[cur],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);	
					memset((BYTE *)&gAudioPlayMsg[0],0,sizeof(SAUDIO_PLAY) * MAX_TEL_AUDIO_MSG);		
				}
				if(handsfree == 1)
				{
					audio_amp_enable(AMP_INT,OFF);
				}
				break;
			}
			OSTimeDly(5);		
		}
	}
	g_dispatch = false;

OFF_LINE2:
	current_msg_tel = -1;
	g_line_call = false;
	if(handsfree == 1)
	{
		audio_amp_enable(AMP_INT,OFF);
	}
	HANDS_EN = 0;
	LINE_PWR = 1; // add 06/29
	LINE_MUTE = 1;
	LINE_HOLD = 0;
	ghandsfree =0;
	pstn_realy_proc(OFF);
	misc_other_proc(CON_LED,OFF);
	//OSTimeDly(1000);
	task_unlock(pstn_lock);

	return org_ret;
	
	#endif
}
 
#endif 
 

u32 low,high;
typedef struct _PSTN_STATE_
{
	u8	level;
	u32	val;
}pstn_state_t;

u32 bell_state_check(pstn_state_t * state,u16 level)
{
	int i=0;
	for(i=0 ;i < 2; i++)
	{
		if(state[i].level == level)
		{
			return state[i].val;
		}
	}
	return 0;
}

u8	pstn_bell_check()
{
	#if 0
	u8 bell;
	u32 time,cur;

	// low
	cur = OSTimeGet();
	while(1)
	{
		bell = LINE_BELL;
		time =  OSTimeGet() - cur;
		low = time;
		if(bell == 0)
		{
			break;
		}
		else
		{
			if(time >= 2200L /*|| time < 1950*/)
			{
				OSTimeDly(5);	
				return 0;
			}
		}
		OSTimeDly(5);	
	}


	// high
	cur = OSTimeGet();
	while(1)
	{
		bell = LINE_BELL;
		time =  OSTimeGet() - cur;
		high = time;
		if(bell == 1)
		{
			break;
		}
		else
		{
			if(time >= 1200L /*|| time < 900L*/) 
			{
				OSTimeDly(5);	
				return 0;
			}
		}
		OSTimeDly(5);	
	}

	if((high < 1200L  && high > 700L) && (low < 2200L  && low > 1600L) )
	{
	
		return true;
	}
	#else
	int i;
	int index=0;
	u16 cpt;
	pstn_state_t	state[3];
	u32 high,low,cur,max;
	
	
	
	memset((u8 *)state,0,sizeof(pstn_state_t) *3);
	cur = OSTimeGet();
	//cpt = LINE_BELL;
	cpt = get_pstn_BELL();
	state[index].level = cpt;
	state[index].val = cur;

	while(1)
	{
		//cpt = LINE_BELL;
		cpt = get_pstn_BELL();
		
		if(OSTimeGet() - state[index].val > 3000)
		{
			return false;
		}

		if(cpt != state[index].level)
		{
			state[index++].val = OSTimeGet() - state[index].val;
			state[index].level = cpt;
			state[index].val = OSTimeGet();
			if(index >= 2)
			{
				break;
			}
		}
		cur = OSTimeGet();
		OSTimeDly(10);
	}


	high = bell_state_check(state,1);
	low = bell_state_check(state,0);
//	printf("high %ld,low %ld\n\r",high,low);
	if((low < 1200L  && low > 700L) && (high < 2200L  && high > 1600L) )
	{
		return true;
	}
	else if((high < 650L  && high > 350L) && (low < 650L  && low > 350L) )
	{
		return 2;
	}



	return false;
	
	
	#endif
}



u8	pstn_bell_check_first()
{
	int i;
	int index=0;
	u16 cpt;
	pstn_state_t	state[3];
	u32 high,low,cur,max;
	
	
	
	memset((u8 *)state,0,sizeof(pstn_state_t) *3);

	cur = OSTimeGet();
	//cpt = LINE_BELL;
	cpt = get_pstn_BELL();
	state[index].level = cpt;
	state[index].val = cur;

	while(1)
	{
		//cpt = LINE_BELL;
		cpt = get_pstn_BELL();
		if(OSTimeGet() - state[index].val > 2500)
		{
			return false;
		}
		if(cpt != state[index].level)
		{
			state[index++].val = OSTimeGet() - state[index].val;
			state[index].level = cpt;
			state[index].val = OSTimeGet();
			if(index >= 2)
			{
				break;
			}
		}
		cur = OSTimeGet();
		OSTimeDly(10);
	}
	//printf("first\n\r");
	return true;
}


void code_change(BYTE code,BYTE *result)
{
	BYTE data;
	char str[4];
	
	if(code == '-') data = 13;
	else if(code == ',') data = 12;	
	else if(code == 'S') data = 51;	
	else if(code == 'L') data = 44;	
	else if(code == 'A') data = 33;	
	else if(code == 'V') data = 54;	
	else if(code == 'E') data = 37;	
	else if(code == 'M') data = 45;	
	else if(code == 'Z') data = 58;	
	else if(code == 'N') data = 46;	
		
	else data = code - 0x30 + 16;
	
	sprintf(str,"%02d",data);
	memcpy(result,str,2);
}


u8 code_changePstnAddress(BYTE code,BYTE *result)
{
	BYTE data;
	BYTE test=0;
	char str[4];
	
	data = code - 0x30 + 16;

	if(data > 45)
	{
		test = 3;
		data -= 30;
	}
	else if(data > 35)
	{
		test = 2;
		data -= 20;
	}
	else if(data > 25)
	{
		test = 1;
		data -= 10;
	}

	sprintf(str,"%02d",data);
	memcpy(result,str,2);

	return test;
}



bool pstn_connected=false;

u8 cmx_dtmf_code_char(u8 data)
{
	if(data >= 0&& data < 0x0b)
	{
		if(data == 0x0a) return 0x30;
		else return 0x30 + data;	
	} 	
	else if(data == 0x0c) return '#';
	else if(data == 0x0b) return '*';
	else if(data == 0x0d) return 'A';
	else if(data == 0x0e) return 'B';
	else if(data == 0x0f) return 'C';
	else return 0x00;
	
	return 0x00;
}

int pass_retry=0;

void line_disconnect()
{
	#if 0

	//OSTimeDly(1000);
	LINE_HOLD = 0;
	LINE_CHANGE = 0;
	pstn_connected = false;
	bell_cnt =0;
	ghandsfree = 0;
//	g_line_call = false;
	pass_retry = 0;
	HANDS_EN = 0;
	MIC_DISABLE = 0;;
	INTC.IER.BIT.IRQ6E = 0;
	misc_other_proc(CON_LED,OFF);
	OSTimeDly(4000);
	//printf("line_disconnect \n\r");
	task_unlock(pstn_lock);
	OSTimeDly(1000);
	//printf("line_disconnect2 \n\r");

	#endif

}

void pstn_msg_play(u8 * msg,u8 repeat,int len)
{
	
	#if 0

	u8 audio[6];
	memset(audio,0,6);
	memcpy(audio,msg,len);

	audio_play_ext_task(audio,repeat,1);
	gAudioMessagePlaying = true;

	#endif

}


void pstn_msg_play_num(u8 num,u8 repeat)
{
	#if 0

	u8 audio[6];
	int i;
	memset(audio,0,6);
	audio[0] = num;

	for(i= 0 ;i < 15 ; i++)
	{
		if(bMessagePlaying == false)
			break;
		OSTimeDly(150);
	}
	audio_play_ext_task(audio,repeat,1);
	gAudioMessagePlaying = true;
	
	#endif

}


void pstn_res(u8 onoff)
{
	//LINE_PWR = 0;
	//LINE_MUTE = 1;
	if(onoff == 2 )
	{
		cmx_tx_data_delay(0x00,1000);
	}
	else if(onoff == ON)
	{
		cmx_tx_data_delay(0x00,500);
		OSTimeDly(200);
		cmx_tx_data_delay(0x00,500);
		OSTimeDly(200);
		cmx_tx_data_delay(0x00,500);
		OSTimeDly(200);
	}
	else
	{
		cmx_tx_data_delay(0x00,3000);
		OSTimeDly(200);
	}
	//LINE_MUTE = 0;
}

/*
 
ret 
 2 hansfree
 4 end
 
 
*/

int pstn_rx_do(u8 * str,int len)
{
	
	#if 0

	
	u16 val;
	char value[10];
	u8	msg[6];


	if(pEnv->comm.pstn.recv.control == true || (str[0] == '5'))
	{
		memset(value,0,10);
		memcpy(value,str,len);
		val = atoi(str);
		if(val == 5)
		{
			return 2; // hans free;
		}
		else if(val == 0 && len == 2)
		{
			return 4;
		}
		else 
		{
			if((val >= 301 && val <= 308) || (val >= 311 && val <= 318))
			{
				int set;
				int area;
				int err=0;

				//pstn_res(ON);

				set = ((val%100)/10);
				area = (val%10) - 1;

				if(!(g_area.active & (0x0001 << area )))
				{
					err = 1;
				}

				if(set == 1)
				{
					if( (g_area.set | (0x0001 << area)) == g_area.set)
					{
						err = 1;
					
					}
				}
			   else
			   {
				   if( (g_area.set & (0x0001 << area)) != (0x0001 << area))
				   {
					   err = 1;
				   }
			   }

			   if(err == 0)
			   {
					msg[0] = 12 + area;
					msg[1] = 82;
					msg[2] = set == 0 ? 42 : 31;
					msg[3] = 0;
					pstn_msg_play(msg,1,3);
					mount_insert(MNT_REMOTE,set == 0 ? RESET :SET ,0x0001 << area ,NULL);
			   }
			   else
			   {
				   msg[0] = 44;
				   msg[1] = 45;
				   msg[2] = 0;
				   pstn_msg_play(msg,1,2);
			   }
			}
			else if(val == 711 || val == 712 || val == 721 ||  val == 722 )
			{
				int relay;
				int onoff;

				relay = ((val%100)/10) - 1;
				onoff = (val%10) - 1;


				msg[0] = relay == 0 ? 71 : 72;
				msg[1] = onoff == 0 ? 73 : 74;
				msg[2] = 0;

				pstn_msg_play(msg,1,2);
				if(onoff ==0)
					g_relaytime[relay]=0xff;
				else
					g_relaytime[relay]=0;

			}
			//seoul, card group 4 delete 
			else if(val >= 41000 && val <= 44999)
			{
				int num;

				num = val % 10000;
				num -= 1000;

				card_db_proc(DELETE,num,NULL);

				msg[0] = 75;
				msg[1] = 77;
				msg[2] = 0;
				pstn_msg_play(msg,1,2);
			}
			else if(val >= 81 && val <= 88)
			{
				int area;

				area = (val%10) -1;

				msg[0] = 12 + area;
				msg[1] = 84;
				if(g_area.set & (0x01 << area))
					msg[2]=85;
				else
					msg[2]=86;
				pstn_msg_play(msg,1,3);

			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
		return 0;
	}

	return 1;
	
	#endif

}

u8 * parsing_cmd[4][8];



int pstn_auto_parsing(u8 * cmd,int len)
{
	int offset = 0;
	int index = 0;
	u8 * pC;


	memset(parsing_cmd,0,sizeof(parsing_cmd));
	pC = cmd;


	while(1)
	{
		if(pC[index] == '3' || pC[index] == '7')
		{
			memcpy(&parsing_cmd[offset][0],&pC[index],3);
			offset++;
			index += 3;
		}
		else if(pC[index] == '4')
		{
			memcpy(&parsing_cmd[offset][0],&pC[index],5);
			offset++;
			index += 5;
		}
		else if(pC[index] == '8')
		{
			memcpy(&parsing_cmd[offset][0],&pC[index],2);
			offset++;
			index += 2;
		}
		else
		{
			break;
		}

		if(offset >= 4) break;
	
		if(len <= index)
		{
			break;
		}
	}

	return offset;

}

int pstn_rx_do_auto(u8 * str,int len)
{
	#if 0

	int i,cmd;
	u16 val;
	u8	msg[6];


	if(pEnv->comm.pstn.recv.control == true )
	{
		cmd = pstn_auto_parsing(str,len);

		if(cmd == 0) return 0;

		for(i = 0 ; i < cmd ; i++)
		{
			val = atoi(&parsing_cmd[i]);

			
			if((val >= 301 && val <= 308) || (val >= 311 && val <= 318))
			{
				int set;
				int area;
	
				set = ((val%100)/10);
				area = (val%10) - 1;
				mount_insert(MNT_REMOTE,set == 0 ? RESET :SET ,0x0001 << area ,NULL);
			}
			else if(val == 711 || val == 712 || val == 721 ||  val == 722 )
			{
				int relay;
				int onoff;
	
				relay = ((val%100)/10) - 1;
				onoff = (val%10) - 1;
				if(onoff == 0)
					g_relaytime[relay]=0xff;
				else
					g_relaytime[relay]=0x00;

				OSTimeDly(900);
	
			}
			else if(val >= 81 && val <= 88)
			{
				int area;
				int length;
				u8 		Data[16];

				area = (val%10) - 1;

				length = event_make_pvc_code(area,Data);
				msg_send_comm(PVC_STATUS_SIG,area ,0,0,Data,length);				
			}
			//seoul, card group 4 delete 
			else if(val >= 41000 && val <= 44999)
			{
				int num;
	
				num = val % 10000;
				num -= 1000;
	
				//card_db_proc(DELETE,num,NULL);
	
			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
		return 0;
	}

	return 1;
	
	#endif

}


/*
ret 
 0 = fail 
 1 = OK
 2 = disconnect
 
*/
u8 pstn_pass_rx_do(u8 * buf)
{
	if(memcmp(buf,pEnv->comm.pstn.recv.pass,6) == 0)
	{
		return 1;
	}
	else if(buf[0] == '0' && buf[1] == '0')
	{
		return 2;
	}

	return 0;
}
/*
ret 
-1 disconnect 
1  OK 
*/
int pstn_pass_check(int time)
{
	#if 0

	
	int pass_retry=0;
	int index =0;
	u8 detect=0x00;
	u32 stime;
	int err_cnt = 0;
	int i;

	stime = cur_time + 60;

	while(1)
	{
		if(cmx_reverse_check(g_reverse) == 1) return -1;

		detect = cmx_tone_wait_time(time/100);

		if(cur_time > stime)
		{
			return -1;
		}

		if(detect != 0x0f)
		{
			pstn_buf[index] = cmx_dtmf_code_char(detect & 0xf);

			if(pstn_buf[index] == '#')
			{
				int ret;

				#if 0
				if(index == 7)
				{
					index = 6;
					ret= pstn_pass_rx_do(&pstn_buf[1]);
				}
				else
				{
					ret= pstn_pass_rx_do(pstn_buf);
				}
				#else
				ret= pstn_pass_rx_do(pstn_buf);

				#endif

				if(ret == 1 && index == 6)
				{
					pstn_msg_play_num(33,1);
					return 1;
				}
				else if(ret == 0 || index != 6)
				{
					index =0;
					pstn_msg_play_num(44,1);
					//seoul retry fail(wait audio play)
					for(i = 0 ;i < 100 ; i++)
					{
						if( gAudioMessagePlaying == false)break;
						else
							OSTimeDly(100);
					}
					if(cmx_reverse_check(g_reverse) == 1) return -1;

					pass_retry++;
					if(pass_retry >= 3)
					{
						return -1;
					}
				}
				else
				{
					return -1;
				}
			}
			else
			{
				index++;
			}
			err_cnt = 0;
		}
		else
		{
			if(err_cnt++ >= 100)
			{
				return -1;
			}
		}
	}

	return -1;
	
	#endif

}


/*
ret 
-1 disconnect 
1  OK 
*/
int pstn_pass_one_check(int time)
{
	int pass_retry=0;
	int index =0;
	u8 detect=0x00;
	u32 stime;
	int err_cnt=0;

	stime = cur_time + 60;

	while(1)
	{
		if(cmx_reverse_check(g_reverse) == 1) return -2;

		detect = cmx_tone_wait_time(time/20);
		
		if(cur_time > stime)
		{
			return -1;
		}

		if(detect != 0x0f)
		{
			if(index > 29)
			{
				index =0;
				pstn_res(OFF);
				return -1;
			}
			pstn_buf[index] = cmx_dtmf_code_char(detect & 0xf);
			OSTimeDly(10);

			if(pstn_buf[index] == '#')
			{
				int ret = pstn_pass_rx_do(pstn_buf);

				if(ret == 1 && index == 6)
				{
					pstn_res(ON);
					return 1;
				}
				else if(ret == 0 || index != 6)
				{
					index =0;
					pstn_res(OFF);
					return -1;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				index++;
			}
			err_cnt =0;
		}
		else
		{
			if(err_cnt++ >= 20)
			{
				return -1;
			}

		}
	}

	return -1;
}


int pstn_manual_rx_do(int time)
{
	#if 0

	
	int cmd_retry=0;
	int index =0;
	u8 detect=0x00;
	int i;
	u32 stime;
	int err_cnt=0;

	stime = cur_time + 60;

	while(1)
	{
		if(cmx_reverse_check(g_reverse) == 1) return -1;

		detect = cmx_tone_wait_time(time/100);
		//printf("detect 0x%02x\n\r",detect);

		if(cur_time > stime)
		{
			return -1;
		}

		if(detect != 0x0f)
		{
			if(index > 29)
			{
				index =0;
				pstn_msg_play_num(44,1);
				//seoul retry fail(wait audio play)
				for(i = 0 ;i < 100 ; i++)
				{
					if(cmx_reverse_check(g_reverse) == 1) return -1;

					if( gAudioMessagePlaying == false)break;
					else
						OSTimeDly(100);
				}
				cmd_retry++;
				if(cmd_retry >= 3)
				{
					return -1;
				}
			}

			pstn_buf[index] = cmx_dtmf_code_char(detect & 0xf);

			if(pstn_buf[index] == '#')
			{
				int ret = pstn_rx_do(pstn_buf,index);
				
				if(ret == 1)
				{
					return 1;
				}
				else if(ret == 2)
				{
					return 2; //hansfree
				}
				else if(ret == 4)
				{
					return -1;
				}
				else 
				{
					index =0;
					pstn_msg_play_num(44,1);
					
					//seoul retry fail(wait audio play)
					for(i = 0 ;i < 100 ; i++)
					{
						if(cmx_reverse_check(g_reverse) == 1) return -1;

						if( gAudioMessagePlaying == false)break;
						else
							OSTimeDly(100);
					}
					cmd_retry++;
					if(cmd_retry >= 3)
					{
						return -1;
					}
				}
			}
			else
			{
				index++;
			}
			err_cnt = 0;
		}
		else
		{
			if(err_cnt++ >= 100)
			{
				return -1;
			}
		}
	}

	return -1;
	
	
	#endif	
	
}

int pstn_center_rx_do(int time)
{
	
	#if 0

	
	int cmd_retry=0;
	int index =0;
	u8 detect=0x00;
	u32 stime;

	stime = cur_time + 60;


	while(1)
	{
		if(cmx_reverse_check(g_reverse) == 1) return -1;

		detect = cmx_tone_wait_time(time);

		if(cur_time > stime)
		{
			return -1;
		}

		if(detect != 0x0f)
		{
			if(index > 29)
			{
				pstn_res(OFF);
				return -1;
			}

			pstn_buf[index] = cmx_dtmf_code_char(detect & 0xf);
			if(pstn_buf[index] == '#')
			{
				int ret = pstn_rx_do_auto(pstn_buf,index);

				printf("ret %d\n\r",ret);
				if(ret == 1)
				{
					pstn_res(ON);
					//return 1;
				}
				else 
				{
					pstn_res(OFF);
					return -1;
				}
			}
			else
			{
				index++;
			}
		}
		else
		{
			return -1;
		}
	}

	return -1;
	
	
	#endif
}

time_t bell_time=0L;

u8 pstn_state_no_bell()
{
	if(g_line_call == true)
	{
		OSTimeDly(1000);
		if(g_line_call == true)
		{
			return false;
		}
	}
	if(pstn_bell_check_first() == true)
	{
		task_lock(pstn_lock);
		bell_cnt = 1;
		bell_time = cur_time + 10;
		return 1;
	}
	return 0 ;
}

u8 pstn_state_bell()
{
	
	#if 0
	
	u8 bell;

	bell = pstn_bell_check();
	//printf("bell %d,%d\n\r",bell,bell_cnt);
	if(bell == true)
	{
		if(bell_cnt++ > pEnv->comm.pstn.recv.bell ) 
		{
			bell_cnt = 0;
			cmx_reset();
			cmx_read(STATUS);
			INTC.IER.BIT.IRQ6E = 1;
			cmx_write(GENERALCONTROL,(u16)G_CTL_DATA);
			misc_other_proc(CON_LED,ON);
			LINE_HOLD = 1;
			LINE_CHANGE = 1;
			MIC_DISABLE = 0;
			LINE_MUTE = 0;
			OSTimeDly(400);

			g_reverse = LINE_STATUS & 0x03;
			return 1;
		}
	}
	else if(bell == 2)
	{
		bell_cnt = 0;
	}
	else
	{
		if(cur_time >= bell_time)
		{
			return 2;
		}
	}

	return 0;
	
	#endif
	
}

u8 pstn_state_dtone_wait()
{
	int i;
	u8 detect;

	if(pEnv->comm.pstn.recv.control == false) return 0;


	detect = cmx_tone_wait_time(2000);

	if((detect & 0x0f) != 0x00)
		return 0;


	return 1;
}


int pstn_state_message()
{
	#if 0
	
	int ret;
	u8 detect;
	int cnt=0;
	u32 msg_time=0L;

	ret = 0;
	msg_time = cur_time + 45;
	pstn_msg_play(pEnv->comm.pstn.recv.msg,4,6);

	while(1)
	{
		if(cmx_reverse_check(g_reverse) == 1) return -1;

		cmx_init_call_progress_init();

		if(cur_time >= msg_time)
		{
			return -1;
		}

		detect = cmx_tone_wait_time(100);


		if(detect != 0x0f)
		{
			if((detect & 0x0f) == 0x0c)
			{
				gAudioMessagePlaying = false;
				if(pEnv->comm.pstn.recv.control == true)
				{
					ret = 1;
				}
				else
				{
					ret = -1;
				}
				break;
			}
		}
		else
		{
			if(gAudioMessagePlaying == false)
			{
				ret = -1;
				break;
			}
		}

		while(bMessagePlay == FALSE )
		{
			if(cmx_reverse_check(g_reverse) == 1) return -1;

			if(cmx_detect_call_progress() == DTMF_BUSY)
			{
				ret =-1;
				break;
			}

			detect = cmx_tone_wait_time(100);

			if(detect != 0x0f)
			{
				if((detect & 0x0f) == 0x0c)
				{
					gAudioMessagePlaying = false;
					if(pEnv->comm.pstn.recv.control == true)
					{
						ret = 1;
					}
					else
					{
						ret = -1;
					}
					break;
				}
			}
		}

		if(ret != 0)
			break;
	}
	return ret;
	
	#endif
}


void pstn_hans_free()
{
	
	#if 0
	
	u32 hans_time=0L;

	LINE_MUTE = 0;
	cmx_init_call_progress_init();
	hans_time = cur_time + 180L;

	while(1)
	{
		if(cur_time > hans_time)
		{
			break;
		}
		else
		{
			OSTimeDly(10);
//			cmx_init_call_progress_init();
		}

		if( cmx_reverse_check(g_reverse) == 1 || cmx_detect_call_progress() == DTMF_BUSY)
		{
			//audio_amp_enable(AMP_INT,OFF);
			gAudioMessagePlaying = false;
			break;
		}
	}
	
	#endif

}

u8 g_pstn_state= P_S_NO_BELL;

DECLARE_TASK_FUNC(Pstn)
{
	
	
	#if 0


	
	int i;
	int index;
	int cmd;
	int ret;
	u8 detect;
	u8 bell;
	u8 reverse;
	TPU2.TIOR.BYTE = 0x00;

    parg = parg; 
	pstn_lock 				= OSSemCreate(1);   

	while(1)
	{

		if(pEnv->comm.pstn.recv.method == 0)
		{
			OSTimeDly(100);
			continue;
		}
		else if(pEnv->comm.pstn.recv.method == 1)
		{
			if(g_area.set  == 0x00 && g_pstn_state == P_S_NO_BELL)
			{
				OSTimeDly(100);
				continue;
			}
		}
		switch(g_pstn_state)
		{
			case P_S_NO_BELL:
				ret = pstn_state_no_bell();
				if(ret == 1)
				{
					g_pstn_state = P_S_BELL;
				}
				else if(ret == 2)
				{
					g_pstn_state = P_S_LINE_BREAK;
				}
				break;
			case P_S_BELL:
				bell = pstn_state_bell();
				if(bell == true)
				{
					pstn_connected = true;
					//g_line_call = true;
					// seoul
					if(pEnv->comm.first != CONN_MSG && pEnv->comm.pstn.recv.control == true)
						pstn_res(2);

					if(cmx_reverse_check(g_reverse) == 1)
					{
						g_pstn_state = P_S_LINE_BREAK;
					}

					g_pstn_state = P_S_PASS_ON_CHECK;
				}
				else if(bell == 2)
				{
					line_disconnect();
					g_pstn_state = P_S_NO_BELL;
				}
				break;
		case P_S_DTONE_WAIT:

				// seoul
				if(pEnv->comm.first != CONN_MSG && pstn_state_dtone_wait() == true  )
				{
					g_pstn_state = P_S_PASS_ON_CHECK;
				}
				else
				{
					g_pstn_state = P_S_MESSAGE;
				}
				break;
		case P_S_PASS_ON_CHECK:

				// seoul
				ret = -1;
				if(pEnv->comm.first != CONN_MSG  )
					ret = pstn_pass_one_check(2000);

				if(ret == -1)
					g_pstn_state = P_S_MESSAGE;
				else if(ret == -2)
					g_pstn_state = P_S_LINE_BREAK;
				else
					g_pstn_state = P_S_DATA_CONTROL2;

				break;
			case P_S_PASS_CHECK:

				ret = pstn_pass_check(20000);

				if(ret == -1)
					g_pstn_state = P_S_LINE_BREAK;
				else
					g_pstn_state = P_S_DATA_CONTROL;

				break;

			case P_S_MESSAGE:
				if(cmx_reverse_check(g_reverse) == 1) g_pstn_state = P_S_LINE_BREAK;
				if(pstn_state_message() == 1)
				{
					int j=0;
					while(audio_get_ext_task_status() == 1)
					{
						OSTimeDly(100);
						j++;
						if(j > 150)
						{
							gAudioMessagePlaying = false;
							OSTimeDly(1000);
							break;
						}
						if(cmx_reverse_check(g_reverse) == 1) g_pstn_state = P_S_LINE_BREAK;
					}

					pstn_msg_play_num(70,1);

					//seoul retry
					for(j = 0 ;j < 100 ; j++)
					{
						if(cmx_reverse_check(g_reverse) == 1) g_pstn_state = P_S_LINE_BREAK;

						if( gAudioMessagePlaying == false)break;
						else
							OSTimeDly(100);
					}

					g_pstn_state = P_S_PASS_CHECK;
				}
				else
				{
					g_pstn_state = P_S_LINE_BREAK;
				}
				break;
			case P_S_DATA_CONTROL:

				ret = pstn_manual_rx_do(15000);

				if(ret == -1)
				{
					g_pstn_state = P_S_LINE_BREAK;
				}
				else if(ret == 2)
				{
					MIC_DISABLE = 1;
					//audio_amp_enable(AMP_INT,ON);
					LINE_PWR = 0;
					LINE_MUTE =0;
					g_pstn_state = P_S_HANDSFREE;
				}
				break;
			case P_S_DATA_CONTROL2:
				ret = pstn_center_rx_do(1000);
				g_pstn_state = P_S_LINE_BREAK;
				break;
			case P_S_HANDSFREE:
				ghandsfree = 1;
				pstn_hans_free();
				g_pstn_state = P_S_LINE_BREAK;
				break;
			case P_S_LINE_BREAK:
				gAudioMessagePlaying = false;

				line_disconnect();
				g_pstn_state = P_S_NO_BELL;
				break;
		}

		if(g_pstn_state != P_S_BELL && g_pstn_state != P_S_LINE_BREAK)
		{
			// line check
		}
	}	
	
	#endif
	
	
}

