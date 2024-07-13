#include "sys.h"

uint32* pmmuRoot;

#define UDT_INVALID     0
#define UDT_RESIDENT    1

#define PDT_INVALID     0
#define PDT_RESIDENT    1
#define PDT_INDIRECT    2

#define UDT_VALID       3
#define PDT_VALID       3

#define PDT_USED        (1 << 3)

#define PMMU_PAGESIZE           (4 * 1024UL)

#define PMMU_INVALID            (0 << 0)
#define PMMU_VALID              (3 << 0)
#define PMMU_USED               (1 << 3)
#define PMMU_SUPER              (1 << 7)
#define PMMU_CM_WRITETHROUGH    (0 << 5)
#define PMMU_CM_COPYBACK        (1 << 5)
#define PMMU_CM_PRECISE         (2 << 5)
#define PMMU_CM_IMPRECISE       (3 << 5)

#define PMMU_INDIRECT           0x2
#define PMMU_READONLY           (PMMU_VALID | PMMU_USED | (1 << 2))
#define PMMU_READWRITE          (PMMU_VALID | PMMU_USED)

#define RESERVED_SIZE			(3 * (1024 * 1024))

#define COPYBACK_STRAM          0
#define COPYBACK_TTRAM          0


extern uint8 kgfx;
uint32* mmuRootTable;
uint32* mmuInvalidPtrTable;
uint32* mmuInvalidPageTable;
uint32* mmuInvalidPage;
uint32* mmuInvalidPageDesc;

extern void SetMMU(TMMU* mmu);
extern void GetMMU(TMMU* mmu);

uint32* pmmu_GetPageDescriptor(uint32 log)
{
    const uint32 ptrTableAddressMask = ~511UL;
    const uint32 pageTableAddressMask = ~255UL;
    //const uint32 pageAddressMask = ~(PMMU_PAGESIZE - 1);
    //uint32 addr = log & ~(PMMU_PAGESIZE - 1);

    uint32 rootIdx = (log >> 25) & 127;
    uint32 ptrIdx  = (log >> 18) & 127;
    uint32 pageIdx = (log >> 12) & 63;

    uint32* ptrTable = (uint32*) (mmuRootTable[rootIdx] & ptrTableAddressMask);
    if (ptrTable == mmuInvalidPtrTable) {
        ptrTable = (uint32*) kmem_Alloc(128 * 4, 512);
        for (int i=0; i<128; i++)
            ptrTable[i] = ((uint32)mmuInvalidPageTable) | PMMU_READWRITE;

        mmuRootTable[rootIdx] = ((uint32)ptrTable) | PMMU_READWRITE;
    }

    uint32* pageTable = (uint32*) (ptrTable[ptrIdx] & pageTableAddressMask);
    if (pageTable == mmuInvalidPageTable) {
        pageTable = (uint32*) kmem_Alloc(64 * 4, 256);
        for (int i=0; i<64; i++)
            pageTable[i] = ((uint32)mmuInvalidPageDesc) | PMMU_INDIRECT;
        ptrTable[ptrIdx] = ((uint32)pageTable) | PMMU_READWRITE;
    }

    return &pageTable[pageIdx];
}

void pmmu_Map(uint32 log, uint32 phys, uint32 size, uint32 flags)
{
    const uint32 pageAddressMask = ~(PMMU_PAGESIZE - 1);
    uint32 end = (log + size + (PMMU_PAGESIZE - 1)) & pageAddressMask;
    uint32 src = log & pageAddressMask;
    uint32 dst = phys & pageAddressMask;

    while (src != end)
    {
        uint32* pageDescriptor = pmmu_GetPageDescriptor(src);
        *pageDescriptor = flags | dst;
        src += PMMU_PAGESIZE;
        dst += PMMU_PAGESIZE;
    }
}

void pmmu_Redirect(uint32 logsrc, uint32 logdst, uint32 size)
{
    const uint32 pageAddressMask = ~(PMMU_PAGESIZE - 1);
    uint32 end = (logsrc + size + (PMMU_PAGESIZE - 1)) & pageAddressMask;
    uint32 src = logsrc & pageAddressMask;
    uint32 dst = logdst & pageAddressMask;

    while (src != end)
    {
        uint32* srcPageDescriptor = pmmu_GetPageDescriptor(src);
        uint32* dstPageDescriptor = pmmu_GetPageDescriptor(dst);
        *srcPageDescriptor = PMMU_INDIRECT | (uint32)dstPageDescriptor;
        src += PMMU_PAGESIZE;
        dst += PMMU_PAGESIZE;
    }
}

void pmmu_Invalid(uint32 log, uint32 size)
{
    const uint32 pageAddressMask = ~(PMMU_PAGESIZE - 1);
    uint32 end = (log + size + (PMMU_PAGESIZE - 1)) & pageAddressMask;
    uint32 src = log & pageAddressMask;

    while (src != end)
    {
        uint32* pageDescriptor = pmmu_GetPageDescriptor(src);
        *pageDescriptor &= ~3UL;
        src += PMMU_PAGESIZE;
    }
}

