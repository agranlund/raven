# Raven060 Rev.A0

(c)2024 Anders Granlund

This source describes Open Hardware and is licensed under the CERN-OHLW v2

You may redistribute and modify this documentation and make products
using it under the terms of the CERN-OHL-W v2 (https:/cern.ch/cern-ohl).

This documentation is distributed WITHOUT ANY EXPRESS OR IMPLIED
WARRANTY, INCLUDING OF MERCHANTABILITY, SATISFACTORY QUALITY
AND FITNESS FOR A PARTICULAR PURPOSE. Please see the CERN-OHL-W v2
for applicable conditions.


## Status:
All major features has been tested and works in EmuTOS and FreeMiNT.

This is the first prototype and I do not recommend building it as more recent software development will be incompatible with parts of this board.

Revision.A1 should be considered instead as the corrected version of this board.

![Alt text](images/raven_a0.jpg?raw=true "")


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

