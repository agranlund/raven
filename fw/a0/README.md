
The following builds with WinCUPL Atmel version 5.30.4
https://www.microchip.com/en-us/products/fpgas-and-plds/spld-cplds/pld-design-resources

I am using the later ATF1508 fitter v.1918 extracted from the Atmel Prochip package instead
of the old v1878 that comes bundled with WinCUPL.
(You do not need an actual Prochip license, you just want to copy those later version atf150x fitters)

It's probably going to work just fine, but I haven't tested building with the original old fitter.


- G1-U108
    - ATF1508AS  7ns : main bus logic
- G2-U501
    - ATF22V10C 10ns : isa control signals
- G3-U406
    - ATF22V10C 10ns : ym/mfp/ide control signals




