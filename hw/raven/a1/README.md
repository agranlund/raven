# Raven060 Rev.A1

(c)2024 Anders Granlund

This source describes Open Hardware and is licensed under the CERN-OHLW v2.



## Status:
Untested. Boards being manufactured.


## Info:
- Designed for JLCPCB Standard Service 6 layers
- [BOM](production/raven-a1_bom.md)
- [Interactive BOM](https://htmlpreview.github.io/?https://github.com/agranlund/raven/blob/main/hw/raven/a1/production/raven-a1_ibom.html)



## Changelog:
- Changed locations of H607, H609
- Changed IDE ADR: A1:A3 -> A2:A4
- Changed I2C DTA: MFP1_I7 -> MFP1_I3
- Changed I2C CLK: MFP1_I3 -> MFP1_I7
- Added UART IRQ6 (MFP1:I4)
- Added Eiffel thermistor
- Added 3.3V fan header
- Added 5V fan header
- Added 12V fan header
- Added 12V fan header (Eiffel controlled)
- Added CPU speed switch J102, J104
- Added CPLD spare input switch J105
- Added CPLD spare output header J106
- Added CPLD input ISA:IOCS16
- Added expansion header J402 (4xYM outputs, 5V, gnd, i2c)
- Added pullup on CPU RW
- Added pullup on 68150 RW
- Combined CPLD excs/exbusen signals
- Disabled RS232 AutoShutdownPlus
- Removed debug testpoints and solderjumpers
- Removed second audio connector
    - YM-out / Midi-in share same lower connector on J601
    - Non selected is exposed on header J602:J604
- Misc silkscreen & bom fixes


## Optional / non-critical:
- Pushbuttons:     SW101, SW201, SW202
- 3.3V backup:     D201, D202, D203
- Constant fans:   J204, J205, J206
- Eiffel fan:      TH301, R311, U302, J305
- Midi-In:         U601, J604, D601, R609
- Midi-Out:        U602, C605, R611
- ISA -5V:         U201, C202, C203
- ISA OSC:         X501, C514
- Expansion:       J402
- Spare CPLD-In:   J105, R117
- Spare CPLD-Out:  J106
- RTC:             Y402, R402, U406, C413


