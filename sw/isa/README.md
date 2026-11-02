# ISA bus

## isa_bios
ISA Plug and Play BIOS.

Enumerates ISA bus, allocates resources and configures PnP cards at boot. 

Written primarily for Raven but designed to support other Atari compatibles with ISA bus too.
See [isa_bios/readme.txt](isa_bios/readme.txt) for
platform support and configuration.

## drivers
Individual device drivers:
- `ne2000/` — NE2K network driver for Sting, Mint, Magic (Ethernec, Hades-compatible)
- `fdc/` — Floppy disk controller
- `fmradio/` — Aims radio card
- `gus/` — Gravis Ultrasound init and mixer
- `picogus/` — PicoGUS (Atari port of pgusinit)
- `mpu401/` — MPU-401 MIDI interface
- `oplmidi/` — OPL softsynth MIDI

Note: gus, mpu401, and oplmidi are standalone legacy drivers.
[sw/rvsnd](../rvsnd/) includes drivers for these and is preferred for new setups.
