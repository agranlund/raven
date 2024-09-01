
# Nessi : Main bus logic

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


