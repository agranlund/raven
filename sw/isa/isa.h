/*----------------------------------------------------------------------
 *
 * Atari ISA bus header only library
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
 * cdecl calling convention and 32bit arguments only
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

#ifndef ISA_BIOS_VERSION
#define ISA_BIOS_VERSION    0x0001
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

typedef struct {
    uint32_t    id[ISA_MAX_DEV_IDS];
    uint32_t    mem[ISA_MAX_DEV_MEM];
    uint16_t    port[ISA_MAX_DEV_PORT];
    uint8_t     irq[ISA_MAX_DEV_IRQ];
    uint8_t     dma[ISA_MAX_DEV_DMA];
} isa_dev_t;

typedef struct {
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

    uint32_t    reserved[4];

    uint32_t    _ISA_API (*irq_attach)(uint8l_t irq, void(*func)(void));
    uint32_t    _ISA_API (*irq_remove)(uint8l_t irq, void(*func)(void));

    isa_dev_t*  _ISA_API (*find_dev)(const char* id, uint16l_t idx);

#if 0
    void        _ISA_API (*delayus)(uint32_t us);
    void        _ISA_API (*delayms)(uint32_t ms);
#endif

    uint16_t    numdevs;
    isa_dev_t*  devs;

} isa_t;


/*----------------------------------------------------------------------
 * Small header-only helper library
 * Can work without ISA_BIOS in a very limited fashion
 * 
 * isa_t*       isa_init(void)
 * isa_t*       isa_get(void)
 * void         isa_delay(uint32_t microseconds)
 * void         isa_writeb(uint8_t data)
 * void         isa_writew(uint16_t data)
 * uint8_t      isa_readb(void)
 * uint16_t     isa_readw(void)
 * 
 * MS-DOS compatible interface:
 *
 *  void        outp(uint16_t port, uint8_t data)
 *  void        outpw(uint16_t port, uint8_t data)
 *  uint8_t     inp(uint16_t port)
 *  uint16_t    inpw(uint16_t port)
 * 
 * defines before including header:
 * 
 * ISA_EXCLUDE_LIB          : exclude this stuff
 * ISA_EXCLUDE_LIB_MSDOS    : exclude ms-dos api
 * ISA_EXCLUDE_LIB_FALLBACK : exclude fallback ISA_BIOS
 *--------------------------------------------------------------------*/
#ifndef ISA_EXCLUDE_LIB

#include <mint/cookie.h>
#include <mint/osbind.h>

static isa_t* isa_if = NULL;

#if defined(__GNUC__)
static inline void _isapriv_nop(void) { __asm__ volatile ( "nop\n\t" : : : ); }
#else
static void _isapriv_nop(void) 0x4E71;
#endif

static int32_t _isapriv_delayus_calibrate(void) {
    uint32_t tick_start = *((volatile uint32_t*)0x4ba);
    uint32_t tick_end = tick_start; uint32_t calib = 0;
    do {
        _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop();
        _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop();
        _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop();
        _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop();
        tick_end = *((volatile uint32_t*)0x4ba); calib++;
        if (calib > 1000000UL) { return 1000000UL; }
    } while ((tick_end - tick_start) <= 50);
    return calib;
}

static void isa_delay(uint32_t us) {
    static uint32_t calib = 0L;
    if (!calib) {
        calib = (uint32_t)Supexec(_isapriv_delayus_calibrate);
    } else {
        while (us) {
            uint32_t loops = (us > 1000) ? 1000 : us; us -= loops;
            loops = 1 + ((4 * calib * loops) / (1000 * 1000UL));
            for (; loops; loops--) {
                _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop();
                _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop();
                _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop();
                _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop(); _isapriv_nop();
            }
        }
    }
}

#ifndef ISA_EXCLUDE_LIB_FALLBACK
#if defined(__GNUC__) && (__GNUC__ > 4)
    static inline uint16_t  _isa_swap16(uint16_t data) { return __builtin_bswap16(data); }
#else
    static uint16_t         _isa_swap16(uint16_t data) { return ((data>>8)|(data<<8)); }
