# Bootloader and monitor

The first thing that runs on Raven.

Must live at the start of the ROM image, before TOS or any other sub-image added to the complete ROM.

## Stages

**Stage 0** (`_boot_stage0.S`)
Minimal system + uart init, boot message, then jumps to stage1 for full hardware init.

Fixed layout, governed by the linker script:

- 0x0000 early vectors
- 0x0400 ROM table of contents area
- 0x0800 API function exports
- 0x1000 PRAM area for config storage
- 0x2000 Bootloader entry point.


**Stage 1** (`boot_stage1.c`)
Hardware detection and initialisation.
  - Identifies CPU variant and board revision
  - Probes ROM and RAM slots and sizes
  - DSP detection
  - Inits all individual hw systems (i2c, rtc, mfp, ikbd, etc etc.. see `hw` folder)
  - Jumps to stage2 if TOS is present in ROM, else drops into the interactive monitor.

**Stage 2** (`boot_stage2.c`) Builds Atari compatible environment and boots operating system.
- Runs VGABIOS on graphicscard through x86 emulation.
- Builds Atari compatible address map in PMMU
- Configures VBR proxy
- Sets up ACIA and VBL emulation
- Jumps to TOS


## Monitor (`monitor.c`)

Interactive debug monitor accessible over the serial port. Available in two situations:
- Automatically at boot if TOS is not found in ROM image.
- By pressing NMI button on a running system.

The NMI entry point is useful for diagnosing hangs — if the monitor responds to NMI
then a hang is likely in software. If it doesn't, then it's more likely in bus logic.

Monitor works as a basic debugger where you can inspect and modify memory/io as well as the registers.

For a running system it works as a basic debugger: inspect and modify
memory and registers at the point where execution was interrupted, then resume.

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


To load and run an S-record file just send it over serial. Monitor detects
the S-record format automatically and launches it automatically after received

## Hardware

Individual hardware init/support code lives in the `hw` folder.

## x86
nough x86 emulation to execute vgabios on videocards.
This is the old SciTech emulator used by tons of software for this purpose.


## Building

Build from `sw/rom/`:
- `make` — full ROM image including TOS
- `make mon` — bootloader and monitor only
