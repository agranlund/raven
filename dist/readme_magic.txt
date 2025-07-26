
Raven MagiC

Requirements:

- MagiC requires some kind of hard disk driver installed on the boot partition.
I have been using HDDriver successfully but perhaps other ones work fine too.

- Unlike EmuTOS, MagiC itself and the Atari HDD driver will assume a
standard Atari IDE interface with Motorola byte order.

Raven rev.A2 has this kind already but the older rev.A1 boards come
with a little endian Intel style interface.
To run MagiC on those boards you are going to need some kind of
byteswap cable or adapter. For example this one:
https://github.com/agranlund/raven/tree/main/hw/raven/upgrades/ideswap


Setup:

- todo

MagXnet:

- todo

Auto folder:

The start order is very important. You need magxboot as early as possible
but after rvbios.prg and before rvnova.prg.

rvbios.prg
magxboot.prg
rvnova.prg

