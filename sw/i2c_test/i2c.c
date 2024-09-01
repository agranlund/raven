#include "i2c.h"
#include "mint/osbind.h"    // for Super/Supexec

#define BOARD_REV 0xA1

#if (BOARD_REV == 0xA0)
#define MFP_BIT_DTA     7
#define MFP_BIT_CLK     3
#else
#define MFP_BIT_DTA     3
#define MFP_BIT_CLK     7
#endif
#define MFP_BITMASK     ((1 << MFP_BIT_CLK) | (1 << MFP_BIT_DTA))
#define I2C_DELAY       1000


static unsigned char param;
static unsigned char mfpOld07;     // int enable A
static unsigned char mfpOld09;     // int enable B

static inline void i2cDirClk(unsigned char v) {
    if (v) { *((volatile unsigned char*)0xfffa05) |= (1 << MFP_BIT_CLK);
    } else { *((volatile unsigned char*)0xfffa05) &= ~(1 << MFP_BIT_CLK); }
}
static inline void i2cDirDta(unsigned char v) {
    if (v) { *((volatile unsigned char*)0xfffa05) |= (1 << MFP_BIT_DTA);
    } else { *((volatile unsigned char*)0xfffa05) &= ~(1 << MFP_BIT_DTA); }
}
static inline unsigned char i2cGetDta() {
    return (((*((volatile unsigned char*)0xfffa01)) >> MFP_BIT_DTA) & 1);
}
static inline void i2cSetDta(unsigned char v) {
    if (v) { *((volatile unsigned char*)0xfffa01) |= (1 << MFP_BIT_DTA);
    } else { *((volatile unsigned char*)0xfffa01) &= ~(1 << MFP_BIT_DTA); }
}
static inline void i2cSetClk(unsigned char v) {
    if (v) { *((volatile unsigned char*)0xfffa01) |= (1 << MFP_BIT_CLK);
    } else   { *((volatile unsigned char*)0xfffa01) &= ~(1 << MFP_BIT_CLK); }
}
static inline void i2cDelay(unsigned char num) {
   for (unsigned int i=0; i<(num * I2C_DELAY); i++) {
    __asm__ __volatile__( " nop\n\t" : : : "cc" );
   }
}

static void i2cEnableS() {
    mfpOld09 = *((volatile unsigned char*)0xfffa09);
    mfpOld07 = *((volatile unsigned char*)0xfffa07);
    *((volatile unsigned char*)0xfffa09) &= ~(1<<3);
    *((volatile unsigned char*)0xfffa07) &= ~(1<<7);
    i2cSetClk(1);
    i2cDirClk(1);
    i2cSetDta(1);
    i2cDirDta(1);
}

static void i2cDisableS() {
    *((volatile unsigned char*)0xfffa09) &= ~(1<<3);
    *((volatile unsigned char*)0xfffa07) &= ~(1<<7);
    *((volatile unsigned char*)0xfffa05) &= ~(MFP_BITMASK);
    *((volatile unsigned char*)0xfffa09) = mfpOld09;
    *((volatile unsigned char*)0xfffa07) = mfpOld07;
}

static void i2cStartS() {
    i2cSetDta(1); i2cDirDta(1); i2cDelay(1);
    i2cSetClk(1); i2cDelay(1);
    i2cSetDta(0); i2cDelay(1);
    i2cSetClk(0); i2cDelay(2);
}

static void i2cStopS() {
    i2cSetDta(0); i2cDirDta(1); i2cDelay(1);
    i2cSetClk(1); i2cDelay(1);
    i2cSetDta(1); i2cDelay(2);
}

static unsigned char i2cWriteS() {
    unsigned char v = param;
    i2cDirDta(1);
    for (short i = 0; i < 8; i++) {
        i2cSetDta(v & 0x80);
        i2cDelay(1);
        i2cSetClk(1);
        i2cDelay(2);
        i2cSetClk(0);
        i2cDelay(1);
        v = v << 1;
    }
    i2cDelay(1);
    i2cDirDta(0);
    i2cSetClk(1);
    i2cDelay(2);
    // ack
    unsigned char ack = i2cGetDta();
    i2cSetClk(0);
    i2cDelay(1);
    i2cSetDta(1);
    i2cDirDta(1);
    return ack;
}

static unsigned char i2cReadS() {
    unsigned char ack = param;
    unsigned char v = 0;
    i2cDirDta(0);
    i2cDelay(1);
    for (short i = 0; i< 8; i++) {
        v = v << 1;
        i2cSetClk(1);
        i2cDelay(2);
        v |= i2cGetDta();
        i2cSetClk(0);
        i2cDelay(2);
    }
    i2cSetDta(ack);
    i2cDirDta(1);
    i2cSetClk(1);
    i2cDelay(2);
    i2cSetClk(0);
    i2cDelay(1);
    return v;
}

// -----------------------------------------

void i2cEnable()  { if (Super(1) == 0) { Supexec(i2cEnableS);  } i2cEnableS();  }
void i2cDisable() { if (Super(1) == 0) { Supexec(i2cDisableS); } i2cDisableS(); }
void i2cStart()   { if (Super(1) == 0) { Supexec(i2cStartS);   } i2cStartS();   }
void i2cStop()    { if (Super(1) == 0) { Supexec(i2cStopS);    } i2cStopS();    }
unsigned char i2cWrite(unsigned char v)  { param = v; if (Super(1) == 0) { return Supexec(i2cWriteS); } return i2cWriteS(); }
unsigned char i2cRead(unsigned char v)   { param = v; if (Super(1) == 0) { return Supexec(i2cReadS);  } return i2cReadS(); }

