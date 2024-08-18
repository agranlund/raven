#ifndef _SYS_H_
#define _SYS_H_

//------------------------------------------------
// memory map
//------------------------------------------------
#define PADDR_SIMM0     0x00000000
#define PADDR_SIMM1     0x01000000
#define PADDR_SIMM2     0x02000000
#define PADDR_SIMM3     0x03000000
#define PADDR_UART1     0x20000000
#define PADDR_UART2     0x20000020
#define PADDR_IDE       0xA0000000
#define PADDR_YM        0xA1000800
#define PADDR_MFP1      0xA1000A00
#define PADDR_MFP2      0xA0000A00
#define PADDR_ISA_RAM   0x80000000
#define PADDR_ISA_IO    0x81000000
#define PADDR_GFX_RAM   0x82000000
#define PADDR_GFX_IO    0x83000000


//------------------------------------------------
#ifndef __ASM__

#include "lib.h"

// macros
#define IOB(base,offs)  *((volatile  uint8*)(base + offs))
#define IOW(base,offs)  *((volatile uint16*)(base + offs))
#define IOL(base,offs)  *((volatile uint32*)(base + offs))

static inline void nop() {
    __asm__ __volatile__( "\tnop\n" : : : );
}

bool sys_Init();
uint32 mem_Alloc(uint32 size, uint32 align);

uint32 strtoi(char* s);

void vecNMI();
void vecRTE();

#endif //!__ASM__



#endif // _SYS_H_
