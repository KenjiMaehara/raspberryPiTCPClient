/*
 * 
 *
 * 
 * 
 */ 


#ifndef MENU_H_
#define MENU_H_




#include <mytype.h>


typedef enum __SCREEN_MODE__
{
	LIVE_SCR=0,  // time , mark
	MENU_SCR,    // Main Menu
	VERSION_SCR, // Version Display
}SCREEN_MODE;


typedef struct _SCREEN_MODE_
{
	int         screen;
	int         mode;       // 0 Normal, 1 Mel
	u8		update;
}scr_mode_t;





extern scr_mode_t	scr_mode;


extern u8			radio_mode;
extern u8			old_radio_mode;
extern u8	bOSStop;

extern u8	devicePairingNumber;

//extern u32 gOs_BuzzerTimeLimit_GetTime;

#ifdef NEW_TEST_BUZZER_LIMIT
extern u32 gOs_BuzzerTimeLimit_GetTime;
#endif

SCREEN_MODE menu_get_mode(void);

#endif /* MENU_H_ */