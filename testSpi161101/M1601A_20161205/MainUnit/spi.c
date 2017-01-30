/*
 * 
 *
 * 
 * 
 */ 
#include <avr/io.h>
#include <mytype.h>
#include "port.h"

#define WAIT_EORX() while (! (SPIF.STATUS & SPI_IF_bm)){} 
#define	SPIE_BIT_ENABLE	6
#define SPIE_BIT_MASTER 4	
#define	SPIF_BIT_ENABLE	6
#define SPIF_BIT_MASTER 4
void spi_init(void)
{
	// -enables module
	// -set to mast mode
	// -SPI clk = 32MHz/4=8MHz
	// -mode set to 0
		
	//SPIC.CTRL = SPI_PRESCALER_DIV16_gc |  SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_0_gc;// | SPI_CLK2X_bm
	SPIF_CTRL |= (1<<SPIF_BIT_ENABLE);
	//SPIE_CTRL |= (1<<SPIE_BIT_ENABLE) | (1<<SPIE_BIT_MASTER);
	SPIF_INTCTRL = 0x00;
	//SPIE_INTCTRL = 0x00;	
}

u8 spi_write_single(u8 byte)
{
	u8 dummy;

	SPIF.DATA = byte;
	while (! (SPIF.STATUS & SPI_IF_bm)) 
	{
		; 
	}	
	WAIT_EORX();
	//dummy = SPIF.DATA;
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
		SPIF.DATA = data[i];
		WAIT_EORX();
		dummy = SPIF.DATA;
	}
	return dummy;
}

void spi_read(u8* data, u16 len)
{
	u16 i;
	
	for (i=0; i<len; i++)
	{
		SPIF.DATA = 0x0;
		WAIT_EORX();
		data[i] = SPIF.DATA;
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
