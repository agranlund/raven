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
- Small pad for soldering top of Y402 to board
- Added pullups on MC68150 data bus

## Planned:
- replace 2xPS2 connector with 2xUSB+Ethernet
- ch559 based eiffel replacement
- w5100s ethernet controller
- i2c eprom with preprogrammed mac address
- i2c expansion header
- get rid of old ym pins expansion header
- vs1053 dsp, or suitable expansion header to allow it as expansion later
- cpu temperature sensor
- Status of power button can be read from cpu
- software controlled power-off (gpio pin tbd)
- separate cs signals for 68150 and buffers
- (maybe) change IDE connector to big endian byte order

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

