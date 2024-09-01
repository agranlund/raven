#include "../sys.h"
#include "i2c.h"
#include "mfp.h"
#include "cpu.h"

#if (BOARD_REV == 0xA0)
#define MFP_BIT_DTA     7
#define MFP_BIT_CLK     3
#else
#define MFP_BIT_DTA     3
#define MFP_BIT_CLK     7
#endif

#define MFP_BITMASK     ((1 << MFP_BIT_CLK) | (1 << MFP_BIT_DTA))
#define I2C_DELAY       1000

bool i2cLocked;
unsigned char i2cOldMfp07;
unsigned char i2cOldMfp09;

static inline void i2cDirClk(uint8_t v) {
    if (v) { *((volatile uint8_t*)(PADDR_MFP1 + MFP_DDR)) |= (1 << MFP_BIT_CLK);
    } else { *((volatile uint8_t*)(PADDR_MFP1 + MFP_DDR)) &= ~(1 << MFP_BIT_CLK); }
}
static inline void i2cDirDta(uint8_t v) {
    if (v) { *((volatile uint8_t*)(PADDR_MFP1 + MFP_DDR)) |= (1 << MFP_BIT_DTA);
    } else { *((volatile uint8_t*)(PADDR_MFP1 + MFP_DDR)) &= ~(1 << MFP_BIT_DTA); }
}
static inline uint8_t i2cGetDta() {
    return (((*((volatile uint8_t*)(PADDR_MFP1 + MFP_GPDR))) >> MFP_BIT_DTA) & 1);
}
static inline void i2cSetDta(uint8_t v) {
    if (v) { *((volatile unsigned char*)(PADDR_MFP1 + MFP_GPDR)) |= (1 << MFP_BIT_DTA);
    } else { *((volatile unsigned char*)(PADDR_MFP1 + MFP_GPDR)) &= ~(1 << MFP_BIT_DTA); }
}
static inline void i2cSetClk(uint8_t v) {
    if (v) { *((volatile uint8_t*)(PADDR_MFP1 + MFP_GPDR)) |= (1 << MFP_BIT_CLK);
    } else   { *((volatile uint8_t*)(PADDR_MFP1 + MFP_GPDR)) &= ~(1 << MFP_BIT_CLK); }
}
static inline void i2cDelay(int num) {
   for (int i=0; i<(num * I2C_DELAY); i++) {
    __asm__ __volatile__( " nop\n\t" : : : "cc" );
   }
}


bool i2c_Init()
{
    // mfp1 is already inited to the state we want
    i2cLocked = false;
    return true;
}

bool i2c_Aquire()
{
    if (!cpu_Lock(&i2cLocked))
        return false;

    i2cOldMfp09 = *((volatile unsigned char*)(PADDR_MFP1 + MFP_IERB));
    i2cOldMfp07 = *((volatile unsigned char*)(PADDR_MFP1 + MFP_IERA));
    *((volatile unsigned char*)(PADDR_MFP1 + MFP_IERB)) &= ~(1<<3);
    *((volatile unsigned char*)(PADDR_MFP1 + MFP_IERA)) &= ~(1<<7);
    i2cSetClk(1); i2cDirClk(1);
    i2cSetDta(1); i2cDirDta(1);
    return true;
}

void i2c_Release()
{
    *((volatile unsigned char*)(PADDR_MFP1 + MFP_IERB)) &= ~(1<<3);
    *((volatile unsigned char*)(PADDR_MFP1 + MFP_IERA)) &= ~(1<<7);
    *((volatile unsigned char*)(PADDR_MFP1 + MFP_DDR))  &= ~(MFP_BITMASK);
    *((volatile unsigned char*)(PADDR_MFP1 + MFP_IERB)) = i2cOldMfp09;
    *((volatile unsigned char*)(PADDR_MFP1 + MFP_IERA)) = i2cOldMfp07;
    cpu_Unlock(&i2cLocked);
}

void i2c_Start()
{
    i2cSetDta(1); i2cDirDta(1); i2cDelay(1);
    i2cSetClk(1); i2cDelay(1);
    i2cSetDta(0); i2cDelay(1);
    i2cSetClk(0); i2cDelay(2);
}

void i2c_Stop()
{
    i2cSetDta(0); i2cDirDta(1); i2cDelay(1);
    i2cSetClk(1); i2cDelay(1);
    i2cSetDta(1); i2cDelay(2);
}

uint8_t i2c_Read(uint8_t ack)
{
    uint8_t val = 0;
    i2cDirDta(0);
    i2cDelay(1);
    for (short i = 0; i< 8; i++) {
        val = val << 1;
        i2cSetClk(1);
        i2cDelay(2);
        val |= i2cGetDta();
        i2cSetClk(0);
        i2cDelay(2);
    }
    i2cSetDta(ack);
    i2cDirDta(1);
    i2cSetClk(1);
    i2cDelay(2);
    i2cSetClk(0);
    i2cDelay(1);
    return val;
}

uint8_t i2c_Write(uint8_t val)
{
    i2cDirDta(1);
    for (short i = 0; i < 8; i++) {
        i2cSetDta(val & 0x80);
        i2cDelay(1);
        i2cSetClk(1);
        i2cDelay(2);
        i2cSetClk(0);
        i2cDelay(1);
        val = val << 1;
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
