# Raven
### A homemade Atari clone computer based on Motorola 68060
This computer is primarily designed to be an Atari clone computer with
the goal of being able to natively run clean GEM/TOS applications, and backward compatibility with old ST games by help of software emulation.

Latest release package: https://github.com/agranlund/raven/releases/tag/Raven.A1.latest

Other resources:
- Youtube: https://www.youtube.com/@granlund23/videos
- Forum: https://www.exxosforum.co.uk/forum/viewforum.php?f=119


![Alt text](hw/raven/a1/images/desktop.jpg?raw=true "")

![Alt text](hw/raven/a1/images/a1_cased.jpg?raw=true "")


## This is not a consumer product.
I have no formal EE training or education and learn as I go.

This computer is primarily designed as a one-off for myself. For fun and learning and as a platform for low-level tinkering. 

All hardware and software sources are provided as-is and for free but comes with no guarantees or promise of support.

The computer works for me and runs EmuTOS, FreeMiNT and MagiC but if you decide to build or buy this board I very much recommend coming in with a DIY mindset.
Knowledge and tools to perform hardware-level debugging and/or very low level software debugging is highly recommended.

Revision.A2 is the latest version and the first board is currently being tested.

Revision.A1 has seen 10+ builds so can be the safer choice in terms of finding support. It is also slightly easier to build due to not having the DSP components.


## Specifications Rev.A2
- Motorola 68060 CPU @ 96Mhz
- Motorola 56303 DSP @ 90Mhz
- Max 48MB RAM
- Max 16MB ROM
- USB Mouse & Keyboard
- 1Mbps serial port
- Legacy Atari serial port
- Legacy Atari parallel port
- Legacy Atari joystick ports
- IDE Harddisk interface
- Realtime clock
- Midi In/Out
- YM2149 sound
- MC68901 mfp
- ISA 16bit expansion slots x4

## Manual
- [Revision.A1](doc/manual_a1.pdf)

## Required or recommended parts
- Raven motherboard
	- [Revision.A2](hw/raven/a2/)
    - [Revision.A1](hw/raven/a1/)
    	- [USB upgrade](hw/raven/a1/upgrades/ckbd/)
    - ~~[Revision.A0](hw/raven/a0/)~~
- RAM modules
    - [16MB 55ns](hw/simm/ram_16M55/)
    - ~~[8MB 10ns](hw/simm/ram_8M10/)~~
- ROM module
    - [55ns SMD](hw/simm/rom_SMD/)
    - [55ns PLCC](hw/simm/rom_PLCC/)
- ATX power supply
- USB keyboard and mouse (PS/2 for Rev.A1 without USB upgrade)
- Some kind of suitable 44pin IDE->CF adapter
- RS232<->USB cable capable of 1Mbps
- ISA graphics card.
    - SVGA with hardware acceleration
        - Cirrus GD5434, GD5429, GD5428, GD5426
        - WDC WD90C33, WD90C31
        - Tseng ET4000/W32i
        - ATI Mach32
    - SVGA without hardware acceleration
        - Tseng ET4000AX
        - (untested) Cirrus GD5424, GD5422
        - (untested) WD90C30
    - Cards not listed above may work but only in standard VGA resolutions.
- ISA Network card.
	- RTL8019AS based card
- ISA soundcard
	- SoundBlaster compatible
	- Gravis Ultrasound


## Additional tools
- [ROM module programmer](hw/simm/programmer/)
- Programmer for ATF1508AS, for example ATDH1150-USB
- Programmer for ATF22V10C, for example XGecu TL866II-Plus
- (Rev.A1) Programmer for PIC16F876A, for example XGecu TL866II-Plus

