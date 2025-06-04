//-------------------------------------------------------------------------
// usbreport.c
// Handles the higher-level parts of the PS/2 protocol
// HID conversion, responding to host commands
//-------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "settings.h"
#include "usbhost.h"
#include "usbreport.h"
#include "usbdescr.h"
#include "usbdata.h"
#include "ps2.h"
#include "ikbd.h"

extern const uint16_t HIDtoSET2[256];

#define SetKey(key,report) (report->KeyboardKeyMap[key >> 3] |= 1 << (key & 0x07))
#define GetOldKey(key,report) (report->oldKeyboardKeyMap[key >> 3] & (1 << (key & 0x07)))

ps2mouse_t mouse;
void processSeg(__xdata HID_SEG *currSeg, __xdata HID_REPORT *report, __xdata uint8_t *data)
{
	if (currSeg->InputType == MAP_TYPE_BITFIELD)
	{
    	uint8_t ctrl = currSeg->OutputControl;
		for (uint16_t cnt = currSeg->startBit; cnt < (currSeg->startBit + currSeg->reportCount); cnt++)
		{
			uint8_t *currByte = data + ((cnt) >> 3);
            uint8_t pressed = *currByte & (0x01 << (cnt & 0x07));

			if (currSeg->OutputChannel == MAP_KEYBOARD)
			{
				if (pressed)
				{
					SetKey(ctrl, report);
					if (!GetOldKey(ctrl, report)) {
						report->KeyboardUpdated = 1;
					}
				}
				else
				{
					if (GetOldKey(ctrl, report)) {
						report->KeyboardUpdated = 1;
					}
				}
			}
			else if (currSeg->OutputChannel == MAP_MOUSE)
			{
                report->MouseUpdated = 1;
                if ((ctrl >= MAP_MOUSE_BUTTON1) && (ctrl <= MAP_MOUSE_BUTTON5) && pressed) {
                    report->Mouse.b |= (1 << ctrl);
                }
			}
    		ctrl++;
		}
	}
	else if (currSeg->InputType) //i.e. not MAP_TYPE_NONE
	{
        bool make = 0;
		uint32_t value = 0;

		// bits may be across any byte alignment
		// so do the neccesary shifting to get it to all fit in a uint32_t
		int8_t shiftbits = -(currSeg->startBit % 8);
		uint8_t startbyte = currSeg->startBit / 8;
        uint8_t *currByte = data + startbyte;
		while(shiftbits < (int)currSeg->reportSize) {
			if (shiftbits < 0)
				value |= ((uint32_t)(*currByte)) >> (uint32_t)(-shiftbits);
			else
				value |= ((uint32_t)(*currByte)) << (uint32_t)shiftbits;
            currByte++;
			shiftbits += 8;
		}

		// if it's a signed integer we need to extend the sign
		// todo, actually determine if it is a signed int... look at logical max/min fields in descriptor
		if (currSeg->InputParam & INPUT_PARAM_SIGNED)
			value = SIGNEX(value, currSeg->reportSize - 1);

		if (currSeg->OutputChannel == MAP_KEYBOARD)
        {
			report->KeyboardUpdated = 1;
            if (currSeg->InputType == MAP_TYPE_ARRAY) {
                SetKey(value, report);
            }
            else if ((currSeg->InputType == MAP_TYPE_THRESHOLD_ABOVE && value > currSeg->InputParam) ||
                (currSeg->InputType == MAP_TYPE_THRESHOLD_BELOW && value < currSeg->InputParam) ||
                (currSeg->InputType == MAP_TYPE_EQUAL && value == currSeg->InputParam)) {
                    SetKey(currSeg->OutputControl, report);
            }
        }
        else if (currSeg->OutputChannel == MAP_MOUSE)
        {
            report->MouseUpdated = 1;
            if (currSeg->InputType == MAP_TYPE_THRESHOLD_ABOVE)
            {
            	uint8_t ctrl = currSeg->OutputControl;
                if ((ctrl >= MAP_MOUSE_BUTTON1) && (ctrl <= MAP_MOUSE_BUTTON5)) {
                    report->Mouse.b |= (1 << ctrl);
                }
            }
            else if (currSeg->InputType == MAP_TYPE_SCALE)
            {
				#define DEADZONE 1
				switch (currSeg->OutputControl)
				{
				// TODO scaling
				case MAP_MOUSE_X:
                    /*
					if (currSeg->InputParam == INPUT_PARAM_SIGNED_SCALEDOWN){
						int16_t tmpl = ((int8_t)((value + 8) >> 4) - 0x08);
						// deadzone
						if (tmpl <= -DEADZONE) tmpl+= DEADZONE;
						else if (tmpl >= DEADZONE) tmpl-= DEADZONE;
						else tmpl = 0;
						MouseMove(tmpl, 0, 0);
					}
					else
                    */
                    {
                        report->Mouse.x = (int16_t)value;
                    }
					break;
				case MAP_MOUSE_Y:
                    /*
					if (currSeg->InputParam == INPUT_PARAM_SIGNED_SCALEDOWN) {
						int16_t tmpl = ((int8_t)((currSeg->value + 8) >> 4) - 0x08);
						// deadzone
						if (tmpl <= -DEADZONE) tmpl+= DEADZONE;
						else if (tmpl >= DEADZONE) tmpl-= DEADZONE;
						else tmpl = 0;
						MouseMove(0, tmpl, 0);
					}
					else
                    */
                    {
                        report->Mouse.y = (int16_t)value;
                    }
					break;
				case MAP_MOUSE_WHEEL:
                    report->Mouse.z = (int16_t)value;
					break;
				}
            }
        }

	}
}
 
