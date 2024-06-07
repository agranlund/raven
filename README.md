# Raven
### A homemade Atari clone computer based on Motorola 68060

![Alt text](hw/raven/a0/images/raven_a0.jpg?raw=true "")

This computer is primarily designed to be an Atari clone computer with
the goal of being able to natively run clean GEM/TOS applications.
The machine is fast enough to run hardware-hitting Atari ST games through partial
software emulation.


Disclaimer: I am not an elecrical engineer and I've designed this computer primarily as a one-off for myself.
I'm sure these things are reflected in the design compared to if someone would have made something like this as a product.

Hardware and software sources are provided as-is and free of charge but I cannot make any guarantees or provide much in terms of support.
This thread on the Exxos forums is the build log of my machine: https://www.exxosforum.co.uk/forum/viewtopic.php?f=18&t=6940

Do not expect this to be a finished and polished end-user product. The software side and driver support is very much a work in progress.
In fact, I can only in good conscience recommend building one of these if you are someone who likes tinkering with hardware and enjoy a bit of low level programming.

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


## What parts would you need?

- Raven motherboard
    - [Revision.A0](hw/raven/a0/)
    - [Revision.A1](hw/raven/a0/)
- ROM module
    - [SMD](hw/simm/rom_16M55/)
    - [PLCC](hw/simm/rom_2M55/)
- RAM modules
    - [16MB 55ns](hw/simm/ram_16M55/)
    - [8MB 10ns](hw/simm/ram_8M10/)
- ATX power supply
- PS/2 Keyboard and mouse
- Some kind of 44pin IDE->CF/SD adapter and cable
- ISA graphics card supported by Atari drivers (ET4000AX / ATI Mach32)
- RS232->USB cable is highly recommended


## Additional tools

- [ROM module programmer](hw/simm/programmer/)
- JTAG programmer for ATF1508AS, such as ATDH1150-USB
- Programmer for ATF22V10 chips, such as XGecu TL866
- Programmer for PIC16F chips, such as XGecu TL866

