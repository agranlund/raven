#include "sys.h"
#include "lib.h"

//-----------------------------------------------------------------------
static uint32_t kheapPtrA __attribute__((aligned(4))); // top area
static uint32_t kheapPtrB __attribute__((aligned(4))); // btm area

uint32_t ksimm[4] __attribute__((aligned(4)));
uint32_t krev __attribute__((aligned(4)));

rvsys_t sysvars __attribute__((section(".bss.sysvar")));

//-----------------------------------------------------------------------
uint32_t sys_Chipset(void) {
    return krev;
}

//-----------------------------------------------------------------------
rvtoc_t* sys_GetToc(uint32_t id) {
    extern uint8_t __toc_start;
    rvtoc_t* toc = (rvtoc_t*)&__toc_start;
    while(toc->id) {
        if ((toc->id == id) || (id == 0)) {
            return toc;
        }
        toc++;
    }
    return 0;
}

//-----------------------------------------------------------------------
// split-heap memory.
//  top: linear region that cannot free
//  btm: sbrk-like region which can grow/shrink
//-----------------------------------------------------------------------
bool mem_Init()
{
    // init heap pointers
    kheapPtrA = ((uint32_t)&__heap_end) & ~15;
    kheapPtrB = ((uint32_t)&__heap_start + 15) & ~15UL;
    return true;
}

uint32_t mem_LinearAlloc(uint32_t size, uint32_t alignment)
{
    alignment = (alignment > 4) ? alignment : 4;
    uint32_t m2 = ((kheapPtrA - size) & ~(alignment - 1));
    if (m2 < kheapPtrB) {
        printf("ERROR: mem_LinearAlloc(%d, %d)", size, alignment);
        return 0;
    }
    //printf("alloc %d:%d = %d (%08x, %08x)\n", size, alignment, kheapPtrA - m2, kheapPtrA, m2);
    kheapPtrA = m2;
    return kheapPtrA;
}

uint32_t mem_Sbrk(int32_t size)
{
    uint32_t m1 = kheapPtrB;
    uint32_t m2 = kheapPtrB + size;
    if ((m2 > kheapPtrA) || (m2 < (uint32_t)(&__heap_start))) {
        printf("ERROR: mem_Sbrk(%d)", size);
        return 0;
    }
    kheapPtrB = m2;
    return m1;
}

void mem_Info(void) {
    uint32_t m1,m2;
    m1 = (uint32_t)&__stack_base;
    m2 = (uint32_t)&__stack_top;
    printf("stck %08x %08x : %d\n", m1, m2, m2 - m1);
    m1 = (uint32_t)&__heap_start;
    m2 = (uint32_t)&__heap_end;
    printf("heap %08x %08x : %d\n", m1, m2, m2 - m1);
    printf("ktop %08x %08x : %d\n", kheapPtrA, m2, m2 - kheapPtrA);
    printf("kbtm %08x %08x : %d\n", m1, kheapPtrB, kheapPtrB - m1);
    printf("free %08x %08x : %d\n", kheapPtrB, kheapPtrA, kheapPtrA - kheapPtrB);
}


//-----------------------------------------------------------------------
//
// misc helpers
//
//-----------------------------------------------------------------------
void sys_Delay(uint32_t c) {
    for (uint32_t i = 0; i < (c * 1000); i++) {
        nop();
    }
}
