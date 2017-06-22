#include "lcd.h"		
#include <mytype.h>				// Private
#include <util/delay.h>

//
// Initial command & save registers
//
static unsigned char lcd_reg_func = 
            LCD_INST_FUNC
//     |    LCD_DL_8BIT
//     |    LCD_DL_4BIT
       |    LCD_N_2LINE
//     |    LCD_N_1LINE
//     |    LCD_DH_DOUBLE
       |    LCD_DH_NORMAL
//     |    LCD_IS_EXTEND
//     |    LCD_IS_NORMAL
       ;
static unsigned char lcd_reg_bias = 
            LCD_INST_BIAS
//     |    LCD_BS_4
       |    LCD_BS_5
//     |    LCD_OSC_122HZ
//     |    LCD_OSC_131HZ 
//     |    LCD_OSC_144HZ
//     |    LCD_OSC_161HZ
       |    LCD_OSC_183HZ
//     |    LCD_OSC_221HZ
//     |    LCD_OSC_274HZ
//     |    LCD_OSC_347HZ 
//     |    LCD_OSC_120HZ
//     |    LCD_OSC_133HZ
//     |    LCD_OSC_149HZ
//     |    LCD_OSC_167HZ
//     |    LCD_OSC_192HZ
//     |    LCD_OSC_227HZ
//     |    LCD_OSC_277HZ
       ;
static unsigned char lcd_reg_cont = 
          LCD_INST_CONT
     |    (LCD_C_LOW & LCD_INIT_CONT)
     ;
static unsigned char lcd_reg_pow = 
            LCD_INST_POW
       |    LCD_I_ON                // icon
//     |    LCD_I_OFF 
       |    LCD_B_ON                // booster
//     |    LCD_B_OFF 
       |    (LCD_C_HIGH & (LCD_INIT_CONT >> 4))
       ;
static unsigned char lcd_reg_foll = 
            LCD_INST_FOLL
       |    LCD_F_ON                // switch follwer circuit
//     |    LCD_F_OFF 
//     |    LCD_RAB_1               // x1.0
//     |    LCD_RAB_1_25            // x1.25
//     |    LCD_RAB_1_5             // x1.5
       |    LCD_RAB_1_8             // x1.8
//     |    LCD_RAB_2               // x2.0
//     |    LCD_RAB_2_5             // x2.5
//     |    LCD_RAB_3               // x3.0
//     |    LCD_RAB_3_75            // x3.75
    ;
static unsigned char lcd_reg_disp = 
            LCD_INST_DISP
       |    LCD_DSP_ON
       |    LCD_DSP_OFF
//     |    LCD_CSL_ON
       |    LCD_CSL_OFF
//     |    LCD_BLK_ON
//     |    LCD_BLK_OFF
       ;


static u8 _displayfunction=0x00; 
static u8 _displaycontrol=0x00; 
static u8 _displaymode=0x00; 

//
// Internal function
//
char lcd_itoc(int i) {
	return((i <= 9) ? (i+'0') : (i+'a'-10));
}

//
// API
//
void lcd_start(void) 
{
	LCD_START();
}

void lcd_stop(void) 
{
	//
	// I2C
	//
	LCD_STOP();
}

void lcd_sendCmdData(unsigned char ctl, unsigned char c) 
{
	unsigned char buf[2];

	buf[0] = ctl;			// LCD_CB_xxx
	buf[1] = c;			// Data/Command byte
	LCD_WRITE(buf, sizeof(buf));
	_delay_us(50);                 // 26.3us~
	return;
}


void lcd_control(unsigned char cmd) 
{
	lcd_sendCmdData(LCD_CB_INST, cmd);
}

void lcd_writeData(unsigned char data) 
{
	lcd_sendCmdData(LCD_CB_DATA, data);
}


void lcd_init(void) 
{
#ifdef LCD_4BIT
	lcd_reg_func |= LCD_DL_4BIT;
#else
	lcd_reg_func |= LCD_DL_8BIT;
#endif
    _delay_ms(100);
    lcd_control(lcd_reg_func | LCD_IS_EXTEND);
    lcd_control(lcd_reg_func | LCD_IS_EXTEND);
    lcd_control(lcd_reg_bias);
    lcd_control(lcd_reg_cont);
    lcd_control(lcd_reg_pow);
    lcd_control(lcd_reg_foll);
    _delay_ms(200);          // 200ms
    lcd_control(lcd_reg_disp);
    lcd_control(lcd_reg_func | LCD_IS_NORMAL);
	
	display() ;
    return;
}

// Override stdio -> usage: putchar(),puts(),printf()
//void  putch(char c){
//    lcd_writeData(c);
//    return;
//}

typedef struct MAP_CHAR
{
	char lcd;
	char multi;
	char add;
}map_char_t;

