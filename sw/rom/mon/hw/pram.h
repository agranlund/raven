#ifndef _PRAM_H_
#define _PRAM_H_
#ifndef __ASM__
#include "sys.h"
#include "flash.h"

bool pram_Init(void);
void pram_Clear(void);

uint16_t pram_Get(uint8_t idx);
void pram_Set(uint8_t idx, uint16_t val);
void pram_Compact(void);
bool pram_GetIfExist(uint8_t idx, uint16_t* val);
void pram_Capacity(uint32_t* total, uint32_t* used);

#endif //!__ASM__
#endif // _FLASH_H_
