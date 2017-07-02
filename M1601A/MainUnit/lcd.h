
#ifndef LCD_H
#define LCD_H


#define   LCD_ADDR	0x7c 	// LCD slave address
//-------------------------------------------------------
#include "TWI_I2CMaster.h"
#define LCD_START()		TWI_I2CMasterStart()
#define LCD_STOP()		TWI_I2CStop()
#define LCD_WRITE(b,c)	TWI_I2CWrite(LCD_ADDR, b, c, 0)

// standard / compatible
void lcd_start(void);
void lcd_stop(void);
void lcd_init(void);
void lcd_position(u8 bRow, u8 bCol);
void lcd_control(u8 c);
void lcd_writeData(u8 d);
void lcd_prString(char * sRamString);
void lcd_prCString(const char * sRomString);
void lcd_prHexByte(u8 bValue);
void lcd_prHexInt(int iValue);

// special
void lcd_contrast(u8 bCont);
void lcd_icon(u8 bAddr, u8 bFlag);
void lcd_sendCmdData(u8 cmd, u8 data);


//
// Control command
//
// PSoC Designer LCD module compatible define
#define LCD_DISP_ON                       0x0C
#define LCD_DISP_OFF                      0x08
#define LCD_DISP_BLANK                    0x0A
#define LCD_DISP_CLEAR_HOME               0x01
#define LCD_CURSOR_ON                     0x0E
#define LCD_CURSOR_OFF                    0x0C
#define LCD_CURSOR_WINK                   0x0D
#define LCD_CURSOR_BLINK                  0x0F
#define LCD_CURSOR_SH_LEFT                0x10
#define LCD_CURSOR_SH_RIGHT               0x14
#define LCD_CURSOR_HOME                   0x02
#define LCD_CURSOR_LEFT                   0x04
#define LCD_CURSOR_RIGHT                  0x06
#define LCD_PORT_MASK                     0x7F
#define LCD_SETDDRAMADDR				  0x80 

// ST7032i I2C stream
#define LCD_CB_CONT   0x80          // I2C Control byte -> Continue stream
#define LCD_CB_INST   0x00          // I2C Control byte -> next byte is instruction
#define LCD_CB_DATA   0x40          // I2C Control byte -> next byte is data

// Instruction: "Function Set"
#define LCD_INST_FUNC 0x20          // ID
#define LCD_DL_8BIT   0x10          // DL: Interface data length
#define LCD_DL_4BIT   0x00          //
#define LCD_N_2LINE   0x08          // N: Display line number  1=2-line 0=1-line
#define LCD_N_1LINE   0x00          //
#define LCD_DH_DOUBLE 0x04          // DH: Double height font type
#define LCD_DH_NORMAL 0x00          //
#define LCD_IS_EXTEND 0x01          // IS: normal/extension instruction select
#define LCD_IS_NORMAL 0x00          //

//    Instruction: <extention mode> "Bias selection / Internal OSC frequency adjust"
#define LCD_INST_BIAS 0x10          // ID
#define LCD_BS_4      0x08          // BS: bias selection  1/4
#define LCD_BS_5      0x00          //                     1/5
#define LCD_OSC_122HZ 0x00          // Internal OSC frequency adjust
#define LCD_OSC_131HZ 0x01          // 
#define LCD_OSC_144HZ 0x02          // 
#define LCD_OSC_161HZ 0x03          // 
#define LCD_OSC_183HZ 0x04          // 
#define LCD_OSC_221HZ 0x05          // 
#define LCD_OSC_274HZ 0x06          // 
#define LCD_OSC_347HZ 0x07          // 
#define LCD_OSC_120HZ 0x00          // Vdd=5.0V
#define LCD_OSC_133HZ 0x01          // Vdd=5.0V
#define LCD_OSC_149HZ 0x02          // Vdd=5.0V
#define LCD_OSC_167HZ 0x03          // Vdd=5.0V
#define LCD_OSC_192HZ 0x04          // Vdd=5.0V
#define LCD_OSC_227HZ 0x05          // Vdd=5.0V
#define LCD_OSC_277HZ 0x06          // Vdd=5.0V

//    Instruction: <extention mode> "Set ICON address"
#define LCD_INST_ICON 0x40          // ID
#define LCD_ICON_ADDR 0x0f          // adress mask bit, next data is icon patturn bit

//    Instruction: <extention mode> "Power/ICON control/Contrast set"
#define LCD_INST_POW  0x50          // ID
#define LCD_I_ON      0x08          // Ion: set ICON display
#define LCD_I_OFF     0x00          // 
#define LCD_B_ON      0x04          // Bon: switch booster circuit
#define LCD_B_OFF     0x00          // 
#define LCD_C_HIGH    0x03          // C5~4: Contrast value high nibble

//    Instruction: <extention mode> "Follower control"
#define LCD_INST_FOLL 0x60          // ID
#define LCD_F_ON      0x08          // switch follwer circuit
#define LCD_F_OFF     0x00          // 
#define LCD_RAB       0x07          // Rab2-0: V0 generator amplified ratio
#define LCD_RAB_1     0x00          // x1.0
#define LCD_RAB_1_25  0x01          // x1.25
#define LCD_RAB_1_5   0x02          // x1.5
#define LCD_RAB_1_8   0x03          // x1.8
#define LCD_RAB_2     0x04          // x2.0
#define LCD_RAB_2_5   0x05          // x2.5
#define LCD_RAB_3     0x06          // x3.0
#define LCD_RAB_3_75  0x07          // x3.75

