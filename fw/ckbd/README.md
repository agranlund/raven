# CKBD
## Eiffel replacement for Raven

## **** WIP and not ready for general use ****

CKBD aims to be fairly compatible with Eiffel and allows the use of the following devices:
* USB Keyboard and mouse
* PS2 Keyboard and mouse
* Atari joystick(s)
* Atari mouse
* Amiga mouse

Raven.A2 has the necessary hardware already on the motherboard.

Older revisions can use an [upgrade board](../../hw/raven/upgrades/ckbd/) that plugs into the Eiffel port.
Two USB ports are then exposed on an internal header.

## Technical description

CKBD is based around the CH559 microcontroller from WCH.
Some parts of this sourcecode is based on HIDman by Rasteri (https://github.com/rasteri/HIDman)
which is turn was based on CH559sdccUSBHost by atc114 (https://github.com/atc1441/CH559sdccUSBHost)

# Firmware Update

todo: proper instructions...

todo: instructions on first time programming vs in-system programming


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

