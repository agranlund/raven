#ifndef _SYS_H_
#define _SYS_H_

#ifndef __ASM__

#include "lib.h"
#include "raven.h"

#define initprint(x)    puts(x)

#define IOB(base,offs)  *((volatile  uint8_t*)(base + offs))
#define IOW(base,offs)  *((volatile uint16_t*)(base + offs))
#define IOL(base,offs)  *((volatile uint32_t*)(base + offs))

static inline void nop() {
    __asm__ __volatile__( "\tnop\n" : : : );
}

bool sys_Init();
void sys_Delay(uint32_t d);

uint32_t sys_Chipset(void);
rvtoc_t* sys_GetToc(uint32_t id);
rvcfg_t* sys_GetCfg(uint32_t id);

uint32_t mem_Alloc(uint32_t size, uint32_t align);

uint32_t strtoi(char* s);

void vecNMI();
void vecRTE();

#endif // !__ASM_
#endif // _SYS_H_
