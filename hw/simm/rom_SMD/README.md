
# ROM module, SMD

![Alt text](images/render.png?raw=true "")

Chip suggestions for 2MB:
* SST39LF800A
* SST39LF801C
* SST39LF802C  <-- tested

Chip suggestions for 1MB:
* SST39LF400A
* SST39LF401C  <-- tested
* SST39LF402C

TSOP48 nor flash from other manufacturers might also work but make sure to check the datasheet to establish pinout, timing and programming sequence is fully compatible with SST39.
Board versions before 2025-07-27 does not support flash chips which have byte/word select on pin47. This has since been corrected and could in theory allow a larger variety of different flash chips to be used in the future.

The SIMM board as well as Raven supports up to 16MB rom.

Important:

- As of today, the Raven bus controller assume 55ns or faster ROMS.
This may change in the future to allow a larger selection of rom chips to be used.
In particular, most of the larger ROM chips are 70ns.


## BOM

| Manufacturer          | Part number                 | Value       | Count | Ref                                   |
|-----------------------|-----------------------------|-------------|-------|---------------------------------------|
| Texas Instruments     | SN74LVC2G08DCUR             | 74LVC2G08   | 1     | IC1                                   |
| Microchip             | SST39LF802C-55-4C-EKE       | 8Mbit       | 2     | IC2, IC3                              |
| Kemet                 | C1206C104K5RACTU            | 0.1u        | 3     | C1, C2, C3                            |



