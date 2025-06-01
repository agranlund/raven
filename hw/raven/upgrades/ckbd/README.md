
# Eiffel upgrade module for Raven.A1
### *** WIP. This particular version has not yet been built and firmware is still in development ***

- Plugs into the Eiffel socket instead of Eiffel.
- Retains support for PS/2 mouse+keyboard, joystick and motherboard temperature sensor.
- Adds USB mouse and keyboard support on an internal USB header.
- Adds support for legacy Atari and Amiga mice (*)
- Adds support for reading CPU temperature (*)
- Adds support for software controlled shutdown (*)

### Firmware:
* binary (todo link binary)
* [sourcecode](../../../../fw/ckbd/) 


![Alt text](ckbd.png?raw=true "")

(*) Features marked with an asterix requires soldering patch wires on the motherboard.
All solder points are available on through hole components and can be done on the underside of the motherboard.

#### Support for right mouse button:
- J606:A9  <-> J606:B6 (mouse right button - joystick1 fire button)
#### Support for CPU temperature readout
- U107:M15 <-> U107:M16 (CPU Therm1 - GND)
- U107:L15 <-> U301:26 (CPU Therm0 - Eiffel:Temp2)
#### Support for software controlled shutdown
- SW201:2 <-> U301:25 (PWR switch - Eiffel:PwrOff)




## BOM

| Manufacturer  | Part number       | Value     | Qty | Ref               | Note |
|---------------|-------------------|-----------|-----|-------------------|------|
| WCH           | CH559L            | CH559L    | 1   | U101              |      |
| Yageo         | RT1206FRE0710KL   | 10K       | 3   | R103,R105,R106    |      |
| Yageo         | RT1206FRE071KL    | 1K        | 3   | R101,R102,R104    |      |
| Kemet         | C1206C105K4RACTU  | 1u        | 2   | C103,C104         |      |
| Kemet         | C1206C104K5RACTU  | 0.1u      | 2   | C102,C105         |      |
| Kemet         | C1206C334K5RACTU  | 0.33u     | 1   | C101              |      |
| ?             | pinheader round   | 1x14      | 2   | J101,J102         | (1)  |
| ?             | pinheader square  | 2x5       | 1   | J103              | (2)  |
| ?             | pinheader square  | 1x4       | 1   | J104              | (2)  |
| TI            | SN74LVC1G34DBVRG4 | 74LVC1G34 | 1   | U102              | (3)  |


(1) J101,J102 needs to be round machined pins that can fit in a dip socket.
40-pin breakaway strips can be found here:
https://www.exxosforum.co.uk/atari/store2/#0132

(2) J103,J104 are standard square pin headers which can be found anywhere.
40-pin breakaway strips can be found here:
https://www.exxosforum.co.uk/atari/store2/#0158

(3) 74LVC1G34 is optional.
It is for an option to drive CH559 from an external oscillator rather than the internal one.
The internal oscillator has been working well for me.


