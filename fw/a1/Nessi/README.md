
# Nessi : Main bus logic

- nessi_wip (development version)
    - 

- nessi_251029
    - improved bus timing
    - now works with many different oscillators
    - now works with 1x cpu clock

- nessi_251025
    - ide access fix to solve issue with 250714

- nessi_250714
    - improved compatibility with slower cf-cards
    - version reported as broken on one machine with sd-card adapter

- nessi_250627
    - fix for certain 8bit reads from ISA ram, 16-color vga modes now work.

- nessi_250620
    - reversed order of the ram slots, ram0 is now ram2 and vice versa.

- nessi_240901
    - fixes for Mach32 + W32i

- nessi_240819
    - moved SIMM3 slot to address $40000000

--------

These older versions are not compatible with the latest rom code.
See sw/rom/readme.md for instructions on how to build a ROM that works with these.

- nessi_240812
    - isa: slowed io access to accomodate for PicoGUS
    - isa: fixed io 16bit access to 8bit cards

- nessi_240705
    - First and very timing dependent version. Tested extensively on 96/48 mhz but not many other configs.
