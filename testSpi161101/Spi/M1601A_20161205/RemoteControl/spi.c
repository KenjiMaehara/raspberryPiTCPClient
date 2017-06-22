/*
 * 
 *
 * 
 * 
 */ 
#include <avr/io.h>
#include <mytype.h>
#include "port.h"

#define WAIT_EORX() while (! (SPIC.STATUS & SPI_IF_bm)){} 
#define	SPIC_BIT_ENABLE	6
#define SPIC_BIT_MASTER 4	
void spi_init(void)
{
	// -enables module
	// -set to mast mode
	// -SPI clk = 32MHz/4=8MHz
	// -mode set to 0
		
	//SPIC.CTRL = SPI_PRESCALER_DIV16_gc |  SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_0_gc;// | SPI_CLK2X_bm 
	SPIC_CTRL |= (1<<SPIC_BIT_ENABLE) | (1<<SPIC_BIT_MASTER);
	SPIC_INTCTRL = 0x02;	
}

u8 spi_write_single(u8 byte)
{
	u8 dummy;

	SPIC.DATA = byte;
	while (! (SPIC.STATUS & SPI_IF_bm)) 
	{
		; 
	}

	WAIT_EORX();
	dummy = SPIC.DATA;
	return dummy;
}

u8 spi_read_single(void)
{
	return spi_write_single(0x0);
}

u8 spi_write(u8* data, u16 len)
{
	u8 dummy=0;
	u16 i;
	
	for (i=0; i<len; i++)
	{
		SPIC.DATA = data[i];
		WAIT_EORX();
		dummy = SPIC.DATA;
	}
	return dummy;
}

void spi_read(u8* data, u16 len)
{
	u16 i;
	
	for (i=0; i<len; i++)
	{
		SPIC.DATA = 0x0;
		WAIT_EORX();
		data[i] = SPIC.DATA;
	}
}

void spi_select(void)
{
	set_spi_ss(false);
}

void spi_deselect(void) 
{
	set_spi_ss(true);
}

u16 spi_read_somi(void)
{
	return (u16)get_spi_ss();
}
