/*
 * 
 *
 * 
 * 
 */ 


#ifndef PORT_H_
#define PORT_H_

#include <mytype.h>

typedef enum 
{
	OPEN,
	CLOSE,
	STOP,
	STOP2,

}DOOR_OPER;


void port_init(void);

u8 get_right_m_in1(void);
u8 get_right_m_in2(void);
u8 get_driver_door_switch(void);
u8 get_left_m_in1(void);
u8 get_left_m_in2(void);
u8 get_security_door_switch(void);
u8  get_busy();
void set_driver_door_oper(u8 open);
void set_security_door_oper(u8 open);

void set_buzzer(u8 set);
void set_d_emg_led(u8 onoff);
void set_g_emg_led(u8 onoff);
void set_d_mod_led(u8 onoff);
void set_w_mod_led(u8 onoff);
void set_t_mod_led(u8 onoff);
void set_hed_lmp_drv(u8 onoff);
void set_hzd_lmp_drv(u8 onoff);
void set_cel_motor_drv(u8 onoff);
void set_emg_relay_drv(u8 onoff);
void set_door_emg_drv(u8 onoff);
void set_buzzer_drv(u8 onoff);
void set_reardoor_lock_led(u8 onoff);
void set_re_door_led(u8 onoff);
void set_ri_door_led(u8 onoff);
void set_sidedoor_lock_led(u8 onoff);
void set_se_door_led(u8 onoff);
void set_si_door_led(u8 onoff);
void set_drive_door_led(u8 onoff);
void set_cavin_lock_led(u8 onoff);
void set_car_check(u8 onoff);
u8 get_ekyp_chek(void);

u8 get_gdoa(void);
u8 get_gdo2(void);

u8 get_freq(void);


void set_spi_ss(u8 set);
u8 get_spi_ss(void);
void set_spi_mosi(u8 set);
u8 get_miso(void);
void set_spi_sck(u8 set);

void set_backlight_led(u8 onoff);



u8 get_pstn_L_revers_pos(void);
u8 get_pstn_L_revers_nos(void);
void set_pstn_reset(u8 set);
void set_pstn_HLD_Pdial(u8 set);
void set_pstn_LNchange(u8 set);
void set_pstn_DataLatch(u8 set);
void set_pstn_ReceiveDataEnable(u8 set);
u8 get_pstn_DET_Dial_RingBack_BusyTone(void);
u8 get_pstn_DV_DataReciveValid(void);
u8 get_pstn_BELL(void);
void set_pstn_DM_SEL(u8 set);



<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
=======
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003
u8 get_ch1_Input(void);
u8 get_ch2_Input(void);
u8 get_ch3_Input(void);
u8 get_ch4_Input(void);
u8 get_ch5_Input(void);
u8 get_ch6_Input(void);
u8 get_ch7_Input(void);
u8 get_ch8_Input(void);
u8 get_ch9_Input(void);
u8 get_ch10_Input(void);
u8 get_ch11_Input(void);
u8 get_ch12_Input(void);
u8 get_ch13_Input(void);
u8 get_ch14_Input(void);
u8 get_ch15_Input(void);
u8 get_ch16_Input(void);
u8 get_ch17_Input(void);
u8 get_ch18_Input(void);
u8 get_sr_key_sw(void);
u8 get_tampa_sw(void);
u8 get_BattLow(void);



<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003
=======
>>>>>>> parent of f8ff870... test063003





#endif /* PORT_H_ */