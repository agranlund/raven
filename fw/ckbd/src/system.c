//---------------------------------------------------------------------
// system.c
// misc system utilities
//---------------------------------------------------------------------
#include "system.h"

__xdata volatile uint32_t msnow = 0;
__xdata volatile uint16_t SoftWatchdog = 0;

//-------------------------------------------------------------------------------
// debug leds
//-------------------------------------------------------------------------------
#if defined(DEBUG) && defined(BOARD_DEVKIT)
void dbg_led(UINT8 led, UINT8 onoff)
{
    switch (led)
    {
        case 0:         // p4.0
            P4_OUT = (P4_OUT & ~(1<<0)) | ((onoff & 1) << 0);
            break;
        case 1:         // p4.4
            P4_OUT = (P4_OUT & ~(1<<4)) | ((onoff & 1) << 4);
            break;
    }
}
#endif

//-------------------------------------------------------------------------------
// called by stdlib
//-------------------------------------------------------------------------------
int putchar(int c)
{
#if defined(DEBUG)
    CH559UART0SendByte((c & 0xff));
#endif
	return c;
}

//-------------------------------------------------------------------------------
// called by stdlib
//-------------------------------------------------------------------------------
int getchar(void)
{
#if defined(DEBUG) && defined(BOARD_DEVKIT)
    return (int)CH559UART0RcvByte();
#else
    return 0;
#endif
}


//-------------------------------------------------------------------------------
// elapsed timer
//-------------------------------------------------------------------------------
uint32_t elapsed(uint32_t from) {
    return (msnow >= from) ? (msnow - from) : (1 + msnow + (0xfffffffful - from));
}

//-------------------------------------------------------------------------------
// microsecond delay
//-------------------------------------------------------------------------------
void delayus(uint16_t n) // ��uSΪ��λ��ʱ
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

//-------------------------------------------------------------------------------
// millisecond delay
//-------------------------------------------------------------------------------
void delayms(uint16_t n)
{
	SoftWatchdog = 0;
	while (n--) {
		delayus(1000);
	}
	SoftWatchdog = 0;
}

//-------------------------------------------------------------------------------
// reset / bootloader
//-------------------------------------------------------------------------------
void reset(bool bootloader)
{
#if defined(DEBUG)
    TRACE("System reset");
    if (bootloader) {
        TRACE("--> WCH bootloader");
    }
#endif
    EA = 0;
    WDOG_COUNT = 0;
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG &= ~bWDOG_EN;
    WDOG_COUNT = 0;

    DEBUGLED(0,0);
    DEBUGLED(1,0);

    if (bootloader) {
        static void(* __data CH559BootLoader)(void) = 0xF400;
        CH559BootLoader();
    }

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bSW_RESET;
}
