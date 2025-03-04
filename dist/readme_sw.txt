Raven quickstart

Resources:
https://github.com/agranlund/raven/
https://www.exxosforum.co.uk/forum/viewforum.php?f=119


***   work in progress   ***


- Installation:
---------------
Copy the folder structure to the root of Ravens boot drive.


- AUTO Folder:
--------------
The files need to execute in a particular order.
Use "tools/autosort.prg" to assign execution order.

RVBIOS.PRG   : Raven core xbios extension
 Needs to be first, or very early in the boot process.
 Temporarily supplied as a PRG but will eventually become part of the ROM.

ISA_BIOS.PRG : ISA PnP bios
 Early in the boot process. After RVBIOS.PRG is a good idea.

RVNOVA.PRG   : NOVA driver launcher
 Early in the boot process, after RVBIOS.PRG but before NVDI or other external GDOS.

SETENV.PRG   : (optional)
 Assign environment variables for plain TOS
 Example "c:\setenv.txt" is provided

AUTOEXEC.PRG : (optional)
 Launch programs with command line arguments
 Example, "c:\autoexec.cnf" is provided


Example file order:

rvbios.prg
  (xboot.prg)
rvnova.prg
  (nvdi.prg)
isa_bios.prg
setenv.prg      <- optional
autoexec.prg    <- optional
  (mint.prg)



- NOVA setup:
-------------
Do not put the usual nova executables in the AUTO folder, you're only going
to need RVBIOS.PRG and RVNOVA.PRG.
The correct nova executables, and their settings, are automatically executed
from "c:\nova" according to your settings in Raven BIOS.

To configure NOVA press the DELETE when you see the Atari logo at boot to enter BIOS setup.
Navigate to the NOVA tab to select graphics card and desired settings.

Modifying resolutions:
 Use VMG.PRG in the driver folder for you particular graphicscard.
 ex: if you use the ET4000.W32 driver then you run "c:\nova\et4000.w32\vmg.prg"

 Only sta_vdi.bib is of importance, even for boot resolutions.
 You want to modify the file that lives in the auto subfolder of your driver
 (NOT the one in c:\auto)
 ex: "c:\nova\et4000.w32\auto\sta_vdi.bib"

 Refer to VMG documentation and/or Atari forums for help on how to modify or create
 new NOVA resolutions using the VMG tool.



- TOOLS:
--------

raven/drivers       : hardware drivers

raven/cacheon       : enable cpu caches
raven/cacheoff      : disable cpu caches
raven/castaway      : Atari ST emulation
raven/eiffel        : Configuration software for keyboard/mouse/fan
raven/flash         : ROM flash utility
raven/fpemu         : FPU emulator for 68LC060 users
raven/mon           : raven monitor/debugger
raven/resetnvm      : reset NVRAM

tools/autoexec      : autoexec with support for commandline parameters
tools/autosort      : utility to set file order in auto folder
tools/setenv        : environment variables for TOS
tools/setflags      : utility to set program flags
tools/thingpal      : convert nova_col palette to Photoshop and ThingImg

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

NOVA drivers and utilities
(c) Computerinsel
(c) John McLoud
(c) Idek Tramielski
https://silicon-heaven.org/atari/nova/

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

uip-tools
(c) 2001, Adam Dunkels
https://bitbucket.org/sqward/uip-tools/src/master/

x86emu
(c) 1996-1999 SciTech Software, Inc.
(c) David Mosberger-Tang
(c) 1999 Egbert Eich
(c) 2007 Joerg Sonnenberger
https://github.com/firebee-org/BaS_gcc


