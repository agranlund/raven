//---------------------------------------------------------------------
// settings.c
// user settings stored in flash
//---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "usbhost.h"
#include "ikbd.h"
#include "temps.h"
#include "settings.h"

#define SETTINGS_MAGIC   0x434B4244      /* CKBD */
#define SETTINGS_VERSION 0x25060101

__xdata settings_t Settings;

#define FlashAddress    0xF000
#define FlashSize       1024
#define FlashSettings   ((__code settings_t*)FlashAddress)        /* 1024 bytes */

bool SyncSettings(void)
{
    TRACE("Erasing flash");
    if(CH559EraseDataFlash(FlashAddress) != 0) {
        TRACE("Flash erase failed");
        return false;
    }
    TRACE("Writing flash : %d bytes", sizeof(settings_t));
    if (CH559WriteDataFlash(FlashAddress, (uint8_t *)&Settings, sizeof(settings_t)) != 0) {
        TRACE("Flash write failed");
        return false;
    }
    return true;
}

void InitSettings(bool SafeMode)
{
    // magic value not present (or we're in safe mode), initialize flash data
    if (SafeMode || (FlashSettings->Magic != SETTINGS_MAGIC) || (FlashSettings->Version != SETTINGS_VERSION))
    {
        TRACE("Initializing settings");
        memset(&Settings, 0x00, sizeof(settings_t));
        Settings.Magic = SETTINGS_MAGIC;
        Settings.Version = SETTINGS_VERSION;
        Settings.BaudDebug = BAUD_DEBUG;
        Settings.BaudIkbd = BAUD_IKBD;

        Settings.UsbMouseScale = FIXED16(1.0);
        Settings.UsbKeyboardReportMode = 1;
        Settings.UsbMouseReportMode = 1;

        Settings.PS2MouseScale = FIXED16(1.0);

        Settings.LegacyMouseAmiga = 0;
        Settings.LegacyMouseScale = FIXED16(2.0);


        // default mouse settings
        const settings_eiffel_mouse_t defaultEiffelMouse = {
            IKBD_KEY_WHEELUP,
            IKBD_KEY_WHEELDN,
            IKBD_KEY_WHEELLT,
            IKBD_KEY_WHEELRT,
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
        SyncSettings();
    }
    else
    {
        memcpy(&Settings, FlashSettings, sizeof(settings_t));
    }
}