//    Instruction: <extention mode> "Contrast set"
#define LCD_INST_CONT 0x70          // ID
#define LCD_C_LOW     0x0f          // C3~0: Contrast value low nibble

//    Instruction: "Display ON/OFF"
#define LCD_INST_DISP 0x08          // ID
#define LCD_DSP_ON    0x04          // Diaplay ON/OFF control
#define LCD_DSP_OFF   0x00          // 
#define LCD_CSL_ON    0x02          // Cursor ON/OFF control
#define LCD_CSL_OFF   0x00          // 
#define LCD_BLK_ON    0x01          // Cursor blink ON/OFF control
#define LCD_BLK_OFF   0x00          // 

//    Instruction: "Set DDRAM address"
#define LCD_INST_DADR 0x80          // ID
#define LCD_POS_LINE1 0x00          // col=0, line=0
#define LCD_POS_LINE2 0x40          // col=0, line=1

//
// Icon symbol
//
#define LCD_ICON_ANTENNA       0x00
#define LCD_ICON_TELL          0x02
#define LCD_ICON_SIGNAL        0x04
#define LCD_ICON_ARROW         0x06
#define LCD_ICON_UPDOWN        0x07
#define LCD_ICON_LOCK          0x09
#define LCD_ICON_NG            0x0b
#define LCD_ICON_BATTERY       0x0d
#define LCD_ICON_TIMER         0x0f

#define LCD_ICON_FLAG_OFF      0x00
#define LCD_ICON_FLAG_ON       0x1f
#define LCD_ICON_FLAG_ON1      0x10
#define LCD_ICON_FLAG_ON2      0x08
#define LCD_ICON_FLAG_ON3      0x04
#define LCD_ICON_FLAG_ON4      0x02
#define LCD_ICON_FLAG_ON5      0x01
#define LCD_ICON_UPDOWN_UP     0x10
#define LCD_ICON_UPDOWN_DOWN   0x08
#define LCD_ICON_BATTERY_EMPTY 0x02
#define LCD_ICON_BATTERY_LOW   0x12
#define LCD_ICON_BATTERY_MOD   0x0a
#define LCD_ICON_BATTERY_HIGH  0x1a
#define LCD_ICON_BATTERY_FULL  0x1e


// commands
#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80

#define LCD_EX_SETBIASOSC       0x10        // Bias selection / Internal OSC frequency adjust
#define LCD_EX_SETICONRAMADDR   0x40        // Set ICON RAM address
#define LCD_EX_POWICONCONTRASTH 0x50        // Power / ICON control / Contrast set(high byte)
#define LCD_EX_FOLLOWERCONTROL  0x60        // Follower control
#define LCD_EX_CONTRASTSETL     0x70        // Contrast set(low byte)

// flags for display entry mode
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON           0x04
#define LCD_DISPLAYOFF          0x00
#define LCD_CURSORON            0x02
#define LCD_CURSOROFF           0x00
#define LCD_BLINKON             0x01
#define LCD_BLINKOFF            0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00

// flags for function set
#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00
#define LCD_EX_INSTRUCTION      0x01        // IS: instruction table select

// flags for Bias selection
#define LCD_BIAS_1_4            0x08        // bias will be 1/4
#define LCD_BIAS_1_5            0x00        // bias will be 1/5

// flags Power / ICON control / Contrast set(high byte)
#define LCD_ICON_ON             0x08        // ICON display on
#define LCD_ICON_OFF            0x00        // ICON display off
#define LCD_BOOST_ON            0x04        // booster circuit is turn on
#define LCD_BOOST_OFF           0x00        // booster circuit is turn off
#define LCD_OSC_122HZ           0x00        // 122Hz@3.0V
#define LCD_OSC_131HZ           0x01        // 131Hz@3.0V
#define LCD_OSC_144HZ           0x02        // 144Hz@3.0V
#define LCD_OSC_161HZ           0x03        // 161Hz@3.0V
#define LCD_OSC_183HZ           0x04        // 183Hz@3.0V
#define LCD_OSC_221HZ           0x05        // 221Hz@3.0V
#define LCD_OSC_274HZ           0x06        // 274Hz@3.0V
#define LCD_OSC_347HZ           0x07        // 347Hz@3.0V

// flags Follower control
#define LCD_FOLLOWER_ON         0x08        // internal follower circuit is turn on
#define LCD_FOLLOWER_OFF        0x00        // internal follower circuit is turn off
#define LCD_RAB_1_00            0x00        // 1+(Rb/Ra)=1.00
#define LCD_RAB_1_25            0x01        // 1+(Rb/Ra)=1.25
#define LCD_RAB_1_50            0x02        // 1+(Rb/Ra)=1.50
#define LCD_RAB_1_80            0x03        // 1+(Rb/Ra)=1.80
#define LCD_RAB_2_00            0x04        // 1+(Rb/Ra)=2.00
#define LCD_RAB_2_50            0x05        // 1+(Rb/Ra)=2.50
#define LCD_RAB_3_00            0x06        // 1+(Rb/Ra)=3.00
#define LCD_RAB_3_75            0x07        // 1+(Rb/Ra)=3.75

//
// Initial data
//
//#define LCD_INIT_CONT    32       // Contrast 3.6V
//#define LCD_INIT_CONT    48       //  (0~63)  3.3V
#define LCD_INIT_CONT      50       //          3.3V
//#define LCD_INIT_CONT    62       //          2.7V
//#define LCD_INIT_CONT    40       //          3.5V

#endif  // LCD_H
