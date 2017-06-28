/*
 * 
 *
 * 
 * 
 */ 


#ifndef ENV_H_
#define ENV_H_

#include <mytype.h>


#define						PRG_VERSION_MAJOR		0
#define						PRG_VERSION_SUB			0
#define						PRG_VERSION_SUB_SUB		0
#define						PRG_VERSION_SUB_SUB_SUB		1








#pragma pack 1

typedef struct device_id
{
	u32 main;
	u32 driver;
	u32 security;
	u32 b_door;
	u32 s_door;
	u32 cabin_right;
	u32 cabin_left;
}device_id_t;

typedef struct ex_device_id
{
	u32 ex_driver;
	u32 ex_security;
}ex_device_id_t;

typedef struct device_used
{
	u32 driver;
	u32 security;
	u32 b_door;
	u32 s_door;
	u32 cabin_right;
	u32 cabin_left;
}device_used_t;

typedef struct ex_device_used
{
	u32 ex_driver;
	u32 ex_security;
}ex_device_used_t;


typedef struct door_Type
{
	u32 b_door;
	u32 s_door;
	u32 cabin_right;
	u32 cabin_left;
}door_Type_t;



typedef struct remocon_time
{
	u32 emg_on;
	u32 emg_off;
	u32 button_on;
}remocon_time_t;

typedef struct door
{
	u32 in_used;
	u32 in_on_time;
	u32 in_off_time;
	u32 out_used;
	u32 out_on_time;
	u32 out_off_time;
	u32 lock_time;
	u32 unlock_time;
}door_t;



typedef struct cabin
{
	u32 out_used;
	u32 out_on_time;
	u32 out_off_time;
	u32 in_used;
	u32 in_on_time;
	u32 in_off_time;
	u32 lock_time;
	u32 unlock_time;
}cabin_t;

typedef struct driver_door
{
	u32 used;
	u32 on_time;
	u32 off_time;
	u32 lock_time;
	u32 unlock_time;
}driver_door_t;

typedef struct main_unit
{
	u32 emg_on_time;
	u32 safety_on_time;
	u32	emg_hazard_flash_used;
	u32	reset_with_unlock_b_s_door_used;
	u32	beep_b_s_door_used;
	u32 engine_start;
	u32 service_code;
	u32 maintenance_active;
	u32 security_reset_active;
	u32 unlockRootvanKinkoWaveUsed;
	u32 unlockRootvanDCabWaveUsed;
	u32 startDelayResetFlag;		//20160425 add
	u32 test20160317;
}main_unit_t;





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
	device_id_t id;
	device_used_t use;
	remocon_time_t remocon;
	door_t back_door_set;
	door_t side_door_set;
	cabin_t cabin_right_set;
	cabin_t cabin_left_set;
	driver_door_t driver_set;
	driver_door_t security_set;
	main_unit_t main_set;
	ex_device_id_t ex_id;
	ex_device_used_t ex_use;
	door_Type_t door_Type;
	u32 reg[70];
	//u32 reg[100];
}env_t;


#pragma unpack

extern volatile env_t gEnv;
extern volatile tele_env_t gTele_env;
extern volatile adjust_t adjust_data;

#endif /* ENV_H_ */