#ifndef _ISA_PNP_H_
#define _ISA_PNP_H_

#include "isa_core.h"

extern int32_t pnp_init(void);

typedef struct
{
    uint32_t  base_min;
    uint32_t  base_max;
    uint32_t  length;
    uint32_t  align;
    uint16_t  flags;
} pnp_desc_range_t;

typedef struct
{
    uint16_t              flags;
    uint8_t               nio, nmem, nirq, ndma;
    pnp_desc_range_t    iorange[ISA_MAX_DEV_PORT];
    pnp_desc_range_t    memrange[ISA_MAX_DEV_MEM];
    uint32_t              irqmask[ISA_MAX_DEV_IRQ];
    uint16_t              dmamask[ISA_MAX_DEV_DMA];
} pnp_conf_t;



#endif /* _ISA_PNP_H_ */
