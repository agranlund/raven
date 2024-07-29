Raven ROM programmer

Usage:
Connect with a serial terminal and follow instruction.


Building:
- Set up `pico-sdk` according to the Pico's getting-started guide.
- Clone this repository alongside `pico-sdk`
- Create a `build` folder
- From within the `build` folder, type: `cmake ..`
- From within the `build` folder, type: `make`



Todo:
- Automatically erase before programming

- Support Windows line-endings:
In CoolTerm you currently need to change "Enter Key Emulation" to CR instead of CR+LF on the Windows version.

- Fix compatibility with other terminal programs:
It works well with CoolTerm for both Mac and Windows, but file transfer does not work correctly from TeraTerm or RealTerm.

