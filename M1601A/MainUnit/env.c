/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include "port.h"
#include "env.h"

volatile env_t gEnv ;
volatile tele_env_t gTele_env;
volatile adjust_t adjust_data;

static const unsigned char crc8_table[256] =
{
     0,  94, 188, 226,  97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
   157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,
    35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98,
   190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
    70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89,   7,
   219, 133, 103,  57, 186, 228,   6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
   101,  59, 217, 135,   4,  90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
   248, 166,  68,  26, 153, 199,  37, 123,  58, 100, 134, 216,  91,   5, 231, 185,
   140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113, 147, 205,
    17,  79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
   175, 241,  19,  77, 206, 144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238,
    50, 108, 142, 208,  83,  13, 239, 177, 240, 174,  76,  18, 145, 207,  45, 115,
   202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
    87,   9, 235, 181,  54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
   233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
   116,  42, 200, 150,  21,  75, 169, 247, 182, 232,  10,  84, 215, 137, 107,  53
};



u8 crc8( u8 *data , long size )  
{  
    long lp;
    u8 crc = 0;

	for( lp = 0; lp < size; lp++ )
	{
		 crc = crc8_table[ crc ^ data[lp] ] ;
	}
    
    return crc;
} 



void env_init()
{
	u8 crc;
	u8 * pBuf;
	
	eeprom_read_block((void*)&gEnv,(void*) 0x400,sizeof(env_t));
	
	crc = eeprom_read_byte((void *)0x400 + sizeof(env_t));
	
	
	pBuf = (u8 *)&gEnv;
	
	if(crc != crc8(pBuf,sizeof(env_t)))
	{
		env_default(&gEnv);
		eeprom_update_block((const void*)&gEnv,(void*) 0x400,sizeof(env_t));
		eeprom_write_byte((void *)0x400 + sizeof(env_t),crc8(pBuf,sizeof(env_t)));
	}
}

void env_save(env_t * env)
{
	u8 * pBuf= (u8 *)env;
	
	eeprom_update_block((const void*)env,(void*) 0x400,sizeof(env_t));
	eeprom_write_byte((void *)0x400 + sizeof(env_t),crc8(pBuf,sizeof(env_t)));
}




