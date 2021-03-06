﻿/*
 * 
 *
 * 
 * 
 */

#include <avr/io.h>
#include <avr/eeprom.h>
#include "port.h"
#include "env.h"

volatile env_t gEnv,gEnvDown;
volatile tele_env_t gTele_env;

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
	
	env->id = 0xffffffff;
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

