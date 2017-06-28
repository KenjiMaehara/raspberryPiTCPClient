
/*!
    \file 
    \ingroup


*/

#ifndef I2C_H
#define I2C_H

#include <mytype.h>

void i2c_init(u32 baudrate);
u8 i2c_read8(u8 dev_addr, u8 reg_addr);
u16 i2c_read16(u8 dev_addr, u8 reg_addr);
void i2c_write8(u8 dev_addr, u8 reg_addr, u8 val);
void i2c_write16(u8 dev_addr, u8 reg_addr, u16 val);

#endif
