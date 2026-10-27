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

void        sys_Delay(uint32_t d);
uint32_t    sys_Chipset(void);
rvtoc_t*    sys_GetToc(uint32_t id);

bool        mem_Init();
uint32_t    mem_LinearAlloc(uint32_t size, uint32_t align);
uint32_t    mem_Sbrk(int32_t size);
void        mem_Info(void);

void        vecNMI();
void        vecRTE();

extern uint32_t krev;
extern uint32_t ksimm[4];

extern uint8_t __text_end;
extern uint8_t __data_load;
extern uint8_t __data_start;
extern uint8_t __data_end;
extern uint8_t __bss_start;
extern uint8_t __bss_end;
extern uint8_t __heap_start;
extern uint8_t __heap_end;
extern uint8_t __stack_base;
extern uint8_t __stack_top;

#endif // !__ASM_
#endif // _SYS_H_
