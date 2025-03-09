
# ROM module, SMD

![Alt text](images/render.png?raw=true "")

Current revision: [A0](a0/)


Chip suggestions for 1MB:
* SST39LF400A
* SST39LF401C  <-- tested
* SST39LF402C

Chip suggestions for 2MB:
* SST39LF800A
* SST39LF801C
* SST39LF802C


Only SST39LF401C has been tested. The other ones will _probably_ work.
TSOP48 nor flash from other manufacturers might work too but make sure to check the datasheet to establish pinout, timing and programming methods are fully compatible.

The SIMM board as well as Raven supports up to 16MB rom.


Important:

- Revision A0 can only support WORD roms.
BYTE/WORD selectable roms does not work due to pin47 being unconnected.

- As of today, the Raven bus controller assume 55ns or faster ROMS.
This may change in the future to allow a larger selection of rom chips to be used.
In particular, most of the larger ROM chips are 70ns.


## BOM

| Manufacturer          | Part number                 | Value       | Count | Ref                                   |
|-----------------------|-----------------------------|-------------|-------|---------------------------------------|
| Texas Instruments     | SN74LVC2G08DCUR             | 74LVC2G08   | 1     | IC1                                   |
| Microchip             | SST39LF401C-55-4C-EKE-T     | 4Mbit       | 2     | IC2, IC3                              |
| Kemet                 | C1206C104K5RACTU            | 0.1u        | 3     | C1, C2, C3                            |



