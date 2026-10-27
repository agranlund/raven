#include "sys.h"
#include "hw/i2c.h"
#include "hw/rtc.h"

#define I2C_DS1307      0x68

/*
static uint8_t  int_to_bcd8(uint32_t v) { return (v % 10) + ((v / 10) << 4); }
static uint32_t bcd8_to_int(uint8_t  v) { return (v & 15) + ((v >> 4) * 10); }
*/

void rtc_Read(uint8_t addr, uint8_t* buf, uint8_t siz)
{
    if (addr < RTC_RAM_END) {
        i2c_Aquire();
        i2c_Start();
        i2c_Write((I2C_DS1307 << 1) | 0x00);  // enter write mode
        i2c_Write(addr);                      // write address
        i2c_Start();
        i2c_Write((I2C_DS1307 << 1) | 0x01);  // enter read mode
        for (int i=0; i<siz-1; i++) {
            *buf++ = i2c_Read(0);                   // read data, send ack
        }
        *buf = i2c_Read(1);                         // read data, send !ack
        i2c_Stop();
        i2c_Release();
    } else if (siz > 0) {
        memset(buf, 0, siz);
    }
}

void rtc_Write(uint8_t addr, uint8_t* buf, uint8_t siz)
{
    if (addr < RTC_RAM_END) {
        i2c_Aquire();
        i2c_Start();
        i2c_Write((I2C_DS1307 << 1) | 0x00); // enter write mode
        i2c_Write(addr);                     // write address
        for (int i=0; i<siz; i++) {
            i2c_Write(buf[i]);               // write value
        }
        i2c_Stop();
        i2c_Release();
    }
}

bool rtc_Valid()
{
    uint8_t buf[3];
    rtc_Read(0x3C, buf, 3);
    return (memcmp(buf, "RVN", 3) == 0) ? true : false;
}

void rtc_ClearRam()
{
    unsigned char buf[0x40];
    memset(buf, 0, 0x40);
    buf[0x3C] = 'R';
    buf[0x3D] = 'V';
    buf[0x3E] = 'N';
    rtc_Write(RTC_RAM_START, &buf[RTC_RAM_START], RTC_RAM_END - RTC_RAM_START);
}

void rtc_Reset()
{
    unsigned char buf[RTC_RAM_START];
    memset(buf, 0, RTC_RAM_START);
    // todo: put a sensible default date
    buf[0x04] = 0x01;           // day
    buf[0x05] = 0x01;           // month
    buf[0x06] = 0x56;           // year
    rtc_Write(0x00, buf, RTC_RAM_START);
    rtc_ClearRam();
}

bool rtc_Init()
{
    if (!rtc_Valid()) {
        puts("ResetRtc");
        rtc_Reset();
    }

    // sanity check clock settings
    unsigned char buf[0x40];
    rtc_Read(0x00, &buf[0x00], 0x07);
    if (buf[0x00] & 0x80) {
        buf[0x00] = 0x00;
        rtc_Write(0x00, &buf[0x00], 0x01);
    }

    // todo: reset to 24h format if it was changed?
    return rtc_Valid();
}
