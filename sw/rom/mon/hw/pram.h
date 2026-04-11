#ifndef _PRAM_H_
#define _PRAM_H_

#define PRAM_MAGIC 0x50524D01  /* 'PRM' + version */

#define PRAM_IDX_BAUDRATE   0x01


#ifndef __ASM__
#include "sys.h"
#include "flash.h"

bool pram_Init(void);
void pram_Clear(void);

uint16_t pram_Get(uint8_t idx);
void pram_Set(uint8_t idx, uint16_t val);

uint32_t param_Remain(void);
void pram_Capacity(uint32_t* total, uint32_t* used);
void pram_Compact(void);

bool pram_GetIfExist(uint8_t idx, uint16_t* val);

#endif //!__ASM__
#endif // _FLASH_H_