void pmmu_Init(uint32* simms)
{
    mmuRootTable        = (uint32*) kmem_Alloc(128 * 4, 512);
    mmuInvalidPtrTable  = (uint32*) kmem_Alloc(128 * 4, 512);
    mmuInvalidPageTable = (uint32*) kmem_Alloc( 64 * 4, 256);
    mmuInvalidPageDesc  = (uint32*) kmem_Alloc(4, 4);
    mmuInvalidPage      = (uint32*) kmem_Alloc(PMMU_PAGESIZE, PMMU_PAGESIZE);

    mmuInvalidPageDesc[0] = ((uint32)mmuInvalidPage) | (PMMU_READWRITE | PMMU_CM_WRITETHROUGH);

    for (int i=0; i<PMMU_PAGESIZE/4; i++)
        mmuInvalidPage[i] = 0;

    for (int i=0; i<64; i++)
        mmuInvalidPageTable[i] = ((uint32)mmuInvalidPageDesc) | PMMU_INDIRECT;

    for (int i=0; i<128; i++)
        mmuInvalidPtrTable[i] = ((uint32)mmuInvalidPageTable) | PMMU_READWRITE;

    for (int i=0; i<128; i++)
        mmuRootTable[i] = ((uint32)mmuInvalidPtrTable) | PMMU_READWRITE;

    // Raven peripherals

    extern uint32 _bss_start;
    uint32 stram_size = ((uint32)&_bss_start) & 0xfff00000;
    uint32 reserved_start = stram_size;
    uint32 reserved_end = reserved_start + RESERVED_SIZE;

    uint32 lmem = 0x00000000;
    for (uint32 i=0; i<3; i++)
    {
        uint32 pmem = i << 24;
        for (uint32 j=0; j<(simms[i] >> 20); j++)
        {
			if (pmem < reserved_start || pmem >= reserved_end)
            {
				if (lmem == 0) {
					// system vectors + variables
	                pmmu_Map(0x00000000, 0x00000000, 0x00002000, PMMU_READWRITE | PMMU_CM_PRECISE);
	                pmmu_Map(0x00002000, 0x00002000, 0x000FE000, PMMU_READWRITE | PMMU_CM_WRITETHROUGH);
				}
				else if (lmem >= 0x01000000) {
					// tt-ram
                    #if COPYBACK_TTRAM
	            	    pmmu_Map(lmem, pmem, 0x00100000, PMMU_READWRITE | PMMU_CM_COPYBACK);
                    #else
	            	    pmmu_Map(lmem, pmem, 0x00100000, PMMU_READWRITE | PMMU_CM_WRITETHROUGH);
                    #endif

				}
				else {
					// st-ram
                    #if COPYBACK_STRAM
	                    pmmu_Map(lmem, pmem, 0x00100000, PMMU_READWRITE | PMMU_CM_COPYBACK);
                    #else
	                    pmmu_Map(lmem, pmem, 0x00100000, PMMU_READWRITE | PMMU_CM_WRITETHROUGH);
                    #endif
				}
                lmem += 0x00100000;
            }
            if (lmem == stram_size) {
                lmem = 0x01000000;
            }
            pmem += 0x00100000;
        }
    }

    // internally reserved ram
	for (uint32 addr = reserved_start; addr < reserved_end; addr += 0x00100000) {
	    pmmu_Map(addr, addr, 0x00100000, PMMU_READWRITE | PMMU_CM_WRITETHROUGH);
	}

    // bus error on MB block after tt-ram
    pmmu_Invalid(lmem, 0x00100000);

    // bus error on 64k block at start and end of the MB block after st-ram
    pmmu_Invalid(stram_size, 0x00010000);
    pmmu_Invalid(stram_size + 0x000F0000, 0x00010000);

    // peripheral access flags
    pmmu_Map(0x03000000, 0x03000000, 0x00100000, PMMU_READONLY  | PMMU_CM_WRITETHROUGH);    // ROM
    pmmu_Map(0x20000000, 0x20000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // LOCBUS (uart)
    pmmu_Map(0xA0000000, 0xA0000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS (ide, mfp2)
    pmmu_Map(0xA1000000, 0xA1000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS (ym, mfp1)
    pmmu_Map(0xA2000000, 0xA2000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS (unused)
    pmmu_Map(0xA3000000, 0xA3000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // EXTBUS (unused)

    pmmu_Map(0x80000000, 0x80000000, 0x01000000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA8-RAM
//    pmmu_Map(0x80000000, 0x80000000, 0x00400000, PMMU_READWRITE | PMMU_CM_IMPRECISE);         // ISA8-RAM

    pmmu_Map(0x81000000, 0x81000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA8-IO

    pmmu_Map(0x82000000, 0x82000000, 0x01000000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA16-RAM
//  pmmu_Map(0x82000000, 0x82000000, 0x00400000, PMMU_READWRITE | PMMU_CM_IMPRECISE);         // ISA16-RAM

    pmmu_Map(0x83000000, 0x83000000, 0x00100000, PMMU_READWRITE | PMMU_CM_PRECISE);         // ISA16-IO

    // peripheral memory map
    pmmu_Redirect(0xFF000000, 0x00000000, 0x01000000);  // 24bit space
    pmmu_Redirect(0x00E00000, 0x03040000, 0x00080000);  // ROM
    pmmu_Redirect(0xFFE00000, 0x03040000, 0x00080000);
    pmmu_Redirect(0x00F00000, 0xA0000000, 0x00001000);  // IDE      ($f00000 -> $a0000000)
    pmmu_Redirect(0xFFF00000, 0xA0000000, 0x00001000);
    pmmu_Redirect(0x00FF8000, 0xA1000000, 0x00001000);  // YM2149   ($ff8800 -> $a1000800)
    pmmu_Redirect(0xFFFF8000, 0xA1000000, 0x00001000);
    pmmu_Redirect(0x00FFF000, 0xA1000000, 0x00001000);  // MFP1     ($fffa00 -> $a1000a00)
    pmmu_Redirect(0xFFFFF000, 0xA1000000, 0x00001000);

    // special case graphics card access to satisfy existing Atari drivers
	if (kgfx == 3) {
		// Mach32
	    pmmu_Redirect(0xFE900000, 0x83000000, 0x00100000);      // TT Nova Mach32 reg base : 1024 kb
	    pmmu_Redirect(0xFE800000, 0x82000000, 0x00100000);      // TT Nova Mach32 vga base :  128 kb
	    pmmu_Redirect(0xFEA00000, 0x82200000, 0x00100000);      // TT Nova Mach32 mem base : 2048 kb ?? target addr ??
	    pmmu_Redirect(0xFEB00000, 0x82300000, 0x00100000);      // TT Nova Mach32 mem base : 2048 kb ?? target addr ??

	}
	else if (kgfx == 4) {
		// Mach64
	    pmmu_Redirect(0xFEC00000, 0x83000000, 0x00080000);      // TT Nova Mach32 reg base :  512 kb
	    pmmu_Redirect(0xFEC80000, 0x82000000, 0x00080000);      // TT Nova Mach32 vga base :  512 kb
	    pmmu_Redirect(0xFE800000, 0x82000000, 0x00100000);      // TT Nova Mach32 mem base : 4096 kb ?? target addr ??
	    pmmu_Redirect(0xFE900000, 0x82100000, 0x00100000);      // TT Nova Mach32 mem base : 4096 kb ?? target addr ??
	    pmmu_Redirect(0xFEA00000, 0x82200000, 0x00100000);      // TT Nova Mach32 mem base : 4096 kb ?? target addr ??
	    pmmu_Redirect(0xFEB00000, 0x82300000, 0x00100000);      // TT Nova Mach32 mem base : 4096 kb ?? target addr ??

	}
	else {
		// ET4000
	    pmmu_Invalid(0xFE800000, 0x00100000);
	    pmmu_Invalid(0xFE900000, 0x00100000);
	    pmmu_Redirect(0xFED00000, 0x83000000, 0x00100000);      // TT Nova ET4k reg base    : 1024 kb
	    pmmu_Redirect(0xFEC00000, 0x82000000, 0x00100000);      // TT Nova ET4k mem base	: 1024 kb
	    pmmu_Redirect(0x00D00000, 0x83000000, 0x00100000);      // ST Nova ET4k reg base    : 1024 kb
	    pmmu_Redirect(0x00C00000, 0x82000000, 0x00100000);      // ST Nova ET4k mem base    : 1024 kb
	}

    // hades compatible ISA I/O to take advantage of existing drivers
	pmmu_Redirect(0xFFF30000, 0x81000000, 0x00010000);


    // !! temp !!
    // todo: get rid of this and let the program allocate and map this,
    // after we've exposed the pmmu functions in some kind of bootrom api
	// Emulator space
    pmmu_Redirect(0x04000000, stram_size + 0x00100000, 0x00100000);		// ram	1024kb  : 0x00500000-0x00600000
    pmmu_Redirect(0x04E00000, stram_size + 0x00200000, 0x00040000);		// tos	 256kb
    pmmu_Redirect(0x04FC0000, stram_size + 0x00200000, 0x00030000);
    pmmu_Redirect(0x04FA0000, stram_size + 0x00240000, 0x00020000);		// cart  128kb
    pmmu_Redirect(0x04FF0000, stram_size + 0x00260000, 0x00010000);		// io	  64kb  : 0x00600000-0x00680000

    TMMU mmu;
    mmu.urp = mmuRootTable;
    mmu.srp = mmuRootTable;
    mmu.tcr = 0x8210;
    mmu.itt0 = 0;
    mmu.itt1 = 0;
    mmu.dtt0 = 0;
    mmu.dtt1 = 0;
    SetMMU(&mmu);
}
