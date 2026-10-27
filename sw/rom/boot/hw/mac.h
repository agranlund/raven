#ifndef _MAC_H_
#define _MAC_H_

#ifndef __ASM__
#include "sys.h"

bool        mac_Init();
void        mac_Read(uint8_t addr, uint8_t* buf, uint8_t siz);
void        mac_Write(uint8_t addr, uint8_t* buf, uint8_t siz);
bool		mac_Addr(uint8_t (*macaddr)[6]);

#endif //!__ASM__
#endif // _MAC_H_

