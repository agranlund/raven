# Raven060 Rev.A2

(c)2025 Anders Granlund

This source describes Open Hardware and is licensed under the CERN-OHLW v2.


## Status:
- Work in progress, untested.

## Info:
- Designed for JLCPCB Standard Service 6 layers

## Changelog:
- Silkscreen: Fixed wrong pinout info for J305
- Silkscreen: Reversed order of RAM simms
- Silkscreen: Clearer orientation for a bunch of headers
- Silkscreen: Clearer pin1 markers for a bunch of SMD footprints
- Footprint:  Larger footprints for handsolder D201, D202, D203
- BOM: Replaced PLCC64 socket for U104 with black part
- BOM: Replaced PLCC84 socket for U108 with black part
- BOM: Replaced buttons SW101,SW201,SW202 with black parts
- BOM: Changed C310 to 0.1uF (was 0.01uF)
- Replaced through-hole DS1818 with DS1818R (SMD version)
- Fixed RTC power delivery, now using ATX standby power 
- Added small solderpad for securing Y402 to board
- Added 68150:BUSCS signal separate from 68150:CS
- Added pullups on 68150 data bus
- Added pulldown on IDE:IRQ
- Added i2c eprom with preprogrammed MAC address
- Added i2c expansion header
- Added internal USB header (two ports)
- Added W5100s ethernet controller
- Replaced Eiffel with CKBD
- Replaced 2xPS2 connector with 2xUSB + Ethernet
- CPU temperature sensor to CKBD
- PowerOff control from CKBD
- PowerSwitch status can be read through XR68M752 (detect powerbutton held down during boot)
- Replaced one 12V fan header with PWM + ARGB
- Changed IDE interface to big endian as standard on Atari machines

## Planned:
- get rid of old ym pins expansion header

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
- CPU-Temp:        R123, R124, C129, U109

## Errata:

## Untested:

