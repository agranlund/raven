Raven quickstart

Resources:
https://github.com/agranlund/raven/
https://www.exxosforum.co.uk/forum/viewforum.php?f=119


***   work in progress   ***


- Installation:
---------------
Copy the folder structure to the root of Ravens boot drive.
Take care to copy the files in the AUTO folder in the
order you need them to run.

XBOOT patched for 060 is available in the tools directory
XCONTROL patched for 060 is available in the tools directory


- AUTO Folder:
--------------
The files need to execute in a particular order.
Use "tools/autosort.prg" to assign execution order.

Example autorun order:
 rvbios.prg
 xboot.prg
 magxboot.prg  <- if using MagiC
 rnova.prg
 nvdi.prg
 isa_bios.prg
 rvsnd.prg
 mint.prg  <- if using MiNT


RVBIOS.PRG   : Raven xbios extension
 Needs to be first, or _very_ early in the boot process.

RVNOVA.PRG   : Graphics driver
 Early in the boot process.
 Needs to be before NVDI if you have that installed.

ISA_BIOS.PRG : ISA PnP bios

RVSND.PRG    : Sound driver




- NOVA SETUP:
-------------
Do not put the usual nova executables in the AUTO folder.
You're only going to need RVBIOS.PRG and RVNOVA.PRG.
The correct nova executables, and their settings, are automatically executed
from "c:\nova" according to your settings in Raven BIOS.

To configure NOVA press the DELETE when you see the Atari logo at boot to enter BIOS setup.
Navigate to the NOVA tab to select graphics card and desired settings.


The driver called SVGA is a generic driver supporting multiple cards:

SVGA + Hardware acceleration:
- Cirrus Logic GD5426, GD5428, GD5429, GD5434
- WD90C31, WD90C33

SVGA:
- ET4000, ET4000W32

VGA:
- just about any card should work in regular VGA resolutions
  (640x480 in 16 colors, 320x200 in 256 colors)


Additionally, official NOVA drivers are included for following cards:
- ET4000AX, ET4000W32i, Mach32



- SOUND SETUP:
--------------

1. read rvsnd/readme.txt
2. copy rvsnd/rvsnd.prg -> c:\auto
3. copy rvsnd/rvsnd.inf -> c:\
4. edit c:\rvsnd.inf

The old drivers are no longer needed but kept in the archive for now:
  raven/drivers/gus
  raven/drivers/oplmidi
  raven/drivers/mpu401


- ISA CARDS:
--------------

Look at the example c:\isa_bios.inf and modify as needed.

A file called c:\isa_bios.log will be written after each boot.
This file lists all detected cards and devices, the settings
each of them accepts, as well as the current configuration.

Use this file as a guide for configuring c:\isa_bios.inf.


- MISC TOOLS:
-------------

raven/drivers       : misc hardware drivers
raven/cacheon       : enable cpu caches
raven/cacheoff      : disable cpu caches
raven/castaway      : Atari ST emulation
raven/eiffel        : configuration software for keyboard/mouse/fan
raven/flash         : ROM flash utility
raven/fpemu         : FPU emulator for 68LC060 users
raven/mon           : raven monitor/debugger
raven/resetnvm      : reset NVRAM
raven/ymodem        : serial port file transfer

tools/autoexec      : autoexec with support for commandline parameters
tools/autosort      : utility to set file order in auto folder
tools/emucon2       : EmuTOS console as standalone application
tools/fpupatch      : remove SFP-004 detection from PureC applications
tools/gsxbmix       : gsxb volume mixer, compatible with rvsnd
tools/setenv        : environment variables for TOS
tools/setflags      : utility to set program flags
tools/thingpal      : convert nova_col palette to Photoshop and ThingImg
tools/xboot32e      : boot manager, patched for 060
tools/xcontrol      : Atari control panel + general.cpx patched for 060
cpx/serial.cpx      : Serial port control panel extension




---------------------------------------------------------------------------------

autosort
(c) 1988-92 Eugene F.Sothan
https://www.atari-wiki.com/index.php?title=Autosort

autoexec
(c) 2023 Vinz/MPS
https://github.com/vinz6751/setenv

crosstos
(c) Peter Persson:
https://bitbucket.org/pep-entral/crosstos/src/master/

EmuTOS
(c) The EmuTOS development team
https://github.com/emutos/emutos

CT60 xbios / utilities
(c) Didier Mequignon
(c) Miro Kropacek
https://github.com/mikrosk/ct60tos

CT60 setup
(c) 2009 Patrice Mandin
https://pmandin.atari.org/en/index.php?post/2010/04/15/212-en-ct60-stmicro-flash-chip-support

eiffel
(c) Didier Mequignon
http://didier.mequignon.free.fr/eiffel-e.htm

eneh
(c) 2001-2002 Dr. Thomas Redelberger
https://hardware.atari.org/ether/

fpsp/isp
M68060 Software Package (c) Motorola Inc.
https://github.com/mikrosk/ct60tos
https://github.com/freemint/freemint

fpupatch
(c) 1993 Hartmut Pfitzinger

general.cpx
(c) 1990-1993 Atari Corporation
Patched for 68060 (c) 2003 Roger Burrows
https://github.com/mikrosk/ct60tos

GSXB Mixer
(c) Odd Skancke
https://assemsoft.atari.org/gsxb/index.html

NOVA drivers and utilities
(c) Computerinsel
(c) John McLoud
(c) Idek Tramielski
https://silicon-heaven.org/atari/nova/

parserve (c) Petr Stehlik
https://github.com/joysfera/parcp

parrun (c) Daniel Illgen
https://insane.tscc.de/

patch_xc
(c) 2003 Roger Burrows
https://github.com/mikrosk/ct60tos

pcmake
(c) Thorsten Otto:
https://github.com/th-otto/pcmake

pgusinit
(c) 2022-2024 Ian Scott
https://github.com/polpo/picogus

serial.cpx
(c) 1997 Peter Rottengatter
With additions by: Ulf Ronald Andersson, Thorsten Otto, Anders Granlund

setenv
(c) 2021 Vinz/MPS
https://github.com/vinz6751/setenv

setflags
(c) 1993-2012 Uwe Seimet
https://www.seimet.de/atari/de/

TrueDisk v2.2
(c) 1989-1994 Christoph Zwerschke
Public Domain and free for use in non commercial scale.

uip-tools
(c) 2001, Adam Dunkels
https://bitbucket.org/sqward/uip-tools/src/master/

x86emu
(c) 1996-1999 SciTech Software, Inc.
(c) David Mosberger-Tang
(c) 1999 Egbert Eich
(c) 2007 Joerg Sonnenberger
https://github.com/firebee-org/BaS_gcc

xboot 3.20e
(c) Gribnif Software
Patched by Atarian Computing

xcontrol 1.36
(c) 1990-1993 Atari Corporation
Patched for 68060 (c) 2003 Roger Burrows
https://github.com/mikrosk/ct60tos

ymodem
(c) 2025 Anders Granlund
rtdsr (c) 2011 Pete B. <xtreamerdev@gmail.com>
ymodem.c for reimage, copyright (c) 2009 Rich M Legrand
ymodem.c for bootldr, copyright (c) 2001 John G Dorsey
crc16 function from PIC CRC16, by Ashley Roll & Scott Dattalo

