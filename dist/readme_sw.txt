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

RVNOVA.PRG   : NOVA driver launcher
 Early in the boot process, after RVBIOS.PRG but before NVDI or other external GDOS.

ISA_BIOS.PRG : ISA PnP bios
 Before any program that use ISA cards which is not the graphics card.
 After RVNOVA.PRG is a good idea so that you can see its output messages.


Example file order:

rvbios.prg
  (xboot.prg)
rvnova.prg
isa_bios.prg
  (nvdi.prg)
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
castaway      : Atari ST emulation
eiffel        : Configuration software for keyboard/mouse/fan
fpemu.prg     : FPU emulator for 68LC060 user
cacheon.prg   : enable cpu caches
cacheoff.prg  : disable cpu caches
resetnvm.prg  : reset NVRAM
thingpal.tos  : convert nova_col palette to Photoshop and ThingImg
autosort.prg  : utility to set file order in auto folder

