
#ifndef KEY_H
#define KEY_H
#include <includes.h>

#define KEY_ST_NONE			0
#define KEY_ST_PRESS		1
#define KEY_WAIT			2

//#define KEY_NUMBER          37

//define
#define KEY_NONE            0x0000
#define KEY_0		        0x0801
#define KEY_1		        0x0100
#define KEY_2		        0x0101
#define KEY_3		        0x0102
#define	KEY_4		        0x0200
#define KEY_5		        0x0201
#define KEY_6		        0x0202
#define KEY_7		        0x0400
#define KEY_8		        0x0401
#define KEY_9		        0x0402
#define KEY_A		        0x0103
#define KEY_B		        0x0203
#define KEY_C		        0x0403
#define KEY_D		        0x0803
#define KEY_E		        0x0104
#define KEY_F		        0x0204
#define KEY_UP		        0x0404
#define KEY_DOWN	        0x0804
#define KEY_LEFT	        0x1003
#define KEY_RIGHT	        0x1004
#define KEY_STAR		    0x0800
#define KEY_SHARP		    0x0802


#define KEY_EMG				0x1004
#define KEY_SAFETY			0x1003
#define KEY_MEL				0x1000
#define KEY_SECURITY		0x1001
#define KEY_DOOR			0x1002

#define L_KEY_0		        0x8801
#define L_KEY_1		        0x8100
#define L_KEY_2		        0x8101
#define L_KEY_3		        0x8102
#define	L_KEY_4		        0x8200
#define L_KEY_5		        0x8201
#define L_KEY_6		        0x8202
#define L_KEY_7		        0x8400
#define L_KEY_8		        0x8401
#define L_KEY_9		        0x8402
#define L_KEY_A		        0x8103
#define L_KEY_B		        0x8203
#define L_KEY_C		        0x8403
#define L_KEY_D		        0x8803
#define L_KEY_E		        0x8104
#define L_KEY_F		        0x8204
#define L_KEY_UP		        0x0404
#define L_KEY_DOWN	        0x8804
#define L_KEY_LEFT	        0x9003
#define L_KEY_RIGHT	        0x9004
#define L_KEY_STAR		    0x8800
#define L_KEY_SHARP		    0x8802


#define L_KEY_EMG			0x9004
#define L_KEY_SAFETY		0x9003
#define L_KEY_MEL			0x9000
#define L_KEY_SECURITY		0x9001
#define L_KEY_DOOR			0x9002




//define struct
typedef struct _KEY_DIS_
{
	u8  name[10];
	u16 key;
}key_dis_t;

typedef struct _KEY_DATA_{
	u16	key;
	u16	time;
}KEY_DATA;

//define  extern
extern OS_EVENT    *KeyWait;
extern KEY_DATA		stKey;
extern u8 g_key_status;

#endif  //KEY_H
