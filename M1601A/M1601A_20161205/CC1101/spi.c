#include <avr/io.h>
#include "spi1.h"
#include "macro.h"

/* Local Macros */

/*
 * wait until a byte has been received on spi port
 */
#define WAIT_EORX() while (! (SPSR & _BV(SPIF))){} // SPIFがセットされるのを待つ


/*
 * wait until a byte has been sent on spi port
 */
#define CC1100_CS_PIN (1<<2)

#define CC1100_ENABLE()  PORTB &= ~CC1100_CS_PIN
#define CC1100_DISABLE() PORTB |=  CC1100_CS_PIN


void spi_init(void) 
{
    /* Configure IO pins */
	PBDDR |= (1<<5) | (1<<3); /* output for CLK and MOSI */
	PBDDR &=  ~(1<<4);   /* input for MISO */

	PORTB &= ~(1<<3);	//MOSI LOW

	SPCR = _BV(SPE) | _BV(MSTR);
    CC1100_DISABLE();
}






u8 spi_write_single(u8 byte) 
{
    u8 dummy;

	SPDR = byte;
	while (! (SPSR & _BV(SPIF))) {
	;  // SPIFがセットされるのを待つ
	}

	WAIT_EORX();
    dummy = SPDR;
    return dummy;
}

u8 spi_read_single(void) 
{
    return spi1_write_single(0x0);
}

u8 spi_write(u8* data, u16 len) 
{
    u8 dummy=0;
    u16 i;
    
    for (i=0; i<len; i++) 
	{
		SPDR = data[i];
        WAIT_EORX();
		dummy = SPDR;
    }
    return dummy;
}
void spi_read(u8* data, u16 len) 
{
    u16 i;
    
    for (i=0; i<len; i++) 
	{
		SPDR = 0x0;
        WAIT_EORX();
		data[i] = SPDR;
    }
}

void spi_select(u16 chip) 
{
    switch (chip) {
    case SPI1_CC1100:
        CC1100_ENABLE();
        break;
    default:
        break;
    }
}

void spi_deselect(u16 chip) {
    switch (chip) {
    case SPI1_CC1100:
        CC1100_DISABLE();
        break;
    default:
        break;
    }
}

u16 spi_read_somi(void) 
{
    return PINB & (1<<2);
}


#if 0
u8 spi_active=0;
void spi_signalpin_opendrain_active(void)
{
	SPCR &= ~(1<<SPE);	//SPI non active
	DDRB &= ~(1<<PB2);	//CSn
	DDRB &= ~(1<<PB3);	//MOSI
	DDRB &= ~(1<<PB5);	//SCK

	PORTB &= ~(1<<PB2);	//CSn

	PORTC = 0x00;
	PORTD = 0x00;
	spi_active =1 ;
}


void spi_signalpin_opendrain_nonactive(void)
{
	spi1_init();		
	//DDRB |= (1<<PB0);	//B_CTL
	PORTB &= ~(1<<PB2);	//CSn
	DDRB |= (1<<PB2);	//CSn output

	PORTC |= (1<<PC0);
	PORTC |= (1<<PC1);
	PORTC |= (1<<PC2);
	PORTC |= (1<<PC3);
	PORTC |= (1<<PC4);
	PORTC |= (1<<PC5);
	PORTD = 0xf7;
	spi_active =0 ;
}

#endif 