bool ParseReport(__xdata INTERFACE *interface, uint32_t len, __xdata uint8_t *report)
{
	__xdata HID_REPORT *descReport;
	__xdata UsbLinkedList *currSegNode;

	if (interface->usesReports)
	{
		// first byte of report will be the report number
		descReport = (__xdata HID_REPORT *)UsbListGetData(interface->Reports, report[0]);
	}
	else
	{
		descReport = (__xdata HID_REPORT *)UsbListGetData(interface->Reports, 0);
	}
	
	if (descReport == NULL) {
		TRACE("Invalid report");
		return 0;
	}

	// sanity check length - smaller is no good
	if (len < descReport->length)
	{
		TRACE("report too short - %lu < %u", len, descReport->length);
		return 0;
	}

	currSegNode = descReport->HidSegments;

	// clear key map as all pressed keys should be present in report
	memset(descReport->KeyboardKeyMap, 0, 32);
    descReport->Mouse.x = descReport->Mouse.y = descReport->Mouse.z = descReport->Mouse.b = 0;

	while (currSegNode != NULL)
	{
		processSeg((__xdata HID_SEG *)(currSegNode->data), descReport, report);
		currSegNode = currSegNode->next;
	}

	if(descReport->KeyboardUpdated)
	{
		// for each byte in the report
		for (uint8_t d = 0; d < 32; d++) 
		{
			// XOR to see if any bits are different
			uint8_t xorred = descReport->KeyboardKeyMap[d] ^ descReport->oldKeyboardKeyMap[d];

			if (xorred) {

				for (uint8_t c = 0; c < 8; c++)
				{
					if (xorred & (1 << c)) 
					{
						uint8_t hidcode = (d << 3) | c;
                        uint16_t ps2code = HIDtoSET2[hidcode];
						if (descReport->KeyboardKeyMap[d] & (1 << c)) // set in current but not prev
						{
                            // Make
                            //TRACE("UsbKeyMake  $%x $%x", hidcode, HIDtoSET2[hidcode]);
                            ikbd_Ps2KeyDown(ps2code);
						}
						else // set in prev but not current
						{
                            // break
                            //TRACE("UsbKeyBreak $%x $%x", hidcode, HIDtoSET2[hidcode]);
                            ikbd_Ps2KeyUp(ps2code);
						}
					}
				}
                descReport->oldKeyboardKeyMap[d] = descReport->KeyboardKeyMap[d];
			}
		}
		descReport->KeyboardUpdated = 0;
	}

    if (descReport->MouseUpdated)
    {
        int16_t x = ScaleToIkbd(descReport->Mouse.x, Settings.UsbMouseScale);
        int16_t y = ScaleToIkbd(descReport->Mouse.y, Settings.UsbMouseScale);
        int16_t z = ScaleToIkbd(descReport->Mouse.z, Settings.UsbWheelScale);
        uint8_t b = descReport->Mouse.b >> 1;
        descReport->oldMouse = descReport->Mouse;
        descReport->MouseUpdated = 0;
        ikbd_MouseUpdate(x, y, -z, b);
    }

	return 1;
}


