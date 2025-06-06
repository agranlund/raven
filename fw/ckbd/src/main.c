//---------------------------------------------------------------------
// CKBD
// Eiffel + IKBD emulation on CH559
//---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "settings.h"
#include "ps2.h"
#include "joyport.h"
#include "temps.h"
#include "ikbd.h"
#include "usbhost.h"
#include "usbhidkeys.h"
#include "keyboardled.h"

int main(void)
{
    P1_DIR  = 0b00000000;
    P1_PU   = 0b00000000;
    P2_DIR  = 0b00000000;
    P2_PU   = 0b00000000;
    P3_DIR  = 0b00000000;
    P3_PU   = 0b00000000;
    P3      = 0b00000000;
    P4_DIR  = 0b00000000;
    P4_PU   = 0b01000000;   // p4.6 (prog)
    P4_OUT  = 0b00000000;
    PORT_CFG = bP1_OC | bP2_OC/* | bP3_OC*/;

    // clock setup
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
#if defined(OSC_EXTERNAL)
	CLOCK_CFG |= bOSC_EN_XT;
	CLOCK_CFG &= ~bOSC_EN_INT;
#endif
	CLOCK_CFG &= ~MASK_SYS_CK_DIV;
	CLOCK_CFG |= 6;
	PLL_CFG = (24 << 0) | (6 << 5);
    delayms(500);

    // serial debug on uart0
#if defined(DEBUG)
    uart0init(BAUD_DEBUG);
    TRACE("\n\n--[ CKBD ]--\n");
#endif

    // init settings
    TRACE("Init settings");
    InitSettings(false);

    TRACE("Init usb");
	InitUsbData();
	InitUsbHost();
#if !defined(DISABLE_PS2)
    TRACE("Init ps2");
    InitPS2();
#endif

    TRACE("Init joyport");
    InitJoyport();

    TRACE("Init temperature");
    InitTemps();

    TRACE("InitIkbd");
    InitIkbd();

	// timer0 setup
    TRACE("Init timer");
	TMOD = (TMOD & 0xf0) | 0x02;    // mode 1 (8bit auto reload)
	TH0 = 0xBD;					    // 60khz
	TR0 = 1;                        // start timer0
	ET0 = 1;                        // enable timer0 interrupt

	// enable watchdog
    TRACE("Init watchdog");
	WDOG_COUNT = 0x00;
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bWDOG_EN;
	WDOG_COUNT = 0x00;

    // enable interrupts
    TRACE("Gotime!");
	EA = 1;

    while(1)
    {
        // keepalive
        SoftWatchdog = 0;

        // handle all inputs
		ProcessUsbHostPort();
        #if !defined(DISABLE_PS2)
        ProcessPS2();
        #endif
        ProcessJoyport();
		ProcessKeyboardLed();
        ProcessTemps();
        ProcessIkbd();

        // sync settings when required
        SyncSettings(false);
    }
}

// ---------------------------------------------------------------------------------
// timer0 : 60khz : 48MHz divided by (0xFFFF - (TH0TL0))
// ---------------------------------------------------------------------------------
void mTimer0Interrupt(void) __interrupt(INT_NO_TMR0)
{
    JoyportInterrupt();

    static uint8_t tm0counter = 0;
	if (++tm0counter == 60) {
		tm0counter = 0;
        msnow++;

        // Soft watchdog is to get around the fact that the real watchdog runs too fast
        SoftWatchdog++;
        if (SoftWatchdog > 5000) {
            // if soft watchdog overflows, just go into an infinite loop and we'll trigger the real watchdog
            TRACE("Soft overflow");
            while(1);
        }

        // otherwise, reset the real watchdog
        WDOG_COUNT = 0x00;

        // every 4 milliseconds (250hz), check one or the other USB port (so each gets checked at 125hz)
        static uint8_t UsbUpdateCounter = 0;
        if (UsbUpdateCounter == 4) {
            s_CheckUsbPort0 = TRUE;
        } else if (UsbUpdateCounter == 8) {
            s_CheckUsbPort1 = TRUE;
            UsbUpdateCounter = 0;
        }
        UsbUpdateCounter++;
	}
}
