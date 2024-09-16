#include "sys.h"
#include "raven.h"
#include "hw/uart.h"
#include "hw/ikbd.h"
#include "hw/midi.h"
#include "hw/i2c.h"
#include "hw/rtc.h"

void b_dbg_GPO(int num, int val)
{
    switch (num)
    {
        case 0:     // uart1:powerled
        case 1:     // uart1:TP301
            ikbd_GPO(num, val);
            break;
    }
}

static void     b_rtc_Read(uint32_t addr, uint8_t* buf, uint32_t siz)       { rtc_Read((uint8_t)addr, buf, (uint8_t)siz); }
static void     b_rtc_Write(uint32_t addr, uint8_t* buf, uint32_t siz)      { rtc_Write((uint8_t)addr, buf, (uint8_t)siz); }

static int32_t  b_i2c_Aquire()                                              { return (int32_t) i2c_Aquire(); }
static void     b_i2c_Release()                                             { i2c_Release(); }
static void     b_i2c_Start()                                               { i2c_Start(); }
static void     b_i2c_Stop()                                                { i2c_Stop(); }
static uint32_t b_i2c_Read(uint32_t ack)                                    { return (uint32_t) i2c_Read((uint8_t)ack); }
static uint32_t b_i2c_Write(uint32_t val)                                   { return (uint32_t) i2c_Write((uint8_t)val); }

const raven_t ravenBios =
{
//0x0000
    C_RAVN,             // magic
    VERSION,            // rom version
    {0,0,0,0,0,0},
//0x0020
    b_dbg_GPO,
    {0,0,0,0,0,0,0},
//0x0040
    b_rtc_Read,
    b_rtc_Write,
    {0,0,0,0,0,0},
//0x0060
    b_i2c_Aquire,
    b_i2c_Release,
    b_i2c_Start,
    b_i2c_Stop,
    b_i2c_Read,
    b_i2c_Write,
    {0,0},
//0x0080
};
