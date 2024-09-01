#include "../sys.h"
#include "i2c.h"

#define I2C_DS1307      0x68

#define RTC_RAM_START   0x08
#define RTC_RAM_END     0x40

static uint8_t  int_to_bcd8(uint32_t v) { return (v % 10) + ((v / 10) << 4); }
static uint32_t bcd8_to_int(uint8_t  v) { return (v & 15) + ((v >> 4) * 10); }

static void rtc_Read(uint8_t addr, uint8_t* buf, uint8_t siz)
{
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
}

static void rtc_Write(uint8_t addr, uint8_t* buf, uint8_t siz)
{
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

static void rtc_AccessRam(uint8_t addr, uint8_t* buf, uint8_t siz, bool read)
{
    uint8_t reg = (addr + RTC_RAM_START);
    if (reg < RTC_RAM_END) {
        siz = ((reg + siz) < RTC_RAM_END) ? siz : (RTC_RAM_END - reg);
        if (read) {
            rtc_Read(reg, buf, siz);
        } else {
            rtc_Write(reg, buf, siz);
        }
    }
}

bool rtc_Init()
{
    unsigned char buf[0x40];
    memset(buf, 0, 0x40);

    rtc_Read(0x3C, &buf[0x3C], 4);
    if (memcmp(&buf[0x3C], "RAVN", 4) == 0) {
        return true;
    }
    else {
        buf[0x03] = 0x01;
        buf[0x04] = 0x01;
        buf[0x05] = 0x01;
        buf[0x06] = 0x00;
        buf[0x3C] = 'R';
        buf[0x3D] = 'A';
        buf[0x3E] = 'V';
        buf[0x3F] = 'N';
        rtc_Write(0x00, &buf[0x00], 0x40);
    }

    // sanity check
    rtc_Read(0x00, &buf[0x00], 0x07);
    if (buf[0x00] & 0x80) {
        buf[0x00] = 0x00;
        rtc_Write(0x00, &buf[0x00], 0x01);
    }

    // todo: reset to 24h format if it was changed?
    return true;
}

void rtc_SetDateTime(uint32_t dt, uint32_t mask)
{
    uint8_t regs[8];
    regs[0] = int_to_bcd8((dt <<  1) & 0x3f);       // seconds
    regs[1] = int_to_bcd8((dt >>  5) & 0x3f);       // minutes
    regs[2] = int_to_bcd8((dt >> 11) & 0x31);       // hours
    regs[3] = 1;                                    // day
    regs[4] = int_to_bcd8((dt >> 16) & 0x31);       // date
    regs[5] = int_to_bcd8((dt >> 21) & 0x0f);       // month
    regs[6] = int_to_bcd8((dt >> 25) & 0x7f);       // year
    rtc_Write(0x00, regs, 0x07);
}

uint32_t rtc_GetDateTime()
{
    uint8_t regs[8];
    rtc_Read(0x00, regs, 0x07);

    uint32_t dt = 0;
    dt |= ((bcd8_to_int(regs[6]) & 0x7f) << 25);    // year
    dt |= ((bcd8_to_int(regs[5]) & 0x0f) << 21);    // month
    dt |= ((bcd8_to_int(regs[4]) & 0x31) << 16);    // date
    dt |= ((bcd8_to_int(regs[2]) & 0x31) << 11);    // hours
    dt |= ((bcd8_to_int(regs[1]) & 0x3f) <<  5);    // minutes
    dt |= ((bcd8_to_int(regs[0]) & 0x3f) >>  1);    // seconds
    return dt;
}

void rtc_GetRam(uint8_t addr, uint8_t* buf, uint8_t siz)
{
    rtc_AccessRam(addr, buf, siz, true);
}

void rtc_SetRam(uint8_t addr, uint8_t* buf, uint8_t siz)
{
    rtc_AccessRam(addr, buf, siz, false);
}
