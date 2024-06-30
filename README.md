# Raven
### A homemade Atari clone computer based on Motorola 68060
This computer is primarily designed to be an Atari clone computer with
the goal of being able to natively run clean GEM/TOS applications.
The machine is fast enough to run hardware-hitting Atari ST games with help from software emulation.

![Alt text](hw/raven/a0/images/raven_a0.jpg?raw=true "")


## This is not a product.
It was designed as a one-off computer for myself for learning and as a platform for low-level programming. 
Thus, things like cost-reduction or other factors that might be important for a product was never high on the priority.

Hardware and software sources are provided as-is and for free but I cannot make any guarantees or promise of support, it's all very much work in progress.
This thread on the Exxos forums is the build log of my machine: https://www.exxosforum.co.uk/forum/viewtopic.php?f=18&t=6940

The computer works and runs EmuTOS + FreeMiNT but if you decide to build or buy this board I very much recommend coming in with a DIY mindset.
It's early days still and waiting on drivers or fixes made by others could very well have you waiting indefinitely.
If you are into that sort of programming then this board could potentially provide a bunch of fun.

## Manual

- [Revision.A1](manual.pdf)

## Specifications

- Motorola 68060 CPU
- Max 48MB RAM
- Max 16MB ROM
- PS/2 Mouse & Keyboard
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


## Required or recommended parts

- Raven motherboard
    - [Revision.A1](hw/raven/a1/)
    - [Revision.A0](hw/raven/a0/)
- RAM modules
    - [16MB 55ns](hw/simm/ram_16M55/)
    - [8MB 10ns](hw/simm/ram_8M10/)
- ROM module
    - [1MB 55ns SMD](hw/simm/rom_16M55/)
    - [2MB 55ns PLCC](hw/simm/rom_2M55/)
- ATX power supply
- PS/2 Keyboard and mouse
- Some kind of 44pin IDE->CF/SD adapter and cable
- ISA graphics card supported by Atari drivers (ET4000AX / ATI Mach32)
- RS232<->USB cable is highly recommended

## Additional tools

- [ROM module programmer](hw/simm/programmer/)
- Programmer for ATF1508AS, for example ATDH1150-USB
- Programmer for ATF22V10C, for example XGecu TL866II-Plus
- Programmer for PIC16F876A, for example XGecu TL866II-Plus

