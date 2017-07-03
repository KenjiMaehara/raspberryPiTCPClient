/*!


*/

#include "i2c.h"
#include "twi_master_driver.h"

TWI_Master_t twiMaster;    /*!< TWI master module. */

void i2c_init(u32 baudrate)
{
    u32 clk_spd, i2c_spd;

    // calculate baudrate for I2C
    clk_spd = 32000000;
    i2c_spd = TWI_BAUD(clk_spd, baudrate);

    // I2C interrupt initialized to medium priority level
    TWI_MasterInit(&twiMaster, &TWIC,TWI_MASTER_INTLVL_MED_gc, i2c_spd);
}

u8 i2c_read8(u8 dev_addr, u8 reg_addr)
{
    u8 data;

    TWI_MasterWriteRead(&twiMaster, dev_addr, &reg_addr, 1, 1);
    while (twiMaster.status != TWIM_STATUS_READY);

    data = twiMaster.readData[0];

    return data;
}

u16 i2c_read16(u8 dev_addr, u8 reg_addr)
{
    u8 data[2];

    TWI_MasterWriteRead(&twiMaster, dev_addr, &reg_addr, 1, 2);
    while (twiMaster.status != TWIM_STATUS_READY);

    data[1] = twiMaster.readData[0];
    data[0] = twiMaster.readData[1];

    return *(u16 *)data;
}

void i2c_write8(u8 dev_addr, u8 reg_addr, u8 val)
{
    u8 data[2];

    data[0] = reg_addr;
    data[1] = val;

    TWI_MasterWriteRead(&twiMaster, dev_addr, &reg_addr, 2, 0);
    while (twiMaster.status != TWIM_STATUS_READY);
}

void i2c_write16(u8 dev_addr, u8 reg_addr, u16 val)
{
    u8 data[3];

    data[0] = reg_addr;
    data[1] = val >> 8;
    data[2] = val;

    TWI_MasterWriteRead(&twiMaster, dev_addr, &reg_addr, 3, 0);
    while (twiMaster.status != TWIM_STATUS_READY);
}

ISR(TWIC_TWIM_vect)
{
    TWI_MasterInterruptHandler(&twiMaster);
}
