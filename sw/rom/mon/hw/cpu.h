#ifndef _CPU_H_
#define _CPU_H_
#ifndef __ASM__
#include "sys.h"

//-------------------------------------------------------
#define CPU_UNKONWN             0
#define CPU_68EC060             1
#define CPU_68LC060             2
#define CPU_68060               3

//-------------------------------------------------------
#define PMMU_INVALID            (0 << 0)
#define PMMU_VALID              (3 << 0)
#define PMMU_INDIRECT           (1 << 1)
#define PMMU_USED               (1 << 3)
#define PMMU_SUPER              (1 << 7)
#define PMMU_CM_WRITETHROUGH    (0 << 5)
#define PMMU_CM_COPYBACK        (1 << 5)
#define PMMU_CM_PRECISE         (2 << 5)
#define PMMU_CM_IMPRECISE       (3 << 5)

#define PMMU_READONLY           (PMMU_VALID | PMMU_USED | (1 << 2))
#define PMMU_READWRITE          (PMMU_VALID | PMMU_USED)

//-------------------------------------------------------
typedef struct
{
    uint32_t buscr;
    uint32_t itt1;
    uint32_t itt0;
    uint32_t dtt1;
    uint32_t dtt0;
    uint32_t tc;
    uint32_t srp;
    uint32_t urp;
    uint32_t cacr;
    uint32_t dfc;
    uint32_t sfc;
    uint32_t vbr;
    uint32_t pcr;
    uint32_t usp;
    uint32_t d0,d1,d2,d3,d4,d5,d6,d7;
    uint32_t a0,a1,a2,a3,a4,a5,a6,a7;
    uint16_t sr;
    uint32_t pc;
} regs_t;

typedef struct
{
    uint32_t* urp;
    uint32_t* srp;
    uint32_t  tcr;
    uint32_t  itt0;
    uint32_t  itt1;
    uint32_t  dtt0;
    uint32_t  dtt1;
} mmuregs_t;


//-------------------------------------------------------
// misc
//-------------------------------------------------------
bool        cpu_Init();
uint32_t    cpu_Detect(uint32_t* revout, uint32_t* idout);
void        cpu_Call(uint32_t address);

bool        cpu_Lock(bool* sema);
void        cpu_Unlock(bool* sema);

void        cpu_CacheOn();
void        cpu_CacheOff();
void        cpu_CacheFlush();

//-------------------------------------------------------
// nmi
//-------------------------------------------------------
typedef void (*NMIFunc_t)(regs_t*);
NMIFunc_t   cpu_SetNMI(NMIFunc_t f);
void        cpu_TriggerNMI();


//-------------------------------------------------------
// pmmu
//-------------------------------------------------------
uint32_t*   mmu_Init(uint32_t unmapped_desc);
void        mmu_Map(uint32_t log, uint32_t phys, uint32_t size, uint32_t flags);
void        mmu_Redirect(uint32_t logsrc, uint32_t logdst, uint32_t size);
void        mmu_Invalid(uint32_t log, uint32_t size);
void        mmu_Flush();
uint32_t*   mmu_GetPageDescriptor(uint32_t log);

//-------------------------------------------------------
// vbr proxy
//-------------------------------------------------------
bool        vbr_Init();
void        vbr_Set(uint32_t vec, uint32_t addr);
void        vbr_Apply();

//-------------------------------------------------------
// special register accessors
//-------------------------------------------------------
uint32_t    cpu_GetIPL();
void        cpu_SetIPL(uint32_t v);
uint32_t    cpu_GetCACR();
void        cpu_SetCACR(uint32_t v);
uint32_t    cpu_GetVBR();
void        cpu_SetVBR(uint32_t v);
uint32_t    cpu_GetPCR();
void        cpu_SetPCR(uint32_t v);
void        cpu_GetMMU(mmuregs_t* mmu);
void        cpu_SetMMU(mmuregs_t* mmu);

#endif //!__ASM__
#endif // _CPU_H_

