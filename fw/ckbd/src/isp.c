//---------------------------------------------------------------------
// isp.c
// in-system programming
//
// 1kb code block at location 0x400-0x800
// Do not call code outside of this file during or after flashing.
// It is ok to trash memory since we will reset after.
//---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"

// port1 is connected to host
// port0 is debug port
#if defined(BOARD_DEVKIT)
#define ISP_DEBUG_PORT   1
#else
#define ISP_DEBUG_PORT   0
#endif

#if 1
extern __xdata uint8_t MemPool[];
#define isp_flash_buffer ((__xdata uint8_t*)&MemPool[0])
#else
__at(0x400) __xdata uint8_t isp_flash_buffer[1024];
#endif

extern void IspDelayUs(uint16_t n);
extern void IspDelayMs(uint16_t n);


static inline bool IspPeekUart(void) {
    return ((SER1_LSR & bLSR_DATA_RDY) != 0);
}

static inline uint8_t IspRecvUart(void) {
    while (!IspPeekUart());
    return SER1_RBR;
}

static inline void IspSendUart(uint8_t b) {
    while ((SER1_LSR & bLSR_T_ALL_EMP) == 0);
    SER1_THR = b;
}


#ifdef TRACE
#undef TRACE
#endif
#if defined(ISP_DEBUG_PORT) && ((ISP_DEBUG_PORT == 0) || (ISP_DEBUG_PORT == 1))
void IspPrint(const char* str) {
    while (1) {
        uint8_t b = *str++;
        if (b == 0) {
            return;
        }
#if (ISP_DEBUG_PORT == 0)
        while (!TI);
        TI = 0;
        SBUF = b;
#elif (ISP_DEBUG_PORT == 1)
        while ((SER1_LSR & bLSR_T_ALL_EMP) == 0 );
        SER1_THR = b;
#endif        
    }
}
#define TRACE(...) IspPrint(__VA_ARGS__)
#else
#define TRACE(...) { }
#endif



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

    // disable uart fifo
    SER1_FCR &= ~bFCR_FIFO_EN;
    SER1_FCR |= bFCR_T_FIFO_CLR | bFCR_R_FIFO_CLR;

    TRACE("\n\n--[ IspMain ]--\n");

    // loop
        // send "get" message
        // get "put" message
            // get 1kb from uart
            // (ignore if block 0x400-0x7ff)
            // erase block
            // flash block
            // verify
        // until "done" or timeout            

    // send "done" message
    
    // reset
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bSW_RESET;
}


void IspDelayUs(uint16_t n)
{
	while (n)
	{				// total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
		++SAFE_MOD; // 2 Fsys cycles, for higher Fsys, add operation here
#if FREQ_SYS >= 14000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 16000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 18000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 20000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 22000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 24000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 26000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 28000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 30000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 32000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 34000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 36000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 38000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 40000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 42000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 44000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 46000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 48000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 50000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 52000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 54000000
		++SAFE_MOD;
#endif
#if FREQ_SYS >= 56000000
		++SAFE_MOD;
#endif
		--n;
	}
}

void IspDelayMs(uint16_t n)
{
    while (n) {
        IspDelayUs(1000);
        --n;
    }
}
