# Raven060 Rev.A2

(c)2024 Anders Granlund

This source describes Open Hardware and is licensed under the CERN-OHLW v2.



## Status:
- Work in progress, untested.


## Info:
- Designed for JLCPCB Standard Service 6 layers


## Changelog:
- Silkscreen: Fixed wrong pinout info for J305
- Silkscreen: Clearer orientation for J103 and J401
- Silkscreen: Clearer orientation for J204, J205, J206, J305
- Silkscreen: Clearer orientation for U601
- Footprint:  Larger footprints for handsolder D201, D202, D203 
- BOM: Replaced PLCC64 socket for U104 with black part
- BOM: Replaced PLCC84 socket for U108 with black part
- BOM: Replaced buttons SW101,SW201,SW202 with black parts
- BOM: Changed C310 to 0.1uF (was 0.01uF)



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


## Errata:


## Untested:


## Todo:

- route spare uart1 GPI to jumpers for bios settings
  (CTSA, DSRA, CDA, RIA)
- route spare uart1 GPO to jumper?
  (RTSA)
- uart1 interrupts, use RXRDYA instead of or in combination with /IRQ ?
- pullups on 68150 databus
- IDE connector:
  - 40 instead of 44 pin?
  - or female idc pinout on motherboard?

