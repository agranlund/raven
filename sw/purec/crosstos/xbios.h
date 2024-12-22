#ifndef _XBIOS_H_
#define _XBIOS_H_

#include <stdint.h>

extern uint32_t xbios_dispatch(uint16_t opcode, uint32_t pd);
extern void xbios_init(uint8_t* ram, uint32_t ramsize);

#endif /* _XBIOS_H_ */
