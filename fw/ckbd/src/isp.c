//-------------------------------------------------------------------------
// isp.c
// in-system programming
//-------------------------------------------------------------------------
//
// IspMain() lives in the normal code area and is called first.
// Here we flash the 1kb ISP area at $400
//
// Once done, we jump to IspMain1() which is located at $400
// This takes care of flashing the rest of the rom.
// When everything is done we perform a hard reset.
//
// Do not call code outside of this file during or after flashing.
// It is ok to trash memory since we will reset after.
//
// Be careful that no standard library stuff is called on accident
// double check the assembly output if in doubt.
//
//-------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"

extern __xdata uint8_t MemPool[];
#define isp_flash_buffer ((__xdata uint8_t*)&MemPool[0])

static uint8_t IspRecvUart(void);
static void IspSendUart(uint8_t b);
static bool IspFlashBlock(uint16_t block);

#if defined(DEBUG)
#define IspPut(c) { while (!TI); TI = 0; SBUF = c; }
#else
#define IspPut(...) { }
#endif

//-------------------------------------------------------------------
// Entry point for second stage inside ISP area
// IspMain1() must be the first code and located at addess $400
//-------------------------------------------------------------------
#if defined(ISPCODE)
void IspMain1(void)
{
    for (uint16_t block = 0; block <= 64; block++) {
        if (block != 1) {   // ignore our own block
            if (!IspFlashBlock(block)) {
                break;
            }
        }
    }
    IspPut('\n'); IspPut('d'); IspPut('o'); IspPut('n'); IspPut('e'); IspPut('\n');

    // write protect code area
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~(bCODE_WE | bDATA_WE);
    SAFE_MOD = 0x00;

    // hard reset
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bSW_RESET;
    SAFE_MOD = 0x00;
}
#endif


//-------------------------------------------------------------------
// Entry point in regular code area, can be anywhere >= $800
//-------------------------------------------------------------------
#if !defined(ISPCODE)
void IspMain(void)
{
    // disable all interrupts
    EA = 0;

    // disable watchdog
    WDOG_COUNT = 0;
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~bWDOG_EN;
    WDOG_COUNT = 0;

    // disable and clear uart fifo
    SER1_FCR &= ~bFCR_FIFO_EN;
    SER1_FCR |= bFCR_T_FIFO_CLR | bFCR_R_FIFO_CLR;

    // write enable code area
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= (bCODE_WE | bDATA_WE);
    SAFE_MOD = 0x00;

    IspPut('\n'); IspPut('I'); IspPut('S'); IspPut('P'); IspPut('\n');

    // first we flash the isp area
    if (!IspFlashBlock(1)) {
        return;
    }

    // then we run the isp code to flash all other areas of the rom
    extern void IspMain1();
    IspMain1();
}
#endif


//-------------------------------------------------------------------
// shared code, must be static so isp0.rel and isp1.rel
// both get their own unique copies of these functions
//-------------------------------------------------------------------
static uint8_t IspRecvUart(void) {
    while ((SER1_LSR & bLSR_DATA_RDY) == 0);
    return SER1_RBR;
}

static void IspSendUart(uint8_t b) {
    while ((SER1_LSR & bLSR_T_ALL_EMP) == 0);
    SER1_THR = b;
}

static bool IspFlashBlock(uint16_t block) {
    IspPut('.');
    // send request
    IspSendUart((uint8_t)block);
    if (block != (uint16_t)IspRecvUart()) {
        return false;
    }
    // send ack
    IspSendUart((uint8_t)block);
    // and receive the data
    __xdata uint8_t* buf = isp_flash_buffer;
    for (int i=0; i<1024; i++) {
        buf[i] = IspRecvUart();
    }

    // erase block
    ROM_ADDR = (block << 10);
    ROM_CTRL = ROM_CMD_ERASE;
    while (!(ROM_STATUS & bROM_ADDR_OK));
    // program block
    for (int i=0; i<1024; i+=2) {
        uint16_t w = buf[i+1];
        w <<= 8; w += buf[i];
        ROM_ADDR = (block << 10) + i;
        ROM_DATA = w;
        ROM_CTRL = ROM_CMD_PROG;
        while (!(ROM_STATUS & bROM_ADDR_OK));
    }
    return true;
}

