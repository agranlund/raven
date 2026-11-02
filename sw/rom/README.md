# Raven ROM

|||
|------|----------|
|`boot/`|bootrom|
|`srec/`|baremetal programs that can be sent over serial to run in bootrom monitor, not part of roms|
|`emutos/`|TOS rom|
|`romdisk/`|TOS romdisk|
|`toscart/`|TOS virtual cartridge skeleton|


## Layout

`mkrom` is a tool which pieces together a full image from individual binaries.

bootrom must always be at the start of the image and other contents can be appended in any order.
The TOC lives in the bootrom area and is updated automatically when binaries are added with mkrom.


## Build complete rom including TOS

- Run "make"
- Program ROM simm with "rom_tos.bin"


## Build bootloader + monitor rom only

- Run "make mon"
- Program ROM simm with "rom_mon.bin"