const map_char_t map_char[80]=
{
{0xb1,0x41,0x00},//0. ア
{0xb2,0x43,0x00},//1. イ
{0xb3,0x45,0x00},//2. ウ
{0xb4,0x47,0x00},//3. エ
{0xb5,0x49,0x00},//4. オ
{0xb6,0x4a,0x00},//5. カ
{0xb7,0x4c,0x00},//6. キ
{0xb8,0x4e,0x00},//7. ク
{0xb9,0x50,0x00},//8. ケ
{0xba,0x52,0x00},//9. コ
{0xb7,0x4d,0x01},//6. キ
{0xb8,0x4f,0x01},//7. ク
{0xb9,0x51,0x01},//8. ケ
{0xba,0x53,0x01},//9. コ
{0xbb,0x54,0x00},//10. サ
{0xbc,0x56,0x00},//11. シ
{0xbd,0x58,0x00},//12. ス
{0xbe,0x5a,0x00},//13. セ
{0xbf,0x5c,0x00},//14. ?
{0xbb,0x55,0x01},//10. サ
{0xbc,0x57,0x01},//11. シ
{0xbd,0x59,0x01},//12. ス
{0xbe,0x5b,0x01},//13. セ
{0xbf,0x5d,0x01},//14. ?
{0xc0,0x5e,0x00},//15. ?
{0xc1,0x60,0x00},//16. ?
{0xc2,0x63,0x00},//17. ツ
{0xc3,0x65,0x00},//18. テ
{0xc4,0x67,0x00},//19. ト
{0xc0,0x5f,0x01},//15. ?
{0xc1,0x61,0x01},//16. ?
{0xc2,0x64,0x01},//17. ツ
{0xc3,0x66,0x01},//18. テ
{0xc4,0x68,0x01},//19. ト
{0xc5,0x69,0x00},//20. ナ
{0xc6,0x6a,0x00},//21. ニ
{0xc7,0x6b,0x00},//22. ヌ
{0xc8,0x6c,0x00},//23. ネ
{0xc9,0x6d,0x00},//24. ノ
{0xca,0x6e,0x00},//25. ハ
{0xcb,0x71,0x00},//26. ヒ
{0xcc,0x74,0x00},//27. フ
{0xcd,0x77,0x00},//28. ヘ
{0xce,0x7a,0x00},//29. ホ
{0xca,0x6f,0x01},//30. バ
{0xcb,0x72,0x01},//31. ビ
{0xcc,0x75,0x01},//32. ブ
{0xcd,0x78,0x01},//33. ベ
{0xce,0x7b,0x01},//34. ?
{0xca,0x70,0x02},//35. パ
{0xcb,0x73,0x02},//36. ピ
{0xcc,0x76,0x02},//37. プ
{0xcd,0x79,0x02},//38. ペ
{0xce,0x7c,0x02},//39. ?
{0xcf,0x7d,0x00},//40. ?
{0xd0,0x7e,0x00},//41. ?
{0xd1,0x80,0x00},//42. ?
{0xd2,0x81,0x00},//43. メ
{0xd3,0x82,0x00},//44. モ
{0xd4,0x84,0x00},//45. ヤ
{0xd5,0x86,0x00},//46. ユ
{0xd6,0x88,0x00},//47. ヨ
{0xd7,0x89,0x00},//48. ラ
{0xd8,0x8a,0x00},//49. リ
{0xd9,0x8b,0x00},//50. ル
{0xda,0x8c,0x00},//51. レ
{0xdb,0x8d,0x00},//52. ロ
{0xdc,0x8f,0x00},//53. ワ
{0xdd,0x93,0x00},//54. ン
{0xa7,0x41,0x00},//55. ?
{0xa8,0x43,0x00},//56. ィ
{0xa9,0x45,0x00},//57. ゥ
{0xaa,0x47,0x00},//58. ェ
{0xab,0x49,0x00},//59. ォ
{0xac,0x83,0x00},//60. ャ
{0xad,0x85,0x00},//61. ュ
{0xae,0x87,0x00},//62. ョ
{0xaf,0x83,0x00},//63. ッ
{0xb0,0xfc,0x00},//64. -
};



char maping_char(char map)
{
	int i;
	
	for(i = 0 ; i < 80 ; i++)
	{
		if(map_char[i].multi == map)
		{
			lcd_writeData(map_char[i].lcd);		
			if(map_char[i].add != 0)	
				lcd_writeData(map_char[i].add + 0xdd);
			
			return map_char[i].lcd;
		}
	}
	
	return 0x20;
}

void lcd_prString(char *s) 
{
	volatile u8 kata = false;
	
    for(; *s != '\0'; s++) 
	{
		if(*s == 0x83 && kata == false)		
		{
			kata = true;
		}
		else
		{
			char ch;
			
			ch = *s;
			
			if(kata == true)
			{
				kata = false;
				maping_char(*s);
			}
			else
			{
				lcd_writeData(ch);
			}
		}
    }
	
    return;
}

