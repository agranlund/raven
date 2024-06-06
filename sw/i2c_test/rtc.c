#include "stdio.h"
#include "i2c.h"

#define I2C_DS1307  0x68

int superMain() {
    unsigned char ack, val;

    i2cEnable();
    i2cStart();
    ack = i2cWrite((I2C_DS1307 << 1) | 0x00);
    ack = i2cWrite(0x00);
    i2cStart();
    ack = i2cWrite((I2C_DS1307 << 1) | 0x01);
    val = i2cRead(1);
    i2cStop();
    i2cDisable();

    printf("got 0x%02x\r\n", val);
    return 0;
}

int main() {
    printf("RTC test\r\n");

    printf("supervisor ");
    Supexec(superMain);

    printf("usermode   ");
    superMain();

    printf("done\r\n");
    return 0;
}

