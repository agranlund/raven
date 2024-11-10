Raven quickstart

Resources:
https://github.com/agranlund/raven/
https://www.exxosforum.co.uk/forum/viewtopic.php?f=18&t=6940



- Installation:
---------------
Copy the folder structure to the root of Ravens boot drive.


- AUTO Folder:
--------------
The files need to execute in a particular order.
Use "tools/autosort.prg" to assign execution order.

RVBIOS.PRG : raven xbios extension
 Needs to be first, or very early in the boot process.
 Temporarily supplied as a PRG but will eventually become part of the ROM.

RVNOVA.PRG : nova driver launcher
 Early in the boot process, after RVBIOS.PRG but before NVDI or other external GDOS.


Example file order:
 rvbios.prg
 xboot.prg
 rvnova.prg
 nvdi.prg
 mint.prg



- NOVA setup:
-------------
Do not put the usual nova executables in the AUTO folder, you only need RVNOVA.PRG.
The correct nova executables, and their settings, are automatically executed
from "c:\nova" according to your settings in Raven BIOS.

To configure NOVA press the DELETE key during boot to enter BIOS setup.
The NOVA tab will lets you select your graphics card and settings.

Modifying resolutions:
 Use VMG.PRG in the driver folder for you particular videocard.
 ex: if using an ET4000-W32i you run "c:\nova\et4kw32i\vmg.prg"

 Only sta_vdi.bib is of importance, even for boot resolutions, and the file to modify
 is the one in the auto folder that is local to the driver (NOT in c:\auto)
 ex: "c:\drivers\nova\et4kw32i\auto\sta_vdi.bib"

 Refer to VMG documentation and/or Atari forums for help on how to modify or create
 new NOVA resolutions using the VMG tool.



- TOOLS:
--------
castaway      : Atari ST emulation
eiffel        : Configuration software for keyboard/mouse/fan
autosort.prg  : sort utility for auto folder
cacheon.prg   : enable cpu caches
cacheoff.prg  : disable cpu caches
resetnvm.prg  : reset NVRAM
thingpal.tos  : convert nova_col palette to Photoshop and ThingImg