void lcd_prCString(const char *s) 
{
	volatile u8 kata = false;
	
    for(; *s != '\0'; s++) 
	{
		if(*s == 0x83 && kata == false)		
		{
			kata = true;
		}
		else
		{
			char ch;
			
			ch = *s;
			
			if(kata == true)
			{
				kata = false;
				maping_char(*s);
			}
			else
			{
				lcd_writeData(ch);
			}
		}
    }
    return;
}

void lcd_prHexByte(unsigned char b) 
{
    char buf[1];

    buf[0] = lcd_itoc(b >> 4);
    lcd_writeData(buf[0]);
    buf[0] = lcd_itoc(b & 0xf);
    lcd_writeData(buf[0]);
    return;
}

void lcd_prHexInt(int i) 
{
    lcd_prHexByte(i >> 8);
    lcd_prHexByte(i & 0xff);
    return;
}

void  lcd_position(unsigned char bRow, unsigned char bCol)
{
    lcd_control(LCD_INST_DADR | (bRow << 6) | (bCol));
    return;
}

void  lcd_contrast(unsigned char bCont)
{
    bCont &= 0x3f;
    lcd_reg_cont = (lcd_reg_cont & ~LCD_C_LOW ) | (LCD_C_LOW & bCont);
    lcd_reg_pow  = (lcd_reg_pow  & ~LCD_C_HIGH) | (bCont >> 4);
    lcd_control(lcd_reg_func | LCD_IS_EXTEND);
    lcd_control(lcd_reg_cont);
    lcd_control(lcd_reg_pow);
    lcd_control(lcd_reg_func | LCD_IS_NORMAL);
    return;
}

void lcd_clear(void)
{
    lcd_control(LCD_DISP_CLEAR_HOME);
	
}

void  lcd_icon(unsigned char bAddr, unsigned char bFlag)
{
    lcd_control(lcd_reg_func | LCD_IS_EXTEND);
    lcd_control(LCD_INST_ICON | (bAddr & LCD_ICON_ADDR));
    lcd_writeData(bFlag);
    lcd_control(lcd_reg_func | LCD_IS_NORMAL);
    return;
}


void lcd_scroll_left(void) 
{
	lcd_control(LCD_CURSOR_SH_RIGHT | 0x08);
}

void lcd_scroll_right(void)
{
    lcd_control(LCD_CURSOR_SH_LEFT | 0x08);
}

void lcd_set_cursor(u8 col, u8 row) 
{ 
  const int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 }; 


  if ( row > 2 ) 
  { 
    row = 2-1;    // we count rows starting w/0 
  } 
   
  lcd_control(LCD_SETDDRAMADDR | (col + row_offsets[row])); 
} 


void lcd_cursor_enable(u8 enable)
{
	if(enable == true)
		cursor();
	else
		noCursor();
}

void lcd_blink_enable(u8 enable)
{
	if(enable == true)
		Blink();
	else
		noBlink();
}



// c++
void setDisplayControl(u8 setBit) 
{
  _displaycontrol |= setBit;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void resetDisplayControl(u8 resetBit) 
{
  _displaycontrol &= ~resetBit;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void setEntryMode(u8 setBit) {
  _displaymode |= setBit;
  command(LCD_ENTRYMODESET | _displaymode);
}

void resetEntryMode(u8 resetBit) {
  _displaymode &= ~resetBit;
  command(LCD_ENTRYMODESET | _displaymode);
}

void normalFunctionSet() {
  command(LCD_FUNCTIONSET | _displayfunction);
}

void extendFunctionSet() {
  command(LCD_FUNCTIONSET | _displayfunction | LCD_EX_INSTRUCTION);
}



// Turn the display on/off (quickly)
void noDisplay() 
{
  resetDisplayControl(LCD_DISPLAYON);
}

void display() 
{
  setDisplayControl(LCD_DISPLAYON);
}

// Turns the underline cursor on/off
void noCursor() 
{
  resetDisplayControl(LCD_CURSORON);
}
void cursor() 
{
  setDisplayControl(LCD_CURSORON);
}

// Turn on and off the blinking cursor
void noBlink() 
{
  resetDisplayControl(LCD_BLINKON);
}

void Blink() 
{
  setDisplayControl(LCD_BLINKON);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(void) 
{
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight(void) 
{
  setEntryMode(LCD_ENTRYLEFT);
}

// This is for text that flows Right to Left
void rightToLeft(void) 
{
  resetEntryMode(LCD_ENTRYLEFT);
}

// This will 'right justify' text from the cursor
void autoscroll(void) 
{
  setEntryMode(LCD_ENTRYSHIFTINCREMENT);
}

// This will 'left justify' text from the cursor
void noAutoscroll(void) 
{
  resetEntryMode(LCD_ENTRYSHIFTINCREMENT);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void createChar(u8 location, u8 charmap[]) 
{
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) 
	{
		lcd_write(charmap[i]);
	}
}

/*********** mid level commands, for sending data/cmds */

void command(u8 value) 
{
	lcd_sendCmdData(0,value);		
}

void lcd_write(u8 value) 
{
	lcd_sendCmdData(0x40,value);		
}

