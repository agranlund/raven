/*----------------------------------------------------------------------
 *
 * Atari ISA bus header only library
 * 
 * todo: fallback interface if isa_bios is not installed
 * 
 *--------------------------------------------------------------------*/
#ifndef _ISA_H_
#define _ISA_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*----------------------------------------------------------------------
 *
 * GCC / PureC compatibility
 * 
 *--------------------------------------------------------------------*/
#ifdef __GNUC__
    typedef unsigned char   uint8l_t;
    typedef unsigned short  uint16l_t;
    #define _ISA_API
    #define _ISA_INL static inline
#else
    typedef unsigned long   uint8l_t;
    typedef unsigned long   uint16l_t;
    #define _ISA_API cdecl
    #define _ISA_INL static
#endif


/*----------------------------------------------------------------------
 *
 * ISA_BIOS cookie interface
 * 
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

typedef struct
{
    uint32_t    id[ISA_MAX_DEV_IDS];
    uint32_t    mem[ISA_MAX_DEV_MEM];
    uint16_t    port[ISA_MAX_DEV_PORT];
    uint8_t     irq[ISA_MAX_DEV_IRQ];
    uint8_t     dma[ISA_MAX_DEV_DMA];
} isa_dev_t;


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


/*----------------------------------------------------------------------
 *
 * Small header-only helper library
 * 
 *--------------------------------------------------------------------*/
#ifndef ISA_EXCLUDE_LIBRARY
#include <mint/cookie.h>

static isa_t* isa_if = NULL;
static bool isa_inited = false;

_ISA_INL isa_t* isa_init_fallback(void) {
    /* here we could init a fallback isa_t interface for known computers
     * that does now have isa_bios installed */
    return NULL;
}

_ISA_INL isa_t* isa_init(void) {
    uint32_t cookie;
    if (!isa_inited) {
        isa_inited = true;
        if (Getcookie(C__ISA, (long*)&cookie) == C_FOUND) {
            isa_if = (isa_t*)cookie;
        } else {
            isa_if = isa_init_fallback();
        }
    }
    return isa_if;
}

_ISA_INL void isa_delay(uint32_t micros) {
    (void)micros;
    /* todo */
}

_ISA_INL void isa_writeb(uint16_t port, uint8_t data) { if (isa_init()) { isa_if->outp(port, data); } }
_ISA_INL void isa_writew(uint16_t port, uint16_t data) { if (isa_init()) { isa_if->outpw(port, data); } }
_ISA_INL uint8_t isa_readb(uint16_t port) { return isa_init() ? isa_if->inp(port) : 0xff; }
_ISA_INL uint16_t isa_readw(uint16_t port) { return isa_init() ? isa_if->inpw(port) : 0xffff; }


/*----------------------------------------------------------------------
 *
 * MS-DOS compatible functions for easy porting
 * 
 *--------------------------------------------------------------------*/
#ifndef ISA_EXCLUDE_MSDOS_LIBRARY
_ISA_INL void outp(uint16_t port, uint8_t data) { isa_writeb(port, data); }
_ISA_INL void outpw(uint16_t port, uint16_t data) { isa_writew(port, data); }
_ISA_INL uint8_t inp(uint16_t port) { return isa_readb(port); }
_ISA_INL uint16_t inpw(uint16_t port) { return isa_readw(port); }
#endif /* ISA_EXCLUDE_MSDOS_LIBRARY */

#endif /* ISA_EXCLUDE_LIBRARY */

#endif /* _ISA_H_ */
