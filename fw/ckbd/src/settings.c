//-------------------------------------------------------------------------
// settings.c
// user settings stored in flash
//-------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "usbhost.h"
#include "ikbd.h"
#include "temps.h"
#include "settings.h"

#define SETTINGS_MAGIC   0x434B4244      /* CKBD */
#define SETTINGS_VERSION 0x25060207

__xdata settings_t Settings;

#define FlashAddress    0xF000
#define FlashSize       1024
#define FlashSettings   ((__code settings_t*)FlashAddress)        /* 1024 bytes */

#define AutoSyncDelay   2000

static uint32_t synctime;
static uint32_t syncto;

bool SyncSettings(bool force)
{
    // sync if changed, or forced
    if (!force) {
        if (Settings.Changed == FlashSettings->Changed) {
            return true;
        }
        if (Settings.Changed != syncto) {
            // each change resets the timer
            syncto = Settings.Changed;
            synctime = msnow;
            return true;
        } else {
            // sync when x time has passed since last change
            if (elapsed(synctime) < AutoSyncDelay) {
                return true;
            }
        }
    }

    // disable all interrupts
    EA = 0;

    // unlock data area
    SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bDATA_WE;
    SAFE_MOD = 0x00;

    // erase
    TRACE("Erasing flash");
    uint16_t written = 0;
	ROM_ADDR = FlashAddress;
    if (ROM_STATUS & bROM_ADDR_OK) {
        ROM_CTRL = ROM_CMD_ERASE;
        if (((ROM_STATUS ^ bROM_ADDR_OK) & 0x7F) == 0) {
            // program
            TRACE("Programming flash");
            uint8_t* buf = (uint8_t*)&Settings;
            for(int i=0; i<sizeof(settings_t); i+=2)
            {
                uint16_t w = buf[i+1];
                w <<= 8; w += buf[i];
                ROM_ADDR = FlashAddress + i;
                ROM_DATA = w;
                if (ROM_STATUS & bROM_ADDR_OK) {
                    ROM_CTRL = ROM_CMD_PROG;
                    if (((ROM_STATUS ^ bROM_ADDR_OK) & 0x7F) == 0) {
                        written += 2;
                    }
                }
            }
        }
    }

    // lock data area
    SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~bDATA_WE;
    SAFE_MOD = 0x00;

    // enable interrupts
    EA = 1;

    if (written < sizeof(settings_t)) {
        TRACE("Failed (%d / %d)", written, sizeof(settings_t));
        return false;
    }
    TRACE("Done");
    return true;
}

void InitSettings(bool SafeMode)
{
    // magic value not present (or we're in safe mode), initialize flash data
    if (SafeMode || (FlashSettings->Magic != SETTINGS_MAGIC) || (FlashSettings->Version != SETTINGS_VERSION))
    {
        TRACE("Defaulting settings");
        memset(&Settings, 0x00, sizeof(settings_t));
        Settings.Magic = SETTINGS_MAGIC;
        Settings.Version = SETTINGS_VERSION;
        Settings.Changed = 0;

        Settings.UsbMouseScale = FIXED16(1.0);
        Settings.UsbWheelScale = FIXED16(1.0);
        Settings.UsbKeyboardReportMode = 1;
        Settings.UsbMouseReportMode = 1;

        Settings.PS2MouseScale = FIXED16(1.0);
        Settings.PS2WheelScale = FIXED16(1.0);

        Settings.LegacyMouseAmiga = 0;
        Settings.LegacyMouseScale = FIXED16(2.0);

        // default mouse settings
        const settings_eiffel_mouse_t defaultEiffelMouse = {
#if 1
            // Since EmuTOS GEM has disabled support for Eiffel wheel, and NAES does not support
            // them, we're going to set arrow keys as default wheel behavior.
            // This can be reprogrammed to the Eiffel defaults using the Eiffel control panel
            // if needed (XaAES has native and working support for Eiffel wheel scancodes)
            // There are supposedly TSR's that can translate Eiffel wheel codes into VDI scroll events.
            IKBD_KEY_UP,
            IKBD_KEY_DOWN,
            IKBD_KEY_LEFT,
            IKBD_KEY_RIGHT,
#else
            // eiffel defaults
            IKBD_KEY_WHEELUP,
            IKBD_KEY_WHEELDN,
            IKBD_KEY_WHEELLT,
            IKBD_KEY_WHEELRT,
#endif            
            IKBD_KEY_BUTTON3,
            IKBD_KEY_BUTTON4,
            IKBD_KEY_BUTTON5,
            3
        };

        // default temp sensors
        Settings.FanControl0 = FANCONTROL_AUTO; // board
        Settings.FanControl1 = FANCONTROL_ON;   // core
        Settings.CoreTempShutdown = (0 << 7) | 80;
        const settings_eiffel_temp_t defaultEiffelTemp[2] = {
            {   // temp1 (case)
                40, 35,
                { 27,  33,  39,  47,  56,  68,  82, 100, 120, 150, 180, 220},   // rctn
                { 64,  57,  52,  47,  41,  36,  30,  25,  20,  15,  11,   6}    // temp
            },
            {   // temp2 (cpu)
                40, 35,
                { 27,  33,  39,  47,  56,  68,  82, 100, 120, 150, 180, 220},   // rctn
                { 64,  57,  52,  47,  41,  36,  30,  25,  20,  15,  11,   6}    // temp
            },
        };

        memcpy((void*)&Settings.EiffelMouse,   (void*)&defaultEiffelMouse,   sizeof(settings_eiffel_mouse_t));
        memcpy((void*)&Settings.EiffelTemp[0], (void*)&defaultEiffelTemp[0], sizeof(settings_eiffel_temp_t));
        memcpy((void*)&Settings.EiffelTemp[1], (void*)&defaultEiffelTemp[1], sizeof(settings_eiffel_temp_t));
        SyncSettings(true);
    }
    else
    {
        memcpy(&Settings, FlashSettings, sizeof(settings_t));
    }
    syncto = Settings.Changed;
    synctime = msnow;
}
