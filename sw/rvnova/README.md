# rvnova

NOVA-compatible ISA graphics card driver package for Raven.


## SVGA driver (`driver/`)

Raven specific NOVA driver, called 'SVGA' in rvbios setup screen.
- Cirrus GD54xx — SVGA with hardware acceleration
- WDC WD90C3x — SVGA with hardware acceleration
- ET4K, OAK, S3 — wip SVGA without hw acceleration
- Other VGA compatible cards — standard VGA modes only

todo: maybe split into individual drivers rather than monolith SVGA driver


## STA_VDI and Hardware acceleration (`driver/vdi.c`)

SVGA driver assumes STA_VDI from the TT_ET4000T8_V300 package, and other
versions are not guaranteed to work.
(STA_VDI binary is patched in memory at runtime, based on info from disassembling that particular version)

## Loader

`rvnova.prg` in c:\auto folder replaces the usual trio of driver-specific auto programs.

Individual NOVA driver installations should instead live under `c:\nova\<driver>\`.


## Settings

todo: describe available settings for `rvga.inf` 

## Legacy NOVA drivers

Original NOVA drivers are supported (and included in Raven release package)
- Tseng ET4000/W32i — hardware acceleration
- ATI Mach32 — hardware acceleration
- ATI Mach64 — hardware acceleration
