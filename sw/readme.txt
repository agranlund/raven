
## Subtrees:
sw/rom/emutos           : https://github.com/emutos/emutos master
sw/isa/drivers/picogus  : https://github.com/polpo/picogus main


## Compilers:
rom/mon                     : m68k-atari-mintelf-gcc
rom/srec                    : m68k-atari-mintelf-gcc
rom/emutos                  : m68k-atari-mint-gcc

rvbios                      : pureC
rvnova                      : pureC / m68k-atari-mint-gcc
tools/cache                 : pureC
tools/eiffel                : pureC
tools/flash                 : pureC
tools/fpu_emu               : m68k-atari-mint-gcc
tools/fpu_off               : pureC
tools/mon                   : pureC
tools/patch_xc              : pureC
tools/raven.cpx             : pureC
tools/resetnvm              : pureC
tools/serial.cpx            : pureC
tools/thingpal              : pureC / m68k-atari-mint-gcc
tools/uiptools              : m68k-atari-mint-gcc

isa/isa_bios                : pureC / m68k-atari-mint-gcc
isa/drivers/fmradio         : pureC / m68k-atari-mint-gcc
isa/drivers/gus             : pureC / m68k-atari-mint-gcc
isa/drivers/mpu401          : pureC
isa/drivers/oplmidi         : pureC
isa/drivers/ne2000/ethernec : turboC (1)
isa/drivers/picogus         : m68k-atari-mint-gcc


(1) using existing binaries, not compiling from source.
    to be replaced by own driver at some point.
