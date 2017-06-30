/*
 * 
 *
 * 
 * 
 */ 


#ifndef OPER_H_
#define OPER_H_

#include <mytype.h>


#define MAX_OPER 10

enum _OPER_EM_
{
	OPER_NONE=0,
	OPER_RESET,
	OPER_SET,
	OPER_EMG,
	OPER_EMG_RESET,
	OPER_AUDIO_MINUS,
	OPER_AUDIO_PLUS,
	OPER_ENG_START,
	OPER_ENG_RESET,
	OPER_SERVICE,
	OPER_DOOR,
	OPER_RF_DOOR,
	OPER_RF_SET,
	OPER_RF_RESET,
	OPER_REMOCON_EMG,
	OPER_CHECK,
	OPER_POWERDOWN,
	OPER_VERSIONCHECK,
	OPER_SYSRESET,
	OPER_PAIRING_REMO_DRIVER,
	OPER_PARING_REMO_SECURITY,
	OPER_PAIRING_EX_REMO_DRIVER,
	OPER_PARING_EX_REMO_SECURITY,
	OPER_PARING_DOORCTL_CABIN_RIGHT,
	OPER_PARING_DOORCTL_CABIN_LEFT,
	OPER_PARING_DOORCTL_SIDE,
	OPER_PARING_DOORCTL_BACK,
	OPER_NOTUSE_REMO_DRIVER,
	OPER_NOTUSE_REMO_SECURITY,
	OPER_NOTUSE_EX_REMO_DRIVER,
	OPER_NOTUSE_EX_REMO_SECURITY,
	OPER_NOTUSE_DOORCTL_CABIN_RIGHT,
	OPER_NOTUSE_DOORCTL_CABIN_LEFT,
	OPER_NOTUSE_DOORCTL_SIDE,
	OPER_NOTUSE_DOORCTL_BACK
};

enum __RF_MODE__
{
	RF_NOMAL_MODE=0,  //nomal rx mode
	RF_TEST_CARRIER,    // TEST_carrier_out
	RF_TEST_MODURATION, // TEST_moduration_out
	RF_TEST_ADJ_FREQ,	// TEST_moduration_out
	RF_TEST_ID_CHECK,
	RF_TEST_RSSI_CHECK,
	RF_DEVICE_COMUNICATION_CHECK,
	RF_TX_MODE,
	RF_POWER_DOWN,
	RF_MENTENANCE,
	RF_PAIRING_MODE
};




enum __DRIVER_DOOR_MODE__
{
	DRVR_DOOR_IDLE,
	DRVR_READY_DOOR_OPEN,
	DRVR_WAIT_DOOR_OPEN,
	DRVR_WAIT_DOOR_OPEN_TIMEOUT,
	DRVR_READY_DOOR_CLOSE,
	DRVR_WAIT_DOOR_CLOSE,
	DRVR_CLOSE_DOOR,
	DRVR_CLOSE_DOOR_COUTION,
	DRVR_DOOR_POWERSAVE,
	DRVR_TIMEOUT_CLOSE_DOOR,
};




typedef struct _OPER_
{
	u8		oper;
	u8		used;
	u8		data;
}oper_t;

extern u8 gDoorStatus[12];
extern u8 gDoorLock[6];

extern u8 powerdown_led;
extern u8 g_state_drive_door[2];

//extern u8 audio_volume_ctl;

//OS_EVENT *      oper_wait;

extern u8 g_hzd_flash;

extern u8 reset_timeout_again;

extern u8 volume_half_ctl;
extern u8 g_driver_door_open_err_sound_stop;
extern void operation_set(void);
//extern void operation_reset(void);
extern u8 g_interrupt_buzzer;
extern u32 Os_SoundModuleOnTime;
extern u32 Os_PairingDeviceOnTime;

#endif /* OPER_H_ */