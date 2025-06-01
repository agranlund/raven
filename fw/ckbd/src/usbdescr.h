#ifndef _USBDESCR_H_
#define _USBDESCR_H_
#include "system.h"
#include "usbhost.h"

// maps a joystick control to a keyboard or mouse output

#define MAP_KEYBOARD    0
#define MAP_MOUSE       1

#define MAP_MOUSE_BUTTON1   1
#define MAP_MOUSE_BUTTON2   2
#define MAP_MOUSE_BUTTON3   3
#define MAP_MOUSE_BUTTON4   4
#define MAP_MOUSE_BUTTON5   5
#define MAP_MOUSE_X         4
#define MAP_MOUSE_Y         5
#define MAP_MOUSE_WHEEL     6

#define MAP_TYPE_NONE               0
#define MAP_TYPE_THRESHOLD_BELOW    1
#define MAP_TYPE_THRESHOLD_ABOVE    2
#define MAP_TYPE_SCALE              3
#define MAP_TYPE_ARRAY              4
#define MAP_TYPE_BITFIELD           5
#define MAP_TYPE_EQUAL              6

#define INPUT_PARAM_SIGNED              1
#define INPUT_PARAM_SIGNED_SCALEDOWN    2

#define JOYPRESETCOUNT  18

typedef struct _JoyPreset
{
	uint8_t Number;
	uint8_t InputUsagePage;
	uint32_t InputUsage;
	// Mouse or keyboard
	uint8_t OutputChannel;
	// for keyboard, this is the HID scancode of the key associated with this control
	// for mouse, this is one of the values of MAP_MOUSE_x
	uint8_t OutputControl;
	// How this control gets interpreted - MAP_TYPE_x
	uint8_t InputType;
	// Param has different meanings depending on InputType
	uint16_t InputParam;
	// linked list
	struct JoyPreset *next;
} JoyPreset;

// defines a mapping between a HID segment and an output event
typedef struct _HID_SEG
{
	uint8_t index;
	uint16_t startBit;
	// Mouse or keyboard
	uint8_t OutputChannel;
	// for keyboard, this is the HID scancode of the key associated with this control
	// for mouse, this is one of the values of MAP_MOUSE_x
	uint8_t OutputControl;
	// How this control gets interpreted - MAP_TYPE_x
	uint8_t InputType;
	// Param has different meanings depending on InputType
	uint16_t InputParam;
	uint8_t reportSize;
	uint8_t reportCount;
	uint32_t value;
	struct HID_SEG *next;
} HID_SEG;

typedef struct _HID_MOUSE
{
    int16_t x, y, z;
    uint8_t b;
} HID_MOUSE;

typedef struct _HID_REPORT
{
	uint16_t appUsage;
	uint16_t appUsagePage;
	uint16_t length;
	bool KeyboardUpdated;
    bool MouseUpdated;
	// bit map for currently pressed keys (0-256)
    uint8_t KeyboardKeyMap[32];
	uint8_t oldKeyboardKeyMap[32];
    HID_MOUSE Mouse;
    HID_MOUSE oldMouse;
	__xdata UsbLinkedList *HidSegments;
} HID_REPORT;

extern BOOL ParseDeviceDescriptor(USB_DEV_DESCR *pDevDescr, UINT8 len, USB_HUB_PORT *pUsbDevice);
extern BOOL ParseConfigDescriptor(USB_CFG_DESCR *pCfgDescr, UINT16 len, USB_HUB_PORT *pUsbDevice);
extern BOOL ParseReportDescriptor(uint8_t *pDescriptor, UINT16 len, INTERFACE *pHidSegStruct);

#endif /* _USBPARSE_H_ */

