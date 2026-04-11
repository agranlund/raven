#ifndef _FLASH_H_
#define _FLASH_H_
#ifndef __ASM__
#include "sys.h"

bool flash_Init(void);

uint32_t flash_Id(void);
uint32_t flash_Size(void);
const char* flash_Name(void);

// reprogram rom image
// clear + program + reset, keeps old pram sector intact
bool flash_Program(void* data, uint32_t size);

// erase portion of flash (pram utility)
// is using sector-erase internally so beware of alignment and size
bool flash_SectorErase(uint32_t* ptr, uint32_t size);

// write data but does not erase beforehand (pram utility)
// writes longwords at a time
bool flash_Write(uint32_t* ptr, uint32_t size, uint32_t* data);

#endif //!__ASM__
#endif // _FLASH_H_
