# Raven060 Rev.A0

(c)2024 Anders Granlund

This source describes Open Hardware and is licensed under the CERN-OHLW v2.



## Status:
Success: All major features has been tested and works in EmuTOS and FreeMiNT.

Computer runs stable at 96mhz CPU / 48mhz BUS speed.

This is the very first "throw away" prototype and I do not recommend building it.
No efforts will be made to ensure software or firmware developments maintain compatibility with this version.

Revision.A1 should be considered the corrected version of this board.


## Info:
- Designed for JLCPCB Standard Service 6 layers
- [Interactive BOM](https://htmlpreview.github.io/?https://github.com/agranlund/raven/blob/main/hw/raven/a0/production/raven-a0_ibom.html)


## Notes:

#### Untested features:
- Midi in

#### Errors:
- Layout:
	- H607 wrong location
	- H609 wrong location
    - Back connectors too close to lower edge
- Silkscreen:
	- C512 upside down
- BOM:
	- Exclude DNP parts
	- Missing the jumpers

#### Must fix:
- CPU:
	- pullup on RW
	- remove debug solder jumpers
    - remove debug option header
- 68150:
	- pullup on RW
- RS232:
	- U304 : disable AutoShutDownPlus
	- U401 : disable AutoShutDownPlus
	- remove internal FTDI header
- Eiffel:
	- IRQ6 instead of 5 (or both)
	- Hook up Therm and fan controls
- IDE:
	- use A2:4 instead of A1:3 like Atari/Amiga
- YM:
	- Add internal Line-out header
- ISA:
	- route IOCS16 to cpld


#### Nice to have:
- Fan header(s)
- Combine exbusen & excs
- CPU x1/x2 selectable by jumper
- Spare CPLD in/out on header
- pin info on back silkscreen (ym/mfp)

#### Maybe:
- PC speaker header on a spare MFP2 TBO/TCO
- Indicator leds
- Get rid of uart oscillator and derive from bus clock
- Find a spare input pin usable for bootmode select or similar
- Philosophical question, should IDE be kept little-endian like Amiga/PC or changed to big-endian like Atari
- Should the IDE header be kept as a normal male MB pinout or changed to female cable pinout to allow directly plugging in cheap CF card adapters that expects a cable connection

#### Future ideas:
- onboard ethernet
- onboard usb
- Built in amp + speaker?

