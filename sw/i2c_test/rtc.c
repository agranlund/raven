#include "stdio.h"
#include "i2c.h"

#define I2C_DS1307  0x68

//          7   6   5   4   3   2   1   0
//  $00     CH  10s 10s 10s s   s   s   s   seconds 00-59
//  $01     0   10m 10m 10m m   m   m   m   minutes 00-59
//  $02     0   fmt 10h 10h h   h   h   h   hours   1-12 AM/PM / 00-23
//  $03     0   0   0   0   0   d   d   d   day     01-07
//  $04     0   0   10d 10d d   d   d   d   date    01-31
//  $05     0   0   0   10m m   m   m   m   month   01-12
//  $06     10y 10y 10y 10y y   y   y   y   year    00-99
//  $07     out 0   0   sqe 0   0   rs1 rs0 control -
//  $08     x   x   x   x   x   x   x   x   ram     00-ff
//  ...     x   x   x   x   x   x   x   x   ram     00-ff
//  $3F     x   x   x   x   x   x   x   x   ram     00-ff

// CH  : 0 = oscillator enabled, 1 = disabled (default 1)
// out : output level when seq is disabled (default 0)
// sqe : square wave enable. 0 = disable, 1 = enabled
// rsx : square wave frequency

void rtcRead(unsigned char addr, unsigned char* buf, unsigned char siz)
{
    unsigned char ack, val;
    i2cStart();
    ack = i2cWrite((I2C_DS1307 << 1) | 0x00);   // enter write mode
    ack = i2cWrite(addr);                       // write address
    i2cStart();
    ack = i2cWrite((I2C_DS1307 << 1) | 0x01);   // enter read mode
    for (int i=0; i<siz-1; i++) {
        *buf++ = i2cRead(0);                    // read data, send ack
    }
    *buf = i2cRead(1);                          // read data, send !ack
    i2cStop();
    return val;
}

void rtcWrite(unsigned char addr, unsigned char* buf, unsigned char siz) {
    i2cStart();
    i2cWrite((I2C_DS1307 << 1) | 0x00); // enter write mode
    i2cWrite(addr);                     // write address
    for (int i=0; i<siz; i++) {
        i2cWrite(buf[i]);               // write value
    }
    i2cStop();
}

void rtcInit()
{
    i2cEnable();
    unsigned char buf[0x40];
    memset(buf, 0, 0x40);

    rtcRead(0x3C, &buf[0x3C], 4);
    if (memcmp(&buf[0x3C], "RAVN", 4) == 0) {
        printf("Inited already\r\n");
    }
    else {
        printf("Init rtc ram\r\n");
        buf[0x3C] = 'R';
        buf[0x3D] = 'A';
        buf[0x3E] = 'V';
        buf[0x3F] = 'N';
        rtcWrite(0x08, &buf[0x08], 0x38);
    }

    // enable oscillator
    rtcRead(0x00, &buf[0x00], 0x01);
    if (buf[0x00] & 0x80) {
        buf[0x00] = 0x00;
        rtcWrite(0x00, &buf[0x00], 0x01);
    }

    // 

    i2cDisable();
}

int superMain() {
    unsigned char ack, val;

    unsigned char buf[0x40];

    printf("rtc init\r\n");
    rtcInit();

    i2cEnable();
    printf("rtc read\r\n");
    rtcRead(0x00, buf, 0x40);
    i2cDisable();

    unsigned char* b = buf;
    for (int i=0; i<8; i++) {
        printf("%02x %02x %02x %02x %02x %02x %02x %02x\r\n",
        b[(i*8)+0], b[(i*8)+1], b[(i*8)+2], b[(i*8)+3], b[(i*8)+4], b[(i*8)+5], b[(i*8)+6], b[(i*8)+7]);
    }
    return 0;
}

int main() {
    printf("RTC test\r\n");

//    printf("supervisor ");
    Supexec(superMain);

//    printf("usermode   ");
//    superMain();

    printf("done\r\n");
    return 0;
}

