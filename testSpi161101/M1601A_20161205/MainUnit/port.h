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
u8 get_PowerBlackout(void);
u8 get_BattFull(void);
void set_Router_reset(u8 set);
void set_DTMF_MSG_Select(u8 set);
void set_ESP_Reset(u8 set);
void set_Relay1(u8 set);
void set_Relay2(u8 set);
void set_Relay3(u8 set);
void set_Relay4(u8 set);







#endif /* PORT_H_ */