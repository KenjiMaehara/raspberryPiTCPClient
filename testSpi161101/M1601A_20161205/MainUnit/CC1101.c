/*
 * CC1101.c
 *
 * Created: 
 *  Author: 
 */ 

#include "cc1100.h"
#include "cc1100_globals.h"
#include <mytype.h>



static u16 cc1101_freq;

static void (*mdelay)(u16 msec);
static u8 (*spi_write_single)(u8 byte);
static u8 (*spi_read_single)(void);
static u8 (*spi_write)(u8* data, u16 len);
static void (*spi_read)(u8* data, u16 len);
static void (*spi_select)(void);
static void (*spi_deselect)(void);
static u16 (*spi_read_somi)(void);



void cc1100_init(void (*mili)(u16 msec),
				u8 (*write_single)(u8 byte),
				u8 (*read_single)(void),
				u8 (*write)(u8* data, u16 len),
				void (*read)(u8* data, u16 len),
				void (*select)(void),
				void (*deselect)(void),
				u16 (*read_somi)(void)
				)
{
	mdelay = mili;
	spi_write_single = write_single;
	spi_read_single = read_single;
	spi_write = write;
	spi_read = read;
	spi_select = select;
	spi_deselect = deselect;
	spi_read_somi = read_somi;
	
	//spi_init();
	spi_select();
	spi_deselect();
	spi_select();
	spi_deselect();
	mdelay(1);
	spi_select();
	while (spi_read_somi()) ;
	spi_write_single(CC1100_STROBE_SRES | CC1100_ACCESS_STROBE);
	while (spi_read_somi()) ;
	spi_deselect();

	mdelay(1);


  #if 0
	//setup 426.361755MHz
	cc1100_write_reg(CC1100_REG_FREQ2, 0x10);
	cc1100_write_reg(CC1100_REG_FREQ1, 0x66);
	cc1100_write_reg(CC1100_REG_FREQ0, 0x07);
#endif
}

u8 cc1100_read_reg(u8 addr)
{
	u8 reg;
	spi_select();
	spi_write_single(addr | CC1100_ACCESS_READ);
	reg = spi_read_single();
	spi_deselect();
	return reg;
}

void cc1100_write_reg(u8 addr, u8 value)
{
	spi_select();
	spi_write_single(addr | CC1100_ACCESS_WRITE);
	spi_write_single(value);
	spi_deselect();
}

u8 cc1100_strobe_cmd(u8 cmd)
{
	u8 ret;
	spi_select();
	while(spi_read_somi());
	ret = spi_write_single(cmd | CC1100_ACCESS_STROBE);
	spi_deselect();
	return ret;
}

u8 cc1100_sleep_strobe_cmd(u8 cmd)
{
	u8 ret;
	ret = spi_write_single(cmd | CC1100_ACCESS_STROBE);
	return ret;
}

void cc1100_fifo_put(u8* buffer, u16 length)
{
  spi_select();
  spi_write_single(CC1100_DATA_FIFO_ADDR | CC1100_ACCESS_WRITE_BURST);
  spi_write(buffer, length);
  spi_deselect();
}

void cc1100_fifo_get(u8* buffer, u16 length)
{
	spi_select();
	spi_write_single(CC1100_DATA_FIFO_ADDR | CC1100_ACCESS_READ_BURST);
	spi_read(buffer, length);
	spi_deselect();
}

u8 cc1100_read_status(u8 addr)
{
	u8 temp;

	temp = cc1100_read_reg(addr | CC1100_ACCESS_STATUS);

	return temp;
}



#define STATE_IDLE    0
#define STATE_RX      1
#define STATE_TX      2
#define STATE_FSTXON  3
#define STATE_CALIB   4
#define STATE_SETTL   5
#define STATE_RXOVER  6
#define STATE_TXUNDER 7

#define WAIT_STATUS(status) \
	while ( ((cc1100_cmd_nop()>>4) & 0x7) != status) ;