void env_default(env_t * env)
{
	int i;

	memset(env,0,sizeof(env_t));
	
	env->id.main = (u32)0x12345678;
	env->id.driver = (u32)0x12345678;
	env->id.security = (u32)0x12345678;
	env->id.b_door = (u32)0x12345678;
	env->id.s_door = (u32)0x12345678;
	env->id.cabin_right = (u32)0x12345678;
	env->id.cabin_left = (u32)0x12345678;
	env->ex_id.ex_driver = (u32)0x12345678;
	env->ex_id.ex_security = (u32)0x12345678;
	

	env->use.driver = (u32)1;
	env->use.security = (u32)1;
	env->use.b_door = (u32)1;
	env->use.s_door = (u32)1;
	env->use.cabin_right = (u32)0;
	env->use.cabin_left = (u32)0;
	env->ex_use.ex_driver = (u32)0;
	env->ex_use.ex_security = (u32)0;
	

	//env->remocon.emg_on = (u32)4;
	env->remocon.emg_on = (u32)2;	
	//env->remocon.emg_off = (u32)3;
	env->remocon.emg_off = (u32)2;
	//env->remocon.button_on = (u32)2;
	env->remocon.button_on = (u32)1;

	env->back_door_set.in_used = (u32)1;
	env->back_door_set.in_on_time = (u32)2;
	env->back_door_set.in_off_time = (u32)2;
	env->back_door_set.out_used = (u32)1;
	env->back_door_set.out_on_time = (u32)2;
	env->back_door_set.out_off_time = (u32)20;
	env->back_door_set.lock_time = (u32)3;
	env->back_door_set.unlock_time = (u32)3;

	env->side_door_set.in_used = (u32)1;
	env->side_door_set.in_on_time = (u32)2;
	env->side_door_set.in_off_time = (u32)2;
	env->side_door_set.out_used = (u32)1;
	env->side_door_set.out_on_time = (u32)2;
	env->side_door_set.out_off_time = (u32)20;
	env->side_door_set.lock_time = (u32)3;
	env->side_door_set.unlock_time = (u32)3;
	
	env->cabin_right_set.out_used = (u32)0;
	env->cabin_right_set.out_on_time = (u32)2;
	env->cabin_right_set.out_off_time = (u32)20;
	env->cabin_right_set.in_used = (u32)0;
	env->cabin_right_set.in_on_time = (u32)2;
	env->cabin_right_set.in_off_time = (u32)2;
	env->cabin_right_set.lock_time = (u32)3;
	env->cabin_right_set.unlock_time = (u32)3;
	
	env->cabin_left_set.out_used = (u32)0;
	env->cabin_left_set.out_on_time = (u32)2;
	env->cabin_left_set.out_off_time = (u32)20;
	env->cabin_left_set.in_used = (u32)0;
	env->cabin_left_set.in_on_time = (u32)2;
	env->cabin_left_set.in_off_time = (u32)2;
	env->cabin_left_set.lock_time = (u32)3;
	env->cabin_left_set.unlock_time = (u32)3;

	env->driver_set.used = (u32)1;	
	env->driver_set.on_time = (u32)2;
	env->driver_set.off_time = (u32)10;
	env->driver_set.lock_time = (u32)1;
	env->driver_set.unlock_time = (u32)1;

	env->security_set.used = (u32)1;
	env->security_set.on_time = (u32)2;
	env->security_set.off_time = (u32)10;
	env->security_set.lock_time = (u32)1;
	env->security_set.unlock_time = (u32)1;
	
	env->main_set.emg_on_time = (u32)2;
	env->main_set.safety_on_time = (u32)2;
	env->main_set.emg_hazard_flash_used = (u32)1;
	env->main_set.reset_with_unlock_b_s_door_used = (u32)0;
	env->main_set.beep_b_s_door_used = (u32)0;
	env->main_set.engine_start = (u32)1234;
	env->main_set.service_code = (u32)4567;
	env->main_set.maintenance_active = (u32)0;
	env->main_set.security_reset_active =(u32)1;
	env->main_set.unlockRootvanKinkoWaveUsed = (u32)0;
	env->main_set.unlockRootvanDCabWaveUsed = (u32)0;
	env->main_set.startDelayResetFlag = (u32)0;
	env->door_Type.b_door = (u32)1;
	env->door_Type.s_door = (u32)1;
	env->door_Type.cabin_right = (u32)1;
	env->door_Type.cabin_left = (u32)1;
	//env->reg[9]=(u32)0x8888;
	env->reg[99] = (u32)0x9876;
}



	
	
tele_env_default(tele_env_t	* tele_env)
{

	tele_env->ctl_data.adj_freq = 0x24;
	tele_env->ctl_data.freq_add_sub = 0x80;
	tele_env->ctl_data.adj_temper = 0x18;
	tele_env->ctl_data.temper_add_sub =0x80;
	tele_env->ctl_data.reg[0] = 0x00;
	tele_env->ctl_data.reg[1] = 0x00;
	tele_env->ctl_data.reg[2] = 0x00;
	for(int i=0;i<14;i++)
	{
		tele_env->ctl_data.eprom_level[i] = 0x00;
	}
}


void tele_env_init()
{
	u8 crc;
	u8 * pBuf;
	
	eeprom_read_block((void*)&gTele_env,(void*) 0x000,sizeof(tele_env_t));
	
	crc = eeprom_read_byte((void *)0x000 + sizeof(tele_env_t));
	
	
	pBuf = (u8 *)&gTele_env;
	
	if(crc != crc8(pBuf,sizeof(tele_env_t)))
	{
		tele_env_default(&gTele_env);
		eeprom_update_block((const void*)&gTele_env,(void*) 0x000,sizeof(tele_env_t));
		eeprom_write_byte((void *)0x000 + sizeof(tele_env_t),crc8(pBuf,sizeof(tele_env_t)));
	}
}

void tele_env_save(tele_env_t	* tele_env)
{
	u8 * pBuf= (u8 *)tele_env;
	
	eeprom_update_block((const void*)tele_env,(void*) 0x000,sizeof(tele_env_t));
	eeprom_write_byte((void *)0x000 + sizeof(tele_env_t),crc8(pBuf,sizeof(tele_env_t)));
}

