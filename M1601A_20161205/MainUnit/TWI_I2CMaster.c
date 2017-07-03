#include <avr/io.h>						// WinAVR library
#include "TWI_I2CMaster.h"
#include <util/twi.h>					// WinAVR library 


void TWI_I2CMasterStart(void) 
{
	PORTC.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
	PORTC.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;
	
	TWIC_MASTER_BAUD = 0x90;
	TWIC_MASTER_CTRLA |= 0x08;  
	TWIC_MASTER_STATUS |= 0X01;
	
}


u8 TWI_I2CWrite(u8 addr, u8* buf, u16 Length, u8 Multi) 
{
	u16 i;
  
	TWIC_MASTER_ADDR = addr;

	while (!(TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm));
	
	for(i = 0 ; i < Length ;i ++)
	{
		TWIC_MASTER_DATA = buf[i];	
		while (!(TWIC.MASTER.STATUS & TWI_MASTER_WIF_bm));
	}

	// Send stop condition
	if (!Multi) 
	{
		 TWIC_MASTER_CTRLC = 0X03;
	}

	return(0);

}

void TWI_I2CStop(void){
}
