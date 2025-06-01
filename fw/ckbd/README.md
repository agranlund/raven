# CHKBD
## Eiffel replacement for Raven

CHKBD aims to be fairly compatible with Eiffel and allows the use of the following devices:
* USB Keyboard and mouse
* PS2 Keyboard and mouse
* Atari joystick(s)
* Atari mouse
* Amiga mouse

Raven.A2 has the necessary hardware already on the motherboard.
Older revisions can use an [upgrade board](hw/raven/upgrades/ckbd/) that plugs into the Eiffel port.
Two USB ports are then exposed on an internal header.

## Technical description

CHKBD is based around the CH559 microcontroller from WCH.
The sourecode is based on HIDman: https://github.com/rasteri/HIDman
Which in turn was based on CH559sdccUSBHost: https://github.com/atc1441/CH559sdccUSBHost


# Firmware Update

todo: proper instructions...

## Common
1. Power off Raven
1. Disconnect all USB devices
3. Connect LOWER USB port to your PC
2. Short the programming header
4. Power on Raven
5. Perform programming as per instructions below
6. Power off Raven
7. Disconnect USB cable and reconnect your USB devices
7. Unshort the programming header
9. Power on Raven

## Windows:
Install WCH's ISP tool: https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html
todo: instructions

## Mac/Linux:

todo: instructions


# Compiling

Compiles with SDCC: https://sdcc.sourceforge.net/

