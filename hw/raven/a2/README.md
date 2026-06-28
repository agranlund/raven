# Raven060 Rev.A2

(c)2025-2026 Anders Granlund

This source describes Open Hardware and is licensed under the CERN-OHLW v2.

![Alt text](images/render.jpg?raw=true "")

## Status:
- Design and routing complete

## Info:
- Designed for JLCPCB standard six layer stackup (JLC06161H-3313)
- [Complete BOM](production/raven-a2_bom.md)
- [Interactive BOM](https://htmlpreview.github.io/?https://github.com/agranlund/raven/blob/main/hw/raven/a2/production/raven-a2_ibom.html)

## Changelog:
- Silkscreen: Fixed wrong pinout info for J305
- Silkscreen: Reversed order of RAM simms
- Silkscreen: Clearer orientation for a bunch of headers
- Silkscreen: Clearer pin1 markers for a bunch of SMD footprints
- Footprint:  Larger footprints for handsolder D201, D202, D203
- BOM: Replaced PLCC64 socket for U104 with black part
- BOM: Replaced PLCC84 socket for U108 with black part
- BOM: Replaced buttons SW101,SW201,SW202 with black parts
- BOM: Replaced jumpers with standard closed-top ones
- BOM: Replaced J401 IDE header
- BOM: Changed C310 to 0.1uF (was 0.01uF)
- Fixed: RTC power delivery, now using ATX standby power
- Fixed: unused but floating inputs on U102
- Fixed: polarity of YM2149 DC blocking caps
- Changed: through-hole DS1818 with DS1818R (SMD version)
- Changed: IDE interface is now big-endian
- Changed: CKBD instead of Eiffel
- Changed: 2xUSB instead of 2xPS2
- Changed: Replaced one 12V fan header with PWM + ARGB
- Removed: UART IRQ6 (MFP1:I4)
- Removed: CPU clock multiplier jumpers
- Added: small solderpad for securing Y402 to board
- Added: 68150:BUSCS signal separate from 68150:CS
- Added: A6 signal instead of A5 to ATF1508 (for BERR on SFP004 access)
- Added: pullups on 68150 data bus
- Added: pulldown on IDE:IRQ
- Added: i2c eprom with preprogrammed MAC address
- Added: i2c expansion header
- Added: CPU temperature readout from CKBD
- Added: PowerOff control from CKBD
- Added: YM2149 sw controlled volume
- Added: internal speaker amp + header
- Added: DSP56303 with DAC+ADC and ESSI1 expansion header

## Optional / non-critical:

## Untested:

## Notes and Errata:
- Part number for ARGB header isn't perfect, might want to use a variant with slightly longer pins.
- PWR_SW to UART:RI trace cut on personal board as pin is not 5V tolerant.
  Schematic, PCB and Gerbers on Github has been updated with same fix.

## Future:
- Remove all components from Analog Devices as they can be hard to purchase for individuals.
  - MAX3245  : ICL3245ECAZ  (drop in replacement)
  - DS1307   : MCP79411     (software changes only, also makes U407+C415 redundant)
  - DS1818R  : MIC6315      (small schematic and board change needed)
  - MAX16054 : tbd          (small schematic and board change needed)
