
#ifndef TWI_I2CMASTER_H
#define TWI_I2CMASTER_H

#include <mytype.h>

void TWI_I2CMasterStart(void);
u8	TWI_I2CWrite(u8 addr, u8* buf, u16 Length, u8 Multi) ;
void TWI_I2CStop(void);

#endif
