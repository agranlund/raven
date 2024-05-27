#ifndef _FLASH_H_
#define _FLASH_H_


bool flash_Init();
bool flash_Identify(uint* mid, uint* did);
uint flash_Read(uint addr);
void flash_Write(uint addr, uint data);
void flash_Erase();

const char* flash_ManufacturerString(uint mid);
const char* flash_DeviceString(uint mid, uint did);

#endif // _FLASH_H_
