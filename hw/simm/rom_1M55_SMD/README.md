
# ROM module, SMD revision A0

![Alt text](images/render.png?raw=true "")

Chip suggestions for 1MB:
* SST39LF400A
* SST39LF401C  <-- tested
* SST39LF402C

Chip suggestions for 2MB:
* SST39LF800A
* SST39LF801C
* SST39LF802C


Only the first one has been tested. The other ones will _probably_ work.
TSOP48 nor flash from other manufacturers could work too but make sure to check the datasheet to establish that the pinout as well as programming methods are fully compatible.
The SIMM board as well as Raven supports up to 16MB rom.

Important:
- This board can only support WORD roms. BYTE/WORD selectable roms does not work due to pin47 being unconnected.
- As of today, the Raven bus controller assume 55ns or faster ROMS.


## BOM

| Manufacturer          | Part number                 | Value       | Count | Ref                                   |
|-----------------------|-----------------------------|-------------|-------|---------------------------------------|
| Texas Instruments     | SN74LVC2G08DCUR             | 74LVC2G08   | 1     | IC1                                   |
| Microchip             | SST39LF401C-55-4C-EKE-T     | 4Mbit       | 2     | IC2, IC3                              |
| Kemet                 | C1206C104K5RACTU            | 0.1u        | 3     | C1, C2, C3                            |



