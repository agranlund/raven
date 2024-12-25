#ifndef _ISA_H_
#define _ISA_H_

#include <stddef.h>
#include <stdint.h>

/*----------------------------------------------------------------------
 * Constants
 *--------------------------------------------------------------------*/
#ifndef C__ISA
#define C__ISA  0x5F495341UL        /* '_ISA' */
#endif
#ifndef C__PNP
#define C__PNP  0x5F504E50UL        /* '_PNP' */
#endif

#define ISA_ENDIAN_BE       0       /* Big endian                       */
#define ISA_ENDIAN_LEAS     1       /* Little endian: Address swapped   */
#define ISA_ENDIAN_LELS     2       /* Little endian: Lane swapped      */

#define ISA_MAX_CARDS       8
#define ISA_MAX_CARD_DEVS   5
#define ISA_MAX_DEVS        (ISA_MAX_CARDS * ISA_MAX_CARD_DEVS)

#define ISA_MAX_DEV_IDS     5
#define ISA_MAX_DEV_PORT    8
#define ISA_MAX_DEV_MEM     4
#define ISA_MAX_DEV_IRQ     2
#define ISA_MAX_DEV_DMA     2

/*----------------------------------------------------------------------
 * Device
 *--------------------------------------------------------------------*/
typedef struct
{
    uint32_t    id[ISA_MAX_DEV_IDS];
    uint32_t    mem[ISA_MAX_DEV_MEM];
    uint16_t    port[ISA_MAX_DEV_PORT];
    uint8_t     irq[ISA_MAX_DEV_IRQ];
    uint8_t     dma[ISA_MAX_DEV_DMA];
} isa_dev_t;


/*----------------------------------------------------------------------
 * Bus interface
 * GCC and PureC compatible
 *--------------------------------------------------------------------*/

#ifdef __GNUC__
    typedef unsigned char   uint8l_t;
    typedef unsigned short  uint16l_t;
    #define _ISA_API
#else
    typedef unsigned long   uint8l_t;
    typedef unsigned long   uint16l_t;
    #define _ISA_API        cdecl
#endif

typedef struct
{
    uint16_t    version;
    uint32_t    iobase;
    uint32_t    membase;
    uint16_t    irqmask;
    uint8_t     drqmask;
    uint8_t     endian;

    void        _ISA_API (*outp)(uint16l_t port, uint8l_t data);
    void        _ISA_API (*outpw)(uint16l_t port, uint16l_t data);
    uint8_t     _ISA_API (*inp)(uint16l_t port);
    uint16_t    _ISA_API (*inpw)(uint16l_t addr);

    void        _ISA_API (*outp_buf)(uint16l_t port, uint8_t* buf, int32_t count);
    void        _ISA_API (*outpw_buf)(uint16l_t port, uint16_t* buf, int32_t count);
    void        _ISA_API (*inp_buf)(uint16l_t port, uint8_t* buf, int32_t count);
    void        _ISA_API (*inpw_buf)(uint16l_t port, uint16_t* buf, int32_t count);

    uint32_t    _ISA_API (*irq_set)(uint8l_t irq, uint32_t func);
    uint32_t    _ISA_API (*irq_en)(uint8l_t irq, uint8l_t enabled);

    isa_dev_t*  _ISA_API (*find_dev)(const char* id, uint16l_t idx);

#if 0
    void        _ISA_API (*delayus)(uint32_t us);
    void        _ISA_API (*delayms)(uint32_t ms);
#endif

    uint16_t    numdevs;
    isa_dev_t   devs[ISA_MAX_DEVS];

} isa_t;


#endif /* _ISA_H_ */

