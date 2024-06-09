# Raven ROM

This is all early temporary stuff and a bit all over the place at the moment.

## Todo:
Either make a nice and clear separation on what goes in the bootblock and what goes in EmuTOS, or just build everything into EmuTOS. Also consider MagiC port and avoid having to copy/paste/maintain the same type of code in multiple places.


## Build

- Get EmuTOS for Raven here: https://github.com/agranlund/emutos/tree/raven
- Make sure EMUTOS_DIR in Makefile points to where you have Raven EmuTOS
- Run "make tos"
- Program rom simm with "rom.bin"


