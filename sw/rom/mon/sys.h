#ifndef _SYS_H_
#define _SYS_H_

//------------------------------------------------
// memory map
//------------------------------------------------
#define PADDR_SIMM0     0x00000000UL
#define PADDR_SIMM1     0x01000000UL
#define PADDR_SIMM2     0x02000000UL
#define PADDR_SIMM3     0x40000000UL
#define PADDR_UART1     0x20000000UL
#define PADDR_UART2     0x20000020UL
#define PADDR_IDE       0xA0000000UL
#define PADDR_YM        0xA1000800UL
#define PADDR_MFP1      0xA1000A00UL
#define PADDR_MFP2      0xA0000A00UL
#define PADDR_ISA_RAM   0x80000000UL
#define PADDR_ISA_IO    0x81000000UL
#define PADDR_GFX_RAM   0x82000000UL
#define PADDR_GFX_IO    0x83000000UL


//------------------------------------------------
#ifndef __ASM__

#include "lib.h"

// macros
#define IOB(base,offs)  *((volatile  uint8_t*)(base + offs))
#define IOW(base,offs)  *((volatile uint16_t*)(base + offs))
#define IOL(base,offs)  *((volatile uint32_t*)(base + offs))

static inline void nop() {
    __asm__ __volatile__( "\tnop\n" : : : );
}

bool sys_Init();
void sys_Delay(uint32_t d);

uint32_t mem_Alloc(uint32_t size, uint32_t align);

uint32_t strtoi(char* s);

void vecNMI();
void vecRTE();


#endif //!__ASM__



#endif // _SYS_H_