void cc1100_cmd_calibrate(void)
{
	u8 status;
	cc1100_cmd_idle();
	cc1100_strobe_cmd(CC1100_STROBE_SCAL);

	do
	{
		status = ((cc1100_cmd_nop()>>4) & 0x7);
	}
	while(status  != STATE_IDLE );
}

void cc1100_cmd_idle(void)
{
	switch ((cc1100_cmd_nop() >> 4) & 0x7)
	{
		case STATE_RXOVER:
			cc1100_cmd_flush_rx();
			break;
		case STATE_TXUNDER:
			cc1100_cmd_flush_tx();
			break;
		default:
		cc1100_strobe_cmd(CC1100_STROBE_SIDLE);
	}
	WAIT_STATUS(STATE_IDLE);
}


u8 pt_data[] = {0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00};		//10dB


reg_t init_all[11]={
					{CC1100_REG_PKTCTRL1,0x04},
					{CC1100_REG_PKTCTRL0,0x45},
					{CC1100_REG_FSCTRL1,0x0C},
					{CC1100_REG_MCSM0,0x04},
					#ifdef GFSK_DEF
					{CC1100_REG_MDMCFG2,0x17},			//GFSK
					#else
					{CC1100_REG_MDMCFG2,0x07},			//2-FSK]
					#endif
					{CC1100_REG_PKTCTRL0,0x05},
					{CC1100_REG_MDMCFG3,0x83},
					{CC1100_REG_MDMCFG4,0x88},
					#ifdef GFSK_DEF
					//{CC1100_REG_DEVIATN,0x27},			//12kHz
					{CC1100_REG_DEVIATN,0x13},					//4.36kHz
					//{CC1100_REG_DEVIATN,0x00},					//1.19kHz				
					#else
					//{CC1100_REG_DEVIATN,0x17},		//6kHz
					{CC1100_REG_DEVIATN,0x13},					//4.36kHz
					//{CC1100_REG_DEVIATN,0x00},					//1.19kHz		
					#endif
					{CC1100_REG_MDMCFG4,0xF8},
					{CC1100_REG_AGCCTRL1,0x50},
				};

void cc1101_init_reg(void)
{
	u8 i;

	for(i = 0 ;i < 10 ; i++) 
	{ 
		cc1100_write_reg(init_all[i].addr,init_all[i].data);
	} 
}

void cc1101_rtx_reg_set(u8 rtx)
{
	cc1101_init_reg();
	cc1100_write_reg(CC1100_REG_PKTCTRL1,rtx == 1 ? 0x0c : 0x04);
}


void cc1101_8PATABLE_write_reg(void)
{
	u8 i;

	spi_select();
	spi_write_single(0x3e | CC1100_ACCESS_WRITE_BURST);
	for(i=0;i<8;i++)
	{
		spi_write_single(pt_data[i]);
	}
	spi_deselect();
}


void tx_fifo_write(u8 * data,u8 length)
{
	#if 0
	data[0] = length + 2;
	cc1100_fifo_put(data,1);
	cc1100_fifo_put(data,length+2);
	#endif
	
	#if 1
	data[0] = length;
	cc1100_fifo_put(data,1);
	cc1100_fifo_put(data,length);
	#endif
}


u8 rx_fifo_read(u8 * data,u8 * rssi, u8 * lqi)
{
	u8 length;

	cc1100_fifo_get(&length,1);

	cc1100_fifo_get(data,length);

	cc1100_fifo_get(rssi,1);
	cc1100_fifo_get(lqi,1);

	return length;
	//return length - 2;
}



void cc1101_set_freq(u16 frq)
{
	cc1101_freq	= frq;
}



void cc1101_carrier_wave_setup(void)
{
	cc1100_write_reg(CC1100_REG_FREQ2, 0x10);
	cc1100_write_reg(CC1100_REG_FREQ1, (cc1101_freq >> 8)  & 0xff);
	cc1100_write_reg(CC1100_REG_FREQ0, cc1101_freq & 0xff);
}

