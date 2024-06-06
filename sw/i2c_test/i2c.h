#ifndef _I2C_H_
#define _I2C_H_

extern void i2cEnable();
extern void i2cDisable();
extern void i2cStart();
extern void i2cStop();
extern unsigned char i2cWrite(unsigned char value);
extern unsigned char i2cRead(unsigned char ack);

#endif // _I2C_H_
