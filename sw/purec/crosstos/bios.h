#ifndef _BIOS_H_
#define _BIOS_H_

#include <stdint.h>

extern uint32_t bios_dispatch(uint16_t opcode, uint32_t pd);
extern void bios_init(uint8_t* ram, uint32_t ramsize);

#endif /* _XIOS_H_ */
