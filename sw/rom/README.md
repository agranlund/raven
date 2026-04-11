# Raven ROM

## Build complete rom including TOS

- Run "make"
- Program rom simm with "rom_tos.bin"


## Build bios-rom with debug monitor

- Run "make mon"
- Program rom simm with "rom_mon.bin"


## Todo:

- rename mon to bios
- clean separation of atari specific vs raven common
- move some more stuff out of emutos and into rvbios

bios/
atari/
  emutos/
  romdisk/
  cart/


