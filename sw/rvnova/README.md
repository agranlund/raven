# rvnova

NOVA-compatible ISA graphics card driver package for Raven.

A standard NOVA installation has `emulator.prg`, `menu.prg` and `sta_vdi.prg` in
the AUTO folder, all three specific to one card. rvnova replaces this with a single
`rvnova.prg` in AUTO which launches the correct set of drivers from `c:\nova\<driver>\`
depending on which card is active. This makes switching cards a settings change rather
than a file management exercise.

## SVGA driver (`driver/`)

A generic NOVA driver with auto-detecting per-card backends. Select SVGA as the
active driver in the rvbios setup screen and it will detect and initialise the
installed card automatically.

Specific card support:
- Cirrus GD54xx — SVGA modes, hardware acceleration
- WDC WD90Cxx — SVGA modes, hardware acceleration
- Oak, Tseng ET4000AX — SVGA modes, no acceleration
- Any other ISA VGA card — standard VGA modes only

## Official legacy NOVA drivers

Third-party NOVA drivers for specific cards are included alongside SVGA for cases
where they provide better support:
- Tseng ET4000/W32i — hardware acceleration
- ATI Mach32 — hardware acceleration
- ATI Mach64 — hardware acceleration

## Hardware acceleration (`driver/vdi.c`)

For cards with acceleration support, the driver patches the loaded VDI binary at
runtime rather than replacing it. It locates the relocated VDI in memory by anchoring
to a known offset, walks the internal dispatch tables, and patches them to redirect
draw calls to hardware-accelerated callbacks. The original VDI binary is not modified.

## Loader (`loader/`)

`rvnova.prg` lives in the AUTO folder and manages driver selection. Active driver
is configured via the rvbios setup screen or `rvga.inf`.
