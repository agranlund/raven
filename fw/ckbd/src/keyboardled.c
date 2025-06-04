//-------------------------------------------------------------------------
// keyboardled.c
// shared interface for ps2 + usb keyboard leds
//-------------------------------------------------------------------------
#include "system.h"
#include "usbhost.h"
#include "keyboardled.h"
#include "usbhidkeys.h"
#include "ps2.h"


static uint8_t keyboardLed = 0x00;
static uint8_t oldLed = 0x00;

static inline uint8_t ps2_to_hid(uint8_t led) {
    uint8_t ret = 0;
    if (led & PS2_KEY_LED_SCROLLLOCK) {
        ret |= HID_KEY_LED_SCROLLLOCK;
    }
    if (led & PS2_KEY_LED_NUMLOCK) {
        ret |= HID_KEY_LED_NUMLOCK;
    }
    if (led & PS2_KEY_LED_CAPSLOCK) {
        ret |= HID_KEY_LED_CAPSLOCK;
    }
    return ret;
}

static inline uint8_t hid_to_ps2(uint8_t led) {
    uint8_t ret = 0;
    if (led & HID_KEY_LED_SCROLLLOCK) {
        ret |= PS2_KEY_LED_SCROLLLOCK;
    }
    if (led & HID_KEY_LED_NUMLOCK) {
        ret |= PS2_KEY_LED_NUMLOCK;
    }
    if (led & HID_KEY_LED_CAPSLOCK) {
        ret |= PS2_KEY_LED_CAPSLOCK;
    }
    return ret;
}

void SetKeyboardLedStatus(uint8_t hidled)
{
	HAL_CRITICAL_STATEMENT(keyboardLed = hidled);
}

void SetKeyboardLedStatusFromPS2(UINT8 ps2led)
{
	SetKeyboardLedStatus(ps2_to_hid(ps2led));
}

UINT8 GetKeyboardLedStatus(void)
{
	UINT8 led;
	HAL_CRITICAL_STATEMENT(led = keyboardLed);
	return led;
}

void ProcessKeyboardLed(void) {
	uint8_t led = GetKeyboardLedStatus();
	if (oldLed != led){
		UpdateUsbKeyboardLed(led);
        #if !defined(DISABLE_PS2)
        UpdatePs2KeyboardLed(hid_to_ps2(led));
        #endif
		oldLed = led;
	}
}
