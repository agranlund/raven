
- Nessi
    - ATF1508AS  7ns : main bus logic
- Narnia
    - ATF22V10C 10ns : isa control signals
- Iksi
    - ATF22V10C 10ns : ym/mfp/ide control signals


Builds with WinCUPL Atmel version 5.30.4
https://www.microchip.com/en-us/products/fpgas-and-plds/spld-cplds/pld-design-resources

I am using the newer ATF1508 fitter v1918 extracted from the Atmel Prochip package.
It's probably going to work fine with the original old version but I have not tested it.

ATF1508AS can be programmed in-place using ATMISP software and ATDH1150USB programming cable.
Both of the ATF22V10C chips needs to be programmed off-board in an XGecu-TL866 or similar.


