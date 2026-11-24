# rvbios

Raven BIOS extension for TOS.

Runs right before TOS runs the AUTO folder, regardless if a disk is present or not.
(It's mapped as a virtual cartridge, pmmu-mapped to 0x00fa0000 during boot stage2)

- Installs up Raven specific bios/xbios overrides and TOS extensions:
  - Falcon compatible DSP xbios
  - CT60 compatible xbios extensions, cache control, etc
  - Eiffel/CKBD controller extensions
  - Machine specific TOS cookies
  - And so on..

- Boot logo with option to go into setup screen:
  - Cache settings, NVRAM, fan controls, etc..
  - Flash ROM or CKBD firmware
  - Launch the serial / on-screen monitor
  - Launch early EmuTOS terminal. useful as a safemode rescure if normal boot environment is broken
