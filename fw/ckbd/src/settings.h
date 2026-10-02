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
/* 0x00 */  uint32_t Magic;
/* 0x04 */  uint32_t Version;
/* 0x08 */  uint32_t Changed;
            uint8_t pad0C[4];
/* 0x10 */  fuint16_t PS2MouseScale;
/* 0x12 */  fuint16_t PS2WheelScale;
/* 0x14 */  fuint16_t UsbMouseScale;
/* 0x16 */  fuint16_t UsbWheelScale;
/* 0x18 */  fuint16_t LegacyMouseScale;
            uint8_t pad1A[6];
/* 0x20 */  uint8_t UsbKeyboardReportMode;          // usb keyboard boot or report mode
/* 0x21 */  uint8_t UsbMouseReportMode;             // usb mouse boot or report mode
/* 0x22 */  uint8_t LegacyMouseAmiga;               // atari or amiga mouse
/* 0x23 */  uint8_t CoreTempShutdown;               // abbbbbbb. a = enable, b = temp (0-127)
/* 0x24 */  uint8_t FanControl0;
/* 0x25 */  uint8_t FanControl1;
/* 0x26 */  uint8_t FanThreshold0;
/* 0x27 */  uint8_t FanThreshold1;
/* 0x28 */  uint8_t FanMinSpeed0;
/* 0x29 */  uint8_t FanMaxSpeed0;
/* 0x2a */  uint8_t FanMinSpeed1;
/* 0x2b */  uint8_t FanMaxSpeed1;
/* 0x2c */	uint8_t FanRgbI;
/* 0x2d */	uint8_t FanRgbR;
/* 0x2e */	uint8_t FanRgbG;
/* 0x2f */	uint8_t FanRgbB;

/* 0x30 */  settings_eiffel_mouse_t EiffelMouse;    // eiffel mouse settings
/* 0x38 */  settings_eiffel_temp_t EiffelTemp[2];   // eiffel temperature tables
/* 0x6C */  uint8_t EiffelKeymap[0x90];             // eiffel keyboard usertable

} settings_t;


extern __xdata settings_t Settings;
extern void InitSettings(bool SafeMode);
extern bool SyncSettings(bool force);

#endif
