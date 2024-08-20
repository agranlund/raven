# Raven ROM

This is all early temporary stuff and a bit all over the place at the moment.

Latest rom code requires Nessi firmware 240819 or later due to a memory map change.
To build against an earlier firmware you'll need to change the BIOS_ROM defined in mon/Makefile to 0x03000000


## Todo:
Keep separating Raven specific support functions out of EmuTOS and into bootrom.
This is to easier facilitate other operating systems such as MagiC or a port of regular AtariTOS.


## Build complete rom including TOS

- Run "make"
- Program rom simm with "rom_tos.bin"


## Build rom with only debug monitor

- Run "make mon"
- Program rom simm with "rom_mon.bin"