#endif
    static isa_t            _isapriv_if_fallback;
    static void             _ISA_API _isa_outp_fallback(uint16l_t port, uint8l_t data)  { *((volatile uint8_t*)(_isapriv_if_fallback.iobase + port)) = (uint8_t)data; }
    static void             _ISA_API _isa_outpw_fallback(uint16l_t port, uint16l_t data) { *((volatile uint16_t*)(_isapriv_if_fallback.iobase + port)) = _isa_swap16(data); }
    static uint8_t          _ISA_API _isa_inp_fallback(uint16l_t port) { return *((volatile uint8_t*)(_isapriv_if_fallback.iobase + port)); }
    static uint16_t         _ISA_API _isa_inpw_fallback(uint16l_t port) { return _isa_swap16(*((volatile uint16_t*)(_isapriv_if_fallback.iobase + port))); }
    static isa_dev_t*       _ISA_API _isa_find_dev_fallback(const char* id, uint16l_t idx) { (void)id; (void)idx; return NULL; }
    static uint32_t         _ISA_API _isa_irq_attach_fallback(uint8l_t irq, void(*func)(void)) { (void)irq; (void)func; return 0; }
    static uint32_t         _ISA_API _isa_irq_remove_fallback(uint8l_t irq, void(*func)(void)) { (void)irq; (void)func; return 0; }
#endif /* ISA_EXCLUDE_LIB_FALLBACK */

_ISA_INL isa_t* isa_init(void) {
    static bool inited = false;
    uint32_t cookie;
    if (!inited) {
        inited = true;
        if ((Getcookie(C__ISA, (long*)&cookie) == 0) && (cookie != 0)) {
            isa_if = (isa_t*)cookie;
        }
#ifndef ISA_EXCLUDE_LIB_FALLBACK
        else {
            /* create some kind of minimal interface when isa_bios is not available */
            /* todo: should we really bother? */
            uint32_t cookie, iobase, membase;
            cookie = iobase = membase = 0;
            if (Getcookie(0x68616465L, (long*)&cookie) == 0) {           /* Hades */
                iobase = 0xFFF30000UL;
                membase = 0xFF000000UL;
            } else if (Getcookie(0x5F4D494CL, (long*)&cookie) == 0) {    /* Milan */
                iobase = 0xC0000000UL;
            } else if (Getcookie(0x502F3249L, (long*)&cookie) == 0) {    /* Panther2 */
                uint32_t* cardpth2 = (uint32_t*) (cookie+6);
                iobase = *cardpth2;
                cardpth2 += 2;
                membase = *cardpth2;
            } else if (Getcookie(0x5241564EL, (long*)&cookie) == 0) {    /* Raven */
                iobase = 0x81000000UL;
                membase = 0x82000000UL;
            }
            if (iobase) {
                isa_if = &_isapriv_if_fallback;
                isa_if->version = ISA_BIOS_VERSION;
                isa_if->iobase = iobase;
                isa_if->membase = membase;
                isa_if->irqmask = 0;
                isa_if->drqmask = 0;
                isa_if->numdevs = 0;
                isa_if->devs = 0;
                isa_if->endian = ISA_ENDIAN_LELS;
                isa_if->outp = _isa_outp_fallback;
                isa_if->outpw = _isa_outpw_fallback;
                isa_if->inp = _isa_inp_fallback;
                isa_if->inpw = _isa_inpw_fallback;
                isa_if->find_dev = _isa_find_dev_fallback;
                isa_if->irq_attach = _isa_irq_attach_fallback;
                isa_if->irq_remove = _isa_irq_remove_fallback;
            }
        }
#endif /* ISA_EXCLUDE_LIB_FALLBACK */
        /* calibrate microsecond delay counter */
        isa_delay(1);
    }
    return isa_if;
}

#define isa_get() isa_init()
_ISA_INL void isa_writeb(uint16_t port, uint8_t data) { isa_t* isa = isa_get(); if (isa) { isa->outp((uint16l_t)port, (uint8l_t)data); } }
_ISA_INL void isa_writew(uint16_t port, uint16_t data) { isa_t* isa = isa_get(); if (isa) { isa->outpw((uint16l_t)port, (uint16l_t)data); } }
_ISA_INL uint8_t isa_readb(uint16_t port) { isa_t* isa = isa_get(); return isa ? isa->inp(port) : 0xff; }
_ISA_INL uint16_t isa_readw(uint16_t port) { isa_t* isa = isa_get(); return isa ? isa_if->inpw(port) : 0xffff; }
_ISA_INL isa_dev_t* isa_find(const char* id, uint16_t idx) { isa_t* isa = isa_get(); return isa ? isa->find_dev(id, idx) : 0; }

/*----------------------------------------------------------------------
 *
 * MS-DOS compatible functions for easy porting
 * 
 *--------------------------------------------------------------------*/
#ifndef ISA_EXCLUDE_LIB_MSDOS
#define outp(p,d)   isa_writeb(p,d)
#define outpw(p,d)  isa_writew(p,d)
#define inp(p)      isa_readb(p)
#define inpw(p)     isa_readw(p)
#endif /* ISA_EXCLUDE_LIB_MSDOS */

#endif /* ISA_EXCLUDE_LIB */

#endif /* _ISA_H_ */
