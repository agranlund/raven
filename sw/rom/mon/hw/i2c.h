#ifndef _I2C_H_
#define _I2C_H_
#ifndef __ASM__
#include "../sys.h"

bool    i2c_Init();

bool    i2c_Aquire();
void    i2c_Release();

void    i2c_Start();
void    i2c_Stop();
uint8_t i2c_Read(uint8_t ack);
uint8_t i2c_Write(uint8_t val);

#endif //!__ASM__
#endif // _I2C_H_

