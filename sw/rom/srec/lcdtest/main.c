/*
 * DFRobot i2c rgb-lcd1602 test
 */

#include <lib.h>
#include "../../../lib/raven.h"

static uint8_t _lcdaddr = 0x3e;
static uint8_t _rgbaddr = 0x00;
static uint8_t _width   = 16;
static uint8_t _height  = 2;

#if defined(__GNUC__)
static inline void lcd1602_nop(void) { __asm__ volatile ( "nop\n\t" : : : ); }
#else
#define inline 
static void lcd1602_nop(void) 0x4E71;
#endif

static void lcd1602_delay(uint32_t num) {
    for (uint32_t i=0; i<(num * 1000); i++) {
        lcd1602_nop();
    }
}

static uint8_t lcd1602_fontdata[8*8] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f,
  0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f,
  0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
};

static void lcd1602_write(uint8_t dev, uint8_t reg, uint8_t val) {
    raven()->i2c_Aquire();
    raven()->i2c_Start();
    raven()->i2c_Write(dev << 1);
    raven()->i2c_Write(reg);
    raven()->i2c_Write(val);
    raven()->i2c_Stop();
    raven()->i2c_Release();    
}

void lcd1602_putc(char c) {
    lcd1602_write(_lcdaddr, 0x40, (uint8_t)c);
}

void lcd1602_puts(char* s) {
    while (*s != 0) {
        lcd1602_putc(*s++);
    }
}

void lcd1602_display(bool enable) {
    uint8_t val = enable ? 0x04 : 0x00;
    lcd1602_write(_lcdaddr, 0x80, 0x08 | val);
}

void lcd1602_pos(uint8_t x, uint8_t y) {
    lcd1602_write(_lcdaddr, 0x80, 0x80 | ((y & 1)<< 6) | (x & 15));
}

void lcd1602_clear() {
    lcd1602_write(_lcdaddr, 0x80, 0x01);
    lcd1602_pos(0, 0);
}


void lcd1602_font(uint8_t idx, uint8_t num, uint8_t* data) {
    int i, j;

    idx = idx & 7;
    num = ((idx + num) > 8) ? (8 - idx) : num;
    
    raven()->i2c_Aquire();
    for (i = 0; i < num; i++) {
        lcd1602_write(_lcdaddr, 0x80, 0x40 | ((idx + i) << 3));
        raven()->i2c_Aquire();
        raven()->i2c_Start();
        raven()->i2c_Write(_lcdaddr << 1);
        raven()->i2c_Write(0x40);
        for (j = 0; j < 8; j++) {
            raven()->i2c_Write(*data++);
        }
        raven()->i2c_Stop();
        raven()->i2c_Release();
    }

    lcd1602_pos(0, 0);
}

void lcd1602_rgb(uint8_t r, uint8_t g, uint8_t b) {
    switch (_rgbaddr) {
        case 0x60: {
            lcd1602_write(_rgbaddr, 0x04, r);
            lcd1602_write(_rgbaddr, 0x03, g);
            lcd1602_write(_rgbaddr, 0x02, b);
        } break;
        case 0x30: {
            lcd1602_write(_rgbaddr, 0x06, (uint8_t)((((uint16_t)r)*192)/255));
            lcd1602_write(_rgbaddr, 0x07, (uint8_t)((((uint16_t)g)*192)/255));
            lcd1602_write(_rgbaddr, 0x08, (uint8_t)((((uint16_t)b)*192)/255));
        } break;
        case 0x6b: {
            lcd1602_write(_rgbaddr, 0x06, r);
            lcd1602_write(_rgbaddr, 0x05, g);
            lcd1602_write(_rgbaddr, 0x04, b);
            lcd1602_write(_rgbaddr, 0x07, 0xff);
        } break;
        case 0x2d: {
            lcd1602_write(_rgbaddr, 0x01, r);
            lcd1602_write(_rgbaddr, 0x02, g);
            lcd1602_write(_rgbaddr, 0x03, b);
        } break;
    }
}

void lcd1602_init(uint8_t lcdaddr, uint8_t rgbaddr, uint8_t width, uint8_t height){
    uint8_t val;
    _lcdaddr = lcdaddr ? lcdaddr : _lcdaddr;
    _rgbaddr = rgbaddr ? rgbaddr : _rgbaddr;
    _width = ((width > 0) && (width <= 16)) ? width : _width;
    _height = ((height > 0) && (height <= 2)) ? height : _height;

    val = (_height >= 2) ? 0x08 : 0x00;
    lcd1602_write(_lcdaddr, 0x80, 0x20 | val); lcd1602_delay(5);
    lcd1602_write(_lcdaddr, 0x80, 0x20 | val); lcd1602_delay(5);
    lcd1602_write(_lcdaddr, 0x80, 0x20 | val); lcd1602_delay(5);

    lcd1602_display(false);
    lcd1602_clear();
    lcd1602_delay(2);

    // left to right entry mode
    lcd1602_write(_lcdaddr, 0x80, 0x04 | 0x02 | 0x00);

    // write a blank character else the screen wont init properly
    lcd1602_pos(0, 0);
    lcd1602_putc(' ');
    lcd1602_pos(0, 0);

    // init backlight
    if (_rgbaddr) {
        switch (_rgbaddr) {
            case 0x30: {
                lcd1602_write(_rgbaddr, 0x01, 0x00);
                lcd1602_write(_rgbaddr, 0x02, 0xff);
                lcd1602_write(_rgbaddr, 0x04, 0x15);
            } break;
            case 0x60: {
                lcd1602_write(_rgbaddr, 0x00, 0x00);
                lcd1602_write(_rgbaddr, 0x08, 0xff);
                lcd1602_write(_rgbaddr, 0x01, 0x20);
            } break;
            case 0x6b: {
                lcd1602_write(_rgbaddr, 0x2f, 0x00);
                lcd1602_write(_rgbaddr, 0x00, 0x20);
                lcd1602_write(_rgbaddr, 0x01, 0x00);
                lcd1602_write(_rgbaddr, 0x02, 0x01);
                lcd1602_write(_rgbaddr, 0x03, 0x04);
            } break;
        }
        // default off
        lcd1602_rgb(0x00, 0x00, 0x00);
    }

    // custom bargraph font
    lcd1602_font(0, 8, lcd1602_fontdata);
    lcd1602_pos(0, 0);
}

void main() {
    int i;

    // DFRobot RGB-LCD
    lcd1602_init(0x3e, 0x2d, 16, 2);

    lcd1602_puts("  RAVEN  68060  ");

    lcd1602_pos(0, 1);
    for (i=7; i>=0; i--) { lcd1602_putc(i); }
    for (i=0; i<=7; i++) { lcd1602_putc(i); }

    lcd1602_display(true);
    lcd1602_rgb(0xff, 0xff, 0xff);
}
