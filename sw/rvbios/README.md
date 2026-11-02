# rvbios

Raven BIOS extension for TOS.

Runs automatically a bit before TOS runs the AUTO folder, regardless if a disk is present or not.

- Sets up Raven specific bios/xbios overrides and or extensions for TOS:
  - Implements Falcon compatible DSP xbios
  - Implements CT60 compatible xbios extensions, cache control, etc
  - Eiffel/CKBD controller extensions
  - And so on..

- Presents the boot logo, and the bios setup interface when pressing the [DEL] key:
  - Configure cache settings, NVRAM, fan controls, etc..
  - Flash ROM or CKBD firmware
  - Launch the serial or on-screen monitor
  - Launch an EmuTOS terminal. useful as a safemode rescure if normal boot environment is broken

## How it runs

rvbios is built into the ROM image as a virtual Atari cartridge.

This cartridge is PMMU-mapped to 0x00FA0000 during bootloader stage 2 so that TOS finds it as a cartridge during its normal boot sequence.
