
# Raven software

dsp/        DSP toolchain + bootcode
gcc/        GCC toolchain related
purec/      PureC toolchain related
isa/        ISA bus framework, PnP bios, drivers
lib/        Raven related headers / libs
rom/        bootrom + tos
rvbios/     TOS bios extensions
rvnova/     TOS graphics drivers
rvsnd/      TOS sound drivers
tools/      TOS utils and apps


## Subtrees:
sw/rom/emutos           : https://github.com/emutos/emutos master
sw/isa/drivers/picogus  : https://github.com/polpo/picogus main
sw/dsp/asm56k           : https://github.com/agranlund/asm56k master
sw/dsp/dsptools         : https://github.com/insane-rabenauge/dsptools main


## Compilers:
rom/*                       : m68k-atari-mintelf-gcc

rvbios                      : pureC
rvnova                      : pureC
rvsnd                       : pureC
tools/*                     : PureC
tools/fpu_emu               : m68k-atari-mintelf-gcc
tools/uiptools              : m68k-atari-mintelf-gcc

isa/*                       : pureC
isa/drivers/ne2000/ethernec : turboC (1)
isa/drivers/picogus         : m68k-atari-mintelf-gcc

(1) using existing binaries, not compiling from source.
    to be replaced by own driver at some point.
