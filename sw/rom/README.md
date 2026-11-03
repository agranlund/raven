# Raven ROM

|||
|------|----------|
|[`boot/`](boot/)|bootloader + monitor|
|[`srec/`](srec/)|baremetal programs which can be sent over serial to run in bootrom monitor, not part of roms|
|[`emutos/`](emutos/)|TOS rom|
|[`romdisk/`](romdisk/)|TOS romdisk|
|[`toscart/`](toscart/)|TOS virtual cartridge skeleton|


## Layout

`mkrom` is a tool which pieces together a full image from individual binaries.

bootrom must always be at the start of the image and other contents can be appended in any order.
The TOC lives in the bootrom area and is updated automatically when binaries are added with mkrom.


## Build complete rom including TOS

- Run "make"
- Program ROM simm with "rom_tos.bin"


## Build bootloader + monitor rom only

- Run "make mon"
- Program ROM simm with "rom_mon.bin"

## EmuTOS
`emutos/` is a subtree of upstream [EmuTOS](https://github.com/emutos/emutos) operating system, updated with `update_emutos_subtree.sh`.

Raven specific code lives in:
- `bios/raven.c`, `bios/raven.h`, `bios/raven2.S`
- `bios/startup.S`, `bios/processor.S`, `bios/memory.S`
- `bios/ikbd.c`, `bios/midi.c`, `bios/ide.c`, `bios/screen.c`
- search for `MACHINE_RAVEN` define.

## MagiC
Raven specific code for the alternative operating system [MagiC](https://github.com/th-otto/MagicMac) lives upstream and is not part of this repo.
The define to look for in code is simply RAVEN. Kernel project file is in the upstream repo as kernel/build/raven.prj


## srec
Despite living in this folder, these programs are not included as part of rom images.
They are small baremetal programs which can be sent over serial to be executed from the bootrom monitor.

|||
|------|----------|
|[`srec/selftest/`](srec/selftest/)|selftest diagnostic program|
|[`srec/example/`](srec/example/)|Hello world example to use as a template for own programs|
