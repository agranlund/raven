# Raven
### A homemade computer based on Motorola MC68060 + DSP56303
This computer is designed to be an Atari compatible clone. It runs clean GEM/TOS applications natively, and very old hardware-banging ST games through an emulation layer.

Some of the more imporant bits in this slightly disorganized repo:
- [Hardware](hw/raven/a2/) - KiCad schematics and PCB. Interactive BOM, revision history.
- [Firmware](fw/) - [CUPL bus logic](fw/a2/), [CKBD keyboard controller](fw/ckbd/)
- [Software](sw/) - [bootloader and monitor](sw/rom/boot/), [TOS bios extension](sw/rvbios/), [ISA PnP bios and drivers](sw/isa/), [graphics drivers](sw/rvnova/), [sound drivers](sw/rvsnd/), [misc utils](sw/tools/)


## Links and resources:
- Youtube: https://www.youtube.com/@granlund23/videos
- Forum: https://www.exxosforum.co.uk/forum/viewforum.php?f=119

![Alt text](hw/raven/a1/images/desktop.jpg?raw=true "")

![Alt text](hw/raven/a1/images/a1_cased.jpg?raw=true "")


## This is not a consumer product.
I have no formal EE training or education. The computer is primarily designed as a one-off for myself, for fun and learning and as a platform that can provide a whole range of interesting areas of programming and/or hardware tinkering.

All hardware and software sources are provided as-is and for free but comes with no guarantees or promise of support.


The computer works for me and can run EmuTOS, FreeMiNT or MagiC but if you decide to build or buy this board I very much recommend coming in with a DIY mindset -- Knowledge and tools to perform hardware-level debugging and/or very low level software debugging is highly recommended.

The forum is also a good place to get help and ask questions regarding bringup or any other topics.


Rev.A1 is slightly easier to build and has seen 10+ other people building working ones.

Rev.A2 is an incremental update where the largest addition is the DSP56303 but it has only been built and tested by myself so far.


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
- MC68901 MFP x2
- ISA 16bit expansion slots x4


## Required or recommended parts
- Raven motherboard
	- [Revision.A2](hw/raven/a2/)
    - [Revision.A1](hw/raven/a1/)
    	- [USB upgrade](hw/raven/a1/upgrades/ckbd/)
- RAM modules
    - [16MB 55ns](hw/simm/ram_16M55/)
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
    - Cards not listed above have a high chance of working but only in standard VGA resolutions.
- ISA Network card.
	- RTL8019AS based card
- ISA soundcard
	- SoundBlaster compatible
	- Gravis Ultrasound


## Additional tools
- [ROM Simm programmer](hw/simm/programmer/)
- Programmer for ATF1508AS, for example ATDH1150-USB
- Programmer for ATF22V10C, for example XGecu TL866II-Plus
- (Rev.A1 without USB upgrade) Programmer for PIC16F876A, for example XGecu TL866II-Plus

## License

Hardware + CUPL logic are CERN-OHL-2.0. My own software is mostly GPL v2+, 3rd party code keeps its original license. Where a folder differs it'll have its own license file.
