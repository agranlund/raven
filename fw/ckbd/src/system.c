//-------------------------------------------------------------------------
// system.c
// misc system utilities
//-------------------------------------------------------------------------
#include "system.h"

__xdata volatile uint32_t msnow = 0;
__xdata volatile uint16_t SoftWatchdog = 0;

//-------------------------------------------------------------------------------
// uart0 (debug)
//-------------------------------------------------------------------------------
#if defined(DEBUG)
void uart0init(uint32_t bps) {
    P3_DIR |= bTXD;
    P3_PU |= bTXD;
    P3 |= bTXD;
    SM0 = 0;
    SM1 = 1;
    SM2 = 0;
    RCLK = 0;
    TCLK = 0;
    PCON |= SMOD;
    uint32_t x = 10 * FREQ_SYS / bps / 16;
    uint8_t x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x++;
    TMOD = TMOD & ~ bT1_GATE & ~ bT1_CT & ~ MASK_T1_MOD | bT1_M1;
    T2MOD = T2MOD | bTMR_CLK | bT1_CLK;
    TH1 = 0-x;
    TR1 = 1;
	REN = 1;
	TI = 1;
}
void uart0send(uint8_t c) {
    while (!TI);
    TI = 0;
    SBUF = c;
}
uint8_t uart0recv(void) {
    while(!RI);
    RI = 0;
    return SBUF;
}
#endif

//-------------------------------------------------------------------------------
// uart1 (ikbd)
//-------------------------------------------------------------------------------
void uart1init(uint32_t bps) {
    SER1_LCR |= bLCR_DLAB; // DLAB bit is 1, write DLL, DLM and DIV registers
    SER1_DIV = 1; // Prescaler
    uint32_t x = 10 * FREQ_SYS * 2 / 1 / 16 / bps;
    uint8_t x2 = x % 10;
    x /= 10;
    if (x2 >= 5) x++; // Rounding
    SER1_DLM = x>>8;
    SER1_DLL = x & 0xff;
    SER1_LCR &= ~bLCR_DLAB; // DLAB bit is 0 to prevent modification of UART1 baud rate and clock
    XBUS_AUX |= bALE_CLK_EN; // Close RS485 mode RS485_EN = 0, can not be omitted
    SER1_LCR = (SER1_LCR & ~MASK_U1_WORD_SZ) | ((8 - 5) & MASK_U1_WORD_SZ); // Line control, 5, 6, 7 or 8 databits
    SER1_LCR &= ~(bLCR_PAR_EN | bLCR_STOP_BIT); // Wireless path interval, no parity, 1 stop bit
    SER1_MCR &= ~bMCR_TNOW;
    SER1_IER |= ((2 << 4) & MASK_U1_PIN_MOD); // pin mode 2
    P2_DIR |= bTXD1;
    P2_PU |= bTXD1;
    SER1_ADDR |= 0xff; //Close multi-machine communication
}
void uart1send(uint8_t c) {
    while ((SER1_LSR & bLSR_T_ALL_EMP) == 0 );
    SER1_THR = c;
}
uint8_t uart1recv(void)
{
    while((SER1_LSR & bLSR_DATA_RDY) == 0);
    return SER1_RBR;
}

//-------------------------------------------------------------------------------
// called by stdlib
//-------------------------------------------------------------------------------
int putchar(int c)
{
#if defined(DEBUG)
    uart0send((c & 0xff));
#endif
	return c;
}

//-------------------------------------------------------------------------------
// called by stdlib
//-------------------------------------------------------------------------------
int getchar(void)
{
    return 0;
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

    if (bootloader) {
        static void(* __data CH559BootLoader)(void) = 0xF400;
        CH559BootLoader();
    }

    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bSW_RESET;
}
