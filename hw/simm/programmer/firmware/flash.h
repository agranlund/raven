#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdbool.h>

bool flash_Init();
bool flash_Identify(uint* mid, uint* did);
uint flash_Read(uint addr);
void flash_Write(uint addr, uint data);
void flash_Erase();
void flash_EraseSector(uint sector);
uint flash_GetSector(uint addr);

const char* flash_ManufacturerString(uint mid);
const char* flash_DeviceString(uint mid, uint did);

extern bool flash_ReadbackError;

#endif // _FLASH_H_
