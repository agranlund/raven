#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "system.h"

typedef struct
{
    uint8_t WheelUp;
    uint8_t WheelDown;
    uint8_t WheelLeft;
    uint8_t WheelRight;
    uint8_t Button3;
    uint8_t Button4;
    uint8_t Button5;
    uint8_t WheelRepeat;
} settings_eiffel_mouse_t;

typedef struct
{
    uint8_t High;
    uint8_t Low;
    uint8_t Rctn[12];
    uint8_t Temp[12];
} settings_eiffel_temp_t;

typedef struct
{
    uint32_t Magic;
    uint32_t Version;
    uint32_t Changed;

    uint32_t BaudDebug;
    uint32_t BaudIkbd;

    fuint16_t   PS2MouseScale;
    fuint16_t   UsbMouseScale;
    fuint16_t   LegacyMouseScale;

    uint8_t UsbKeyboardReportMode : 1;      // usb keyboard boot or report mode
    uint8_t UsbMouseReportMode : 1;         // usb mouse boot or report mode
    uint8_t LegacyMouseAmiga : 1;           // atari or amiga mouse

    uint8_t CoreTempShutdown;               // abbbbbbb. a = enable, b = temp (0-127)
    uint8_t FanControl0 : 2;
    uint8_t FanControl1 : 2;
    settings_eiffel_temp_t EiffelTemp[2];   // eiffel temperature tables
    settings_eiffel_mouse_t EiffelMouse;    // eiffel mouse settings
    uint8_t EiffelKeymap[0x90];             // eiffel keyboard usertable

} settings_t;


extern __xdata settings_t Settings;
extern void InitSettings(bool SafeMode);
extern bool SyncSettings(bool force);

#endif
