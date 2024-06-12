#ifndef _ISA_PNP_H_
#define _ISA_PNP_H_

#include "isa_core.h"

extern int32 pnp_init();

typedef struct
{
    uint32  base_min;
    uint32  base_max;
    uint32  length;
    uint32  align;
    uint16  flags;
} pnp_desc_range_t;

typedef struct
{
    uint16              flags;
    uint8               nio, nmem, nirq, ndma;
    pnp_desc_range_t    iorange[ISA_MAX_DEV_PORT];
    pnp_desc_range_t    memrange[ISA_MAX_DEV_MEM];
    uint32              irqmask[ISA_MAX_DEV_IRQ];
    uint16              dmamask[ISA_MAX_DEV_DMA];
} pnp_conf_t;



#endif // _ISA_PNP_H_