const uint16_t HIDtoSET2[256] = {
0,      // none
0,      // err: rollover
0,      // err: post
0,      // err: undefined
0x1C,           // A
0x32,           // B
0x21,           // C
0x23,           // D
0x24,           // E
0x2B,           // F
0x34,           // G
0x33,           // H
0x43,           // I
0x3B,           // J
0x42,           // K
0x4B,           // L
0x3A,           // M
0x31,           // N
0x44,           // O
0x4D,           // P
0x15,           // Q
0x2D,           // R
0x1B,           // S
0x2C,           // T
0x3C,           // U
0x2A,           // V
0x1D,           // W
0x22,           // X
0x35,           // Y
0x1A,           // Z
0x16,           // 1
0x1E,           // 2
0x26,           // 3
0x25,           // 4
0x2E,           // 5
0x36,           // 6
0x3D,           // 7
0x3E,           // 8
0x46,           // 9
0x45,           // 0
0x5A,           // enter
0x76,           // esc
0x66,           // backspace
0x0D,           // tab
0x29,           // space
0x4E,           // dash
0x55,           // euals
0x54,           // left square bracket
0x5B,           // right square bracket
0x5D,           // backslash
0x5D,           // euro1
0x4C,           // semicolon
0x52,           // apostrophe
0x0E,           // tilde
0x41,           // comma
0x49,           // period
0x4A,           // slash
0x58,           // capslock
0x05,           // f1
0x06,           // f2
0x04,           // f3
0x0C,           // f4
0x03,           // f5
0x0B,           // f6
0x83,           // f7
0x0A,           // f8
0x01,           // f9
0x09,           // f10            
0x78,           // f11
0x07,           // f12
0xE07C,         // printscreen
0x7E,           // scroll lock
0x00,           // pause <------
0xE070,         // insert
0xE06C,         // home
0xE07D,         // page up
0xE071,         // delete
0xE069,         // end
0xE07A,         // page down
0xE074,         // right
0xE06B,         // left
0xE072,         // down
0xE075,         // up
0x77,           // numlock
0xE04A,         // slash (numpad)
0x7C,           // asterisk (numpad)
0x7B,           // minus (numpad)
0x79,           // plus (numpad)
0xE05A,         // enter (numpad)
0x69,           // 1 (numpad)
0x72,           // 2 (numpad)
0x7A,           // 3 (numpad)
0x6B,           // 4 (numpad)
0x73,           // 5 (numpad)
0x74,           // 6 (numpad)
0x6C,           // 7 (numpad)
0x75,           // 8 (numpad)
0x7D,           // 9 (numpad)
0x70,           // 0 (numpad)
0x71,           // period (numpad)
0x61,           // euro2
0xE02F,         // app
0xE037,         // power
0x0F,           // equal (numpad)
0x08,           // f13
0x10,           // f14
0x18,           // f15
0x20,           // f16
0x28,           // f17
0x30,           // f18
0x38,           // f19
0x40,           // f20
0x48,           // f21
0x50,           // f22
0x57,           // f23
0x5F,           // f24
0,		// exec
0,      // help
0,      // menu
0,      // select
0,      // stop
0,      // again
0,      // undo
0,      // cut
0,      // copy
0,      // paste
0,      // find
0xE023,         // mute
0xE032,         // volume up
0xE021,         // volume down
0,      // locking caps lock
0,      // locking num lock
0,      // locking scroll lock
0x6D,           // comma (numpad)
0,		// keypad equal
0x51,           // intl1
0x13,           // intl2
0x6A,           // intl3
0x64,           // intl4
0x67,           // intl5
0x27,           // intl6
0,		// intl7
0,		// intl8
0,		// intl9
0xF2,           // lang1
0x13,           // lang2
0x6A,           // lang3
0x67,           // lang4
0x27,           // lang5
0,		// lang6
0,		// lang7
0,		// lang8
0,		// lang9
0,		// alt erase
0,		// sysreq
0,		// cancel
0,		// clear
0,		// prior
0,		// return
0,		// separator
0,		// out
0,		// oper
0,      // clear/again
0,  	// clsel/props
0,      // exsel
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,
0,      // keypad 00
0,      // keypad 000
0,		// thousand separator
0,		// decimal separator
0,		// currency unit
0,		// currency subunit
0,		// keypad (
0,		// keypad )
0,		// keypad {
0,		// keypad }
0,		// keypad tab
0,		// keypad space
0,		// keypad A
0,		// keypad B
0,		// keypad C
0,		// keypad D
0,		// keypad E
0,		// keypad F
0,		// keypad XOR
0,		// keypad ^
0,		// keypad %
0,		// keypad <
0,		// keypad >
0,		// keypad &
0,		// keypad &&
0,		// keypad |
0,		// keypad ||
0,		// keypad :
0,		// keypad #
0,		// keypad space
0,		// keypad @
0,		// keypad !
0,		// keypad memory store
0,		// keypad memory recall
0,		// keypad memory clear
0,		// keypad memory add
0,		// keypad memory subtract
0,		// keypad memory multiply
0,		// keypad memory divide
0,		// keypad +/-
0,		// keypad clear
0,		// keypad clear entry
0,		// keypad binary
0,		// keypad octal
0,		// keypad decimal
0,		// keypad hexadecimal
0,
0,
0x14,           // left control
0x12,           // left shift
0x11,           // left alt
0xE01F,         // left gui
0xE014,         // right control
0x59,           // right shift
0xE011,         // right alt
0xE027,         // right gui
0xE034,         // media play/pause
0xE03B,         // media stop cd
0xE015,         // media previous song
0xE04D,         // media next song
0,              // media eject cd
0xE032,         // media volume up
0xE021,         // media volume down
0xE023,         // media mute
0xE03A,         // media www
0xE038,         // media back
0xE030,         // media fwd
0xE028,         // media stop
0xE010,         // media find
0,              // media scroll up
0,              // media scroll down
0,              // media edit
0,              // media sleep
0,              // media coffee
0,              // media refresh
0,              // media calc
0,
0,
0,
0
};
