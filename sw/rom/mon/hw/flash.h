#ifndef _FLASH_H_
#define _FLASH_H_
#ifndef __ASM__
#include "sys.h"

bool flash_Init(void);
uint32_t flash_Identify(void);
bool flash_Program(void* data, uint32_t size);

#endif //!__ASM__
#endif // _FLASH_H_
