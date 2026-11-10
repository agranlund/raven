# Bootloader and monitor

Must live at the start of ROM image, before TOS or any other sub-images.

Fixed layout:
- 0x0000 early vectors
- 0x0400 ROM table of contents
- 0x0800 API function exports
- 0x1000 PRAM area for config storage
- 0x2000 Bootloader entry point


## Stages

**Stage 0** (`_boot_stage0.S`) - Startup

Minimal early init, uart sign of life, jumps to stage1 for full hardware init.

**Stage 1** (`boot_stage1.c`) - Proper hardware detect + init
  - CPU, FPU, DSP and board rev detect
  - ROM and RAM slot+size detection
  - board and system init (rtc, i2c, mfp, ikbd, pram, etc.. see `hw` folder)
  - Jumps to stage2 if TOS is present in ROM image, else interactive debug monitor

**Stage 2** (`boot_stage2.c`) TOS boot
- Builds Atari compatible address map in PMMU
- Configures VBR proxy
- Sets up ACIA and VBL emulation
- Runs VGABIOS through x86 emulation, setting ST_HIGH compatible resolution
- Jumps to TOS


## Monitor (`monitor.c`)

Interactive serial-port debug monitor. Available in two situations:
- Automatically at boot if TOS is not present in ROM.
- By pressing NMI button on a running system.

NMI is quite useful for diagnosing hangs — if the monitor responds to NMI
then a hang is likely in some software. If it doesn't then it's more likely on the hardware side and probably bus logic (stuck in a cycle).

Monitor has very basic debugging features for inspecting and modifying mem/io/registers.

*todo: breakpoints and single-stepping.*

|Commands| |
|------|----------|
|`x`|exit monitor|
|`r {val}`|show/modify register|
|`pb/pw/pl [addr] {val}`|peek/poke
|`d [addr] {len}`|dump memory
|`a [addr] {len}`|disassemble
|`rtc {clear/reset}`|nvram dump/clear/reset
|`pram {cmd} {opt}`|pram commands
|`kbd {cmd} {opt}`|keyboard controller commands
|`cfg {opt} {val}`|list/get/set option
|`flash`|program ROM over serial
|`reset`|reset machine

(Type `h` for full list of commands)

srec files can simply be sent over serial. it detects the format and launches it automatically after it has been received.

## Disassembler

m68k_disasm.c comes from CT60/Firebee FireTOS
Copyright (c) 1999-2002 Frank Wille
Based on NetBSD's m68k/m68k/db_disasm.c by Christian E. Hopps.

## x86

Just-enough x86 emulation is implemented for running graphics cards vgabios.
This is the old emulator from SciTech which was built for exactly that, adapted for Raven.

## Building

Build from `sw/rom/`:
- `make` — full ROM image including TOS
- `make mon` — bootloader and monitor only
