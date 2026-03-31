# Raven
### A homemade Atari clone computer based on Motorola 68060
This computer is primarily designed to be an Atari clone computer with
the goal of being able to natively run clean GEM/TOS applications, and backward compatibility with old ST games by help of software emulation.

Latest release package: https://github.com/agranlund/raven/releases/tag/Raven.A1.latest

Other resources:
- Youtube: https://www.youtube.com/@granlund23/videos
- Forum: https://www.exxosforum.co.uk/forum/viewforum.php?f=119


![Alt text](hw/raven/a1/images/desktop.jpg?raw=true "")

![Alt text](hw/raven/a0/images/raven_a0.jpg?raw=true "")


## This is not a consumer product.
This computer was designed as a one-off creation for myself, for learning and as a platform for low-level tinkering. 
Things like cost-reduction or other factors that might be important for a product was never high on the priority.

Hardware and software sources are provided as-is and for free but comes with no guarantees or promise of support.


The computer works for me and runs EmuTOS + FreeMiNT but if you decide to build or buy this board I very much recommend coming in with a DIY mindset.
It's early days still and waiting on drivers or fixes made by others could very well have you waiting indefinitely.
If you are into hardware tinkering or low-level programming then this board could potentially provide a bunch of fun.

Knowledge and tools to perform hardware-level debugging and/or very low level software debugging is highly recommended.

It is easy to make misstakes during assembly and such skills can be vital for finding those. There is also always the risk that something out-of-the-box isn't quite right for your particular set of components - this board happening to working for me doesn't necessarily guarantee being production ready and working for all.



## Specifications
- Motorola 68060 CPU
- Max 48MB RAM
- Max 16MB ROM
- PS/2 Mouse & Keyboard
- USB Mouse & Keyboard (with upgrade module)
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
    - [Revision.A1](hw/raven/a1/)
    - ~~[Revision.A0](hw/raven/a0/)~~
- RAM modules
    - [16MB 55ns](hw/simm/ram_16M55/)
    - ~~[8MB 10ns](hw/simm/ram_8M10/)~~
- ROM module
    - [55ns SMD](hw/simm/rom_SMD/)
    - [55ns PLCC](hw/simm/rom_PLCC/)
- USB upgrade module
    - [ckbd rev.d](hw/raven/upgrades/ckbd/)
- ATX power supply
- PS/2 Keyboard and mouse, or USB if using the USB upgrade module.
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
    
## Additional tools
- [ROM module programmer](hw/simm/programmer/)
- Programmer for ATF1508AS, for example ATDH1150-USB
- Programmer for ATF22V10C, for example XGecu TL866II-Plus
- Programmer for PIC16F876A, for example XGecu TL866II-Plus

