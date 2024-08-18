#ifndef _CPU_H_
#define _CPU_H_
#include "../sys.h"
#ifndef __ASM__

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
    uint32 buscr;
    uint32 itt1;
    uint32 itt0;
    uint32 dtt1;
    uint32 dtt0;
    uint32 tc;
    uint32 srp;
    uint32 urp;
    uint32 cacr;
    uint32 dfc;
    uint32 sfc;
    uint32 vbr;
    uint32 pcr;
    uint32 usp;
    uint32 d0,d1,d2,d3,d4,d5,d6,d7;
    uint32 a0,a1,a2,a3,a4,a5,a6,a7;
    uint16 sr;
    uint32 pc;
} regs_t;

typedef struct
{
    uint32* urp;
    uint32* srp;
    uint32  tcr;
    uint32  itt0;
    uint32  itt1;
    uint32  dtt0;
    uint32  dtt1;
} mmuregs_t;


//-------------------------------------------------------
// misc
//-------------------------------------------------------
bool        cpu_Init();
uint32      cpu_Detect(uint32* revout, uint32* idout);
void        cpu_Call(uint32 address);

//-------------------------------------------------------
// nmi
//-------------------------------------------------------
typedef void (*NMIFunc_t)(regs_t*);
NMIFunc_t   cpu_SetNMI(NMIFunc_t f);
void        cpu_TriggerNMI();


//-------------------------------------------------------
// pmmu
//-------------------------------------------------------
uint32*     mmu_Init();
void        mmu_Map(uint32 log, uint32 phys, uint32 size, uint32 flags);
void        mmu_Redirect(uint32 logsrc, uint32 logdst, uint32 size);
void        mmu_Invalid(uint32 log, uint32 size);
void        mmu_Flush();

//-------------------------------------------------------
// vbr proxy
//-------------------------------------------------------
bool        vbr_Init();
void        vbr_Set(uint32 vec, uint32 addr);
void        vbr_Apply();

//-------------------------------------------------------
// special register accessors
//-------------------------------------------------------
uint16      cpu_GetIPL();
void        cpu_SetIPL(uint16 v);
uint32      cpu_GetCACR();
void        cpu_SetCACR(uint32 v);
uint32      cpu_GetVBR();
void        cpu_SetVBR(uint32 v);
uint32      cpu_GetPCR();
void        cpu_SetPCR(uint32 v);
void        cpu_GetMMU(mmuregs_t* mmu);
void        cpu_SetMMU(mmuregs_t* mmu);

#endif //!__ASM__
#endif // _CPU_H_

