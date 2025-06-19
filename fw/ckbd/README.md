# CKBD
## Eiffel replacement for Raven
## This is a work in progress and may contain yet to be discovered incompatibilities

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

# First time programming: Windows
- Install WCHISPTool tool: https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html
- Connect CKBD to your computer using a USB cable connected to the USB0 port.
- Select "File -> Load UI Config" and pick "ckbd_a1.ini"
- Under the "Download File" section pick "ckbd_a1.bin" as Object File 1
- Press Download to program the firmware

# First time programming: MacOS/Linux
There are several tools for flashing CH559, including an official one from WCH.
I am using ch552tool from MarsTechHAN (https://github.com/MarsTechHAN/ch552tool)

Flashing with ch552tool:
- install ch552tool, libusb and pyusb
- Connect CKBD to your computer using a USB cable connected to the USB0 port.
- run "python3 ch55xtool.py -f ckbd_a1.bin -r"

Flashing with WCHISPTool_Cmd:
- Install WCHISPTool_Cmd
- Read the documentation (especially if you are on MacOS)
- Connect CKBD to your computer using a USB cable connected to the USB0 port.
- run "WCHISPTool_CMD -p <devid> -c ckbd_wchisp.ini -o program -f ckbd_a1.bin"
  (you did read the manual regarding finding your value for <devid>, right?)


## Firmware update
With the CBKD installed in Raven:
   - press the NMI button to enter the monitor
   - run "kbd flash"
   - send the ckbd firmware binary with your serial terminal
   - run "reset" to reset Raven

If the ckbd is bricked and won't respond to updating firmware the normal way then:
  - make sure Raven is powered off
  - disconnect all usb devices
  - jumper the two middle pins of CKBD header J104
  - power on Raven
  - perform the same steps as when you did the first time programming
  - power off Raven
  - remove jumper from J104 and your USB programming cable
  - reconnect your devices and power on Raven


# Compiling
Compiles with SDCC: https://sdcc.sourceforge.net/

If you are building and working on the firmware yourself you can use "make flash" or "make flash_wchispcmd" to flash using the bundled ch552tool or wchisptool.
When using wchisptool you'll need to modify the WCHISPDEV variable in the makefile.

Executing "kbd prog" from the Raven monitor will put the CH559 into bootloader mode so it can be programmed with these tools (ie; you don't need to jumper it to start in bootloader mode)
