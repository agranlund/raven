# rvnova

NOVA-compatible ISA graphics card driver package for Raven.

A standard NOVA installation has `emulator.prg`, `menu.prg` and `sta_vdi.prg` in
the AUTO folder, all three specific to one card. rvnova replaces this with a single
`rvnova.prg` in AUTO which launches the correct set of drivers from `c:\nova\<driver>\`
depending on which card is active.

## SVGA driver (`driver/`)

Raven specific NOVA driver with auto-detecting per-card backends. Select SVGA as the
active driver in the rvbios setup screen and it will detect and initialise the
installed card automatically.

Specific card support:
- Cirrus GD54xx — SVGA modes, hardware acceleration
- WDC WD90Cxx — SVGA modes, hardware acceleration
- Oak, Tseng ET4000AX — SVGA modes, no acceleration
- Any other ISA VGA card — standard VGA modes only

## Hardware acceleration (`driver/vdi.c`)

VDI binary (STA_VDI.PRG) is patched in memory at runtime for injecting acceleration hooks.
STA_VDI is assumed to come from TT_ET4000T8_V300 -- other VDI versions are not guaranteed to work.

## Loader (`loader/`)

`rvnova.prg` lives in the AUTO folder and manages driver selection. Active driver
is configured via the rvbios setup screen or `rvga.inf`.


## Legacy NOVA drivers

Third-party NOVA drivers for specific cards are included alongside SVGA for cases
where they provide better support:
- Tseng ET4000/W32i — hardware acceleration
- ATI Mach32 — hardware acceleration
- ATI Mach64 — hardware acceleration
