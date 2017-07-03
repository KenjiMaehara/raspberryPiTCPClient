/*
 * 
 *
 * 
 * 
 */ 


#ifndef ENV_H_
#define ENV_H_

#include <mytype.h>

#define						UNLOCK_VERSION_MAJOR		1
#define						UNLOCK_VERSION_SUB			1
#define						UNLOCK_VERSION_SUB_SUB		5



#pragma pack 1


typedef struct _ctl_data_
{
	//u8 adj_freq;
	int	adj_freq;
	u8 freq_add_sub;
	u8 reg[3];
	//u8 adj_temper;
	int adj_temper;
	u8 temper_add_sub;
	int eprom_level[14];
}ctl_data_t;



typedef struct _new_ctl_data_
{
	u8	fix_osccal_val;
	u8	fix_osccal_flag;
	u8	prog_ver_sub;
	u8	org_osccal_val;
	u8	prog_ver_major;
	u8	device_type;
	u8	reg[10];
}new_ctl_data_t;



typedef struct tele_id_t
{
	ctl_data_t	ctl_data;
	new_ctl_data_t new_ctl_data;
	u8 reg[50];
}tele_env_t;


typedef struct _ENV_
{
	u32 main;
	u32 id;
	u8 door_lock_time;
	u8 door_unlock_time;
	u8 door_open_time;
	u8 door_close_time;
	u8 door_wait_time;
	u8 door_delay_time;
	u8 door_error_reset;
	u8 beep_b_s_door_used;
	u8 unlockRootvanWaveUsed;
	u8 door_Type;
	u8 reg[9];
}env_t;
#pragma unpack

extern volatile env_t gEnv,gEnvDown;
extern volatile tele_env_t gTele_env;

#endif /* ENV_H_ */