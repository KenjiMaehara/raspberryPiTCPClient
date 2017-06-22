#ifndef _MACRO_H_
#define _MACRO_H_
//#include <stdu8.h>

#define						GFSK_DEF
//#define						PRG_VERSION_MAJOR		0
//#define						PRG_VERSION_SUB			4
//#define						PRG_VERSION_SUB_SUB		2
//#define						PRG_VERSION_SUB_SUB_SUB		4

#define						S3_DELAY		1
#define						DEBUG_TEST


#define NEW_TEST_BUZZER_LIMIT



//#define						KOREA

typedef unsigned char 	  u8;

typedef unsigned char 	  u8;
typedef unsigned short 	  u16;
typedef unsigned long	  u32;
typedef unsigned char 	  uint8_t;

//extern u8 status;

#define false			0
#define true			1

#define	LOW				0
#define	HIGH			1

enum
{
	RX_FIFO_NONE=0,
	RX_FIFO_MAKING,
	RX_FIFO_COMPLETE,
};


#if 0

typedef struct _ctl_data_
{
	u8 adj_freq;
	u8 freq_add_sub;
	u8 serial[3];
	u8 adj_temper;
	u8 temper_add_sub;
}ctl_data;

#endif

typedef struct _fifo_option_
{
	u8 env_data[8];
	u8 level[7];
}f_option_t;


typedef struct _txfifo_data_ 
{
	u8 length;
	u8 slave_id[6];
	u8 master_id[3];
	u8 user_data0;
	u8 user_data1;
	u8 bat_val;
	u8 master_ctl;
	u8 setup_mode;
	u8 version_l;
	u8 rssi;
	u8 lqi;
	f_option_t opt;
}fifo_t;


typedef struct _reg_slave_
{
	u8 serial[3];
	u8 slave_ctl_set;
}reg_slave;


typedef struct _reg_cmd_
{
	u8 length;
	u8 cmd[4];
}reg_cmd;


typedef struct _reg_param_
{
	u8 length;
	u8 param[4];
}reg_param;





extern u8 edge_val_gdo0;
extern u8 edge_val_gdo1;

extern u8 gdo0_status;
extern u8 gdo1_status;




typedef enum _PAYLOAD_
{
	PAYLOAD_SYNC = 0,	
	SET_STATE,
	PAYLOAD_EMG,
	PAYLOAD_DOOR_OPEN_CMD,  // BC -> A
	PAYLOAD_SET_CMD,
	PAYLOAD_REMOCON_EMG,
	PAYLOAD_ADJUST_SYNC,
	PAYLOAD_ADJUST_WRITE,
	PAYLOAD_SETUP_ID,
	PAYLOAD_DOOR_OPEN_OPER,  // A -> DEFG
	PAYLOAD_DOOR_EMG, //DEFG-> A
	PAYLOAD_DOOR_STATUS, // DEFG->A open,close
	PAYLOAD_DOOR_LOCK_STATUS, // DEFG->A lock,unlock
	PAYLOAD_GET_DOOR_STATUS, // B,C -> A door open?
	PAYLOAD_EMG_DOOR, // A -> B,C
	PAYLOAD_SYNC_VERSION,
	PAYLOAD_DRIVER_DOOR_ERR_REPORT,	//A -> B,C
	PAYLOAD_RSSI_CHECK,
	PAYLOAD_SYNC_START,
	PAYLOAD_SYNC2 = 0,// B,C -> A
};


typedef struct _PACKET_HDR_
{
	u8 length;
	u32 slave_id;
	u32 master_id;
	u8 rssi;
	u8 lqi;
	u8 payload;
}packet_hdr_t;


typedef struct _PACKET_
{
	packet_hdr_t hdr;
	u8 payload[16];
}packet_t;


// connect check payload
typedef struct _SYNC_
{
	packet_hdr_t hdr;
	u8 ver;
	//u8 version_id[3];
	//u8 ver_major;
	//u8 ver_sub;
	//u8 ver_sub_sub;
	//u8 reserve[9];	
	u8 reserve[15];
}sync_t;


// connect check and VERSION check payload
typedef struct _SYNC_VERSION_
{
	packet_hdr_t hdr;
	u8 ver;
	u8 version_id[3];
	u8 ver_major;
	u8 ver_sub;
	u8 ver_sub_sub;
	u8 reserve[9];
	//u8 reserve[15];
}sync_version_t;






// connect check payload
typedef struct _SYNC2_
{
	packet_hdr_t hdr;
	u8 ver;	
	u8 reserve[15];
}sync2_t;

/*
A-> B,C,D,E,F,G ID setup
*/
typedef struct _SETUP_
{
	packet_hdr_t hdr;
	u32 id;	
	u8 reserve[12];
}setup_t;

/*
A-> B,C,D,E, set/reset status notify
*/
typedef struct _SET_
{
	packet_hdr_t hdr;
	u8 set;
	u8 reserve[15];
}set_t;


typedef struct _EMG_
{
	packet_hdr_t hdr;
	u8 emg;
	u8 reserve[15];
	
}emg_t;



/*
A-> B,C door error notify
*/
typedef struct _EMG_DOOR_
{
	packet_hdr_t hdr;
	u8 emg;
	u8 reserve[15];
	
}emg_door_t;

/*
B,C -> A reset/set
*/
typedef struct _SET_CMD_
{
	packet_hdr_t hdr;
	u8 set;
	//u8 version_id[3];
	//u8 remocon_ver_major;
	//u8 remocon_ver_sub;
	//u8 remocon_ver_sub_sub;
	//u8 reserve[9];
	u8 reserve[15];
}set_cmd_t;

/*
B,C -> A emg
*/
typedef struct _REMOTE_EMG_
{
	packet_hdr_t hdr;
	u8 emg;
	u8 reserve[15];
}remote_emg_t;


/*
D,E,F,G -> A door_emg
PAYLOAD_DOOR_EMG
*/
typedef struct _DOOR_EMG_
{
	packet_hdr_t hdr;
	u8 door;  // 0 external, 1 internal
	u8 emg;
	u8 reserve[14];
}door_emg_t;



/*
F,G,D,E -> A door status 
PAYLOAD_DOOR_STATUS
*/
typedef struct _DOOR_STATUS_
{
	packet_hdr_t hdr;
	u8 door;  // 0 external, 1 internal
	u8 status; // 0 unlock , 1, lock
	u8 reserve[14];
}door_status_t;

/*
F,G,D,E -> A door lock status
PAYLOAD_DOOR_LOCK_STATUS
*/
typedef struct _DOOR_LOCK_STATUS_
{
	packet_hdr_t hdr;
	u8 lock;
	u8 reserve[15];
	
}door_lock_status_t;


/*
door open
PAYLOAD_DOOR_OPEN_CMD
B,C -> A
*/

typedef struct _DOOR_OPEN_
{
	packet_hdr_t hdr;
	u8 door;
	u8 open;
	u8 reserve[14];
	
}door_open_t;


/*
door open oper
PAYLOAD_DOOR_OPEN_OPER

A-> D,E,F,G
*/

typedef struct _DOOR_OPEN_OPER_
{
	packet_hdr_t hdr;
	u8 time;
	u8 reserve[15];
}door_open_oper_t;


/*
door open status
PAYLOAD_GET_DOOR_STATUS

B,C -> A door open?
*/

typedef struct _GET_DOOR_STATUS_
{
	packet_hdr_t hdr;
	u8 door;
	u8 doorlock;
	u8 reserve[14];
}get_door_open_status_t;




typedef struct _ADJUST_
{
	packet_hdr_t hdr;
	u8 bat_val;
	u16 adc_temperature;
	u8 prog_ver_sub;
	u8 prog_ver_major;
	u8 Device_type;
	u8 adj_freq;
	u8 adj_temper;
	u8 freq_add_sub;
	u8 temper_add_sub;
	u8 level[7];
	u8 rssi;
	u8 lqi;
	u8 id[3];

//	f_option_t		f_option;
}adjust_t;



enum {
	DEV_MAIN=0,
	DEV_REMO_DRIVER,
	DEV_REMO_SECURITY,
	DEV_LOCK_DOOR_BACK,
	DEV_LOCK_DOOR_SIDE,
	DEV_LOCK_CABIN_R,
	DEV_LOCK_CABIN_L
};



enum _STATE_
{
	RESET=0,
	TEMP_RESET,
	SET,
	MAINTENCE,
	DRIVING,
	POWER_SAVE,
	DEVICE_PAIRING,
};



enum _SUB_STATE_
{
	DETECTION = 0,
	WAIT_RESET,
	WAIT_RESET_TIMEOUT,
	WAIT_TIME_FOR_DETECTION,
};


#endif
