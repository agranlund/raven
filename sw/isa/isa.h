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
 * Small header-only helper library
 * Can work without ISA_BIOS to a limited degree
 * 
 * bool isa_init(void)
 * void isa_delay(uint32_t microseconds)
 * void isa_writeb(uint8_t data)
 * void isa_writew(uint16_t data)
 * uint8_t isa_readb(void)
 * uint16_t isa_readw(void)
 * 
 * MS-DOS compatible interface:
 *
 *  void outp(uint16_t port, uint8_t data)
 *  void outpw(uint16_t port, uint8_t data)
 *  uint8_t inp(uint16_t port)
 *  uint16_t inpw(uint16_t port)
 * 
 * defines before including header:
 * 
 * ISA_EXCLUDE_LIB          : exclude this stuff
 * ISA_EXCLUDE_LIB_MSDOS    : exclude ms-dos api
 * ISA_EXCLUDE_LIB_FALLBACK : exclude non ISA_BIOS support
 *--------------------------------------------------------------------*/
#ifndef ISA_EXCLUDE_LIB
#include <mint/cookie.h>

static isa_t* isa_if = NULL;
static bool isa_inited = false;

#if defined(__GNUC__)
static inline void _isa_nop(void) { __asm__ volatile ( "nop\n\t" : : : ); }
#else
static void _isa_nop(void) 0x4E71;
#endif

static long _isa_get200hz_super(void) { return *((volatile uint32_t*)0x4ba); }
_ISA_INL uint32_t _isa_get200hz(bool super) { return super ? (uint32_t)_isa_get200hz_super() : (uint32_t)Supexec(_isa_get200hz_super); }
static void _isa_delay(uint32_t microseconds, bool super)
{
    /* calibration */
    static uint32_t loops_count = 0;
    if (loops_count == 0) {
        uint32_t tick_start = _isa_get200hz(super);
        uint32_t tick_end = tick_start;
        do {
            _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); 
            _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); 
            tick_end = _isa_get200hz(super);
            loops_count++;
            if (loops_count > 1000000UL) {
                loops_count = 0xffffffffUL;
                break;
            }
        } while ((tick_end - tick_start) <= 25UL);
    }
    if ((microseconds < 1000) && (loops_count != 0xffffffffUL)) {
        /* microseconds delay using calibration data */
        uint32_t i; uint32_t loops = 1 + ((2 * 4 * loops_count * microseconds) / (1000 * 1000UL));
        for (i=0; i<=loops; i++) {
            _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); 
            _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); _isa_nop(); 
        }
        return;
    } else {
        /* millisecond delay using 200hz counter */
        uint32_t ticks = microseconds / 5000;
        uint32_t start  = _isa_get200hz(super);
        ticks = (ticks < 1) ? 1 : ticks;
        while (1) {
            volatile uint32_t now = _isa_get200hz(super);
            if (now < start) {
                start = now;
            } else if ((now - start) >= ticks) {
                break;
            }
        }
    }
}

static void isa_delay(uint32_t microseconds) { _isa_delay(microseconds, false); }
static void isa_delay_super(uint32_t microseconds) { _isa_delay(microseconds, true); }

#ifndef ISA_EXCLUDE_LIB_FALLBACK
#if defined(__GNUC__)
    static inline uint16_t _isa_swap16(uint16_t data) { return __builtin_bswap16(data); }
#else
    static uint16_t _isa_swap16(uint16_t data) { return ((data>>8)|(data<<8)); }
#endif
    static void     _ISA_API _isa_outp_fallback(uint16l_t port, uint8l_t data)  { *((volatile uint8_t*)(isa_if->iobase + port)) = (uint8_t)data; }
    static void     _ISA_API _isa_outpw_fallback(uint16l_t port, uint16l_t data) { *((volatile uint16_t*)(isa_if->iobase + port)) = _isa_swap16(data); }
    static uint8_t  _ISA_API _isa_inp_fallback(uint16l_t port) { return *((volatile uint8_t*)(isa_if->iobase + port)); }
    static uint16_t _ISA_API _isa_inpw_fallback(uint16l_t port) { return _isa_swap16(*((volatile uint16_t*)(isa_if->iobase + port))); }
    static void     _ISA_API _isa_outp_buf_fallback(uint16l_t port, uint8_t* buf, int32_t count) { int i; for (i = 0; i < count; i++) { _isa_outp_fallback(port, *buf++); } }
    static void     _ISA_API _isa_outpw_buf_fallback(uint16l_t port, uint16_t* buf, int32_t count) { int i; for (i = 0; i < count; i++) { _isa_outpw_fallback(port, *buf++); } }
    static void     _ISA_API _isa_inp_buf_fallback(uint16l_t port, uint8_t* buf, int32_t count) { int i; for (i = 0; i < count; i++) { *buf++ = _isa_inp_fallback(port); } }
    static void     _ISA_API _isa_inpw_buf_fallback(uint16l_t port, uint16_t* buf, int32_t count) { int i; for (i = 0; i < count; i++) { *buf++ = _isa_inp_fallback(port); } }
    static isa_dev_t* _ISA_API _isa_find_dev_fallback(const char* id, uint16l_t idx) { (void)id; (void)idx; return NULL; }
    static uint32_t _ISA_API _isa_irq_set_fallback(uint8l_t irq, uint32_t func) { (void)irq; (void)func; return 0; }
    static uint32_t _ISA_API _isa_irq_en_fallback(uint8l_t irq, uint8l_t en) { (void)irq; (void)en; return 0; }
    static isa_t isa_if_fallback;
#endif /* ISA_EXCLUDE_LIB_FALLBACK */

_ISA_INL isa_t* isa_init(void) {
    uint32_t cookie;
    if (!isa_inited) {
        isa_inited = true;
        if ((Getcookie(C__ISA, (long*)&cookie) == C_FOUND) && (cookie != 0)) {
            isa_if = (isa_t*)cookie;
        }
#ifndef ISA_EXCLUDE_LIB_FALLBACK
        else {
            /* create some kind of minimal interface when isa_bios is not available */
            /* todo: should we really bother? */
            uint32_t cookie, iobase, membase;
            cookie = iobase = membase = 0;
            if (Getcookie(0x68616465L, (long*)&cookie) == C_FOUND) {           /* Hades */
                iobase = 0xFFF30000UL;
                membase = 0xFF000000UL;
            } else if (Getcookie(0x5F4D494CL, (long*)&cookie) == C_FOUND) {    /* Milan */
                iobase = 0xC0000000UL;
            } else if (Getcookie(0x502F3249L, (long*)&cookie) == C_FOUND) {    /* Panther2 */
                uint32_t* cardpth2 = (uint32_t*) (cookie+6);
                iobase = *cardpth2;
                cardpth2 += 2;
                membase = *cardpth2;
            } else if (Getcookie(0x5241564EL, (long*)&cookie) == C_FOUND) {    /* Raven */
                iobase = 0x81000000UL;
                membase = 0x82000000UL;
            }
            if (iobase) {
                isa_if = &isa_if_fallback;
                isa_if->version = ISA_BIOS_VERSION;
                isa_if->iobase = iobase;
                isa_if->membase = membase;
                isa_if->irqmask = 0;
                isa_if->drqmask = 0;
                isa_if->numdevs = 0;
                isa_if->endian = ISA_ENDIAN_LELS;
                isa_if->outp = _isa_outp_fallback;
                isa_if->outpw = _isa_outpw_fallback;
                isa_if->inp = _isa_inp_fallback;
                isa_if->inpw = _isa_inpw_fallback;
                isa_if->outp_buf = _isa_outp_buf_fallback;
                isa_if->outpw_buf = _isa_outpw_buf_fallback;
                isa_if->inp_buf = _isa_inp_buf_fallback;
                isa_if->inpw_buf = _isa_inpw_buf_fallback;
                isa_if->find_dev = _isa_find_dev_fallback;
                isa_if->irq_set = _isa_irq_set_fallback;
                isa_if->irq_en = _isa_irq_en_fallback;
            }
        }
#endif /* ISA_EXCLUDE_LIB_FALLBACK */
    }
    return isa_if;
}

_ISA_INL void isa_writeb(uint16_t port, uint8_t data) { if (isa_init()) { isa_if->outp((uint16l_t)port, (uint8l_t)data); } }
_ISA_INL void isa_writew(uint16_t port, uint16_t data) { if (isa_init()) { isa_if->outpw((uint16l_t)port, (uint16l_t)data); } }
_ISA_INL uint8_t isa_readb(uint16_t port) { return isa_init() ? isa_if->inp(port) : 0xff; }
_ISA_INL uint16_t isa_readw(uint16_t port) { return isa_init() ? isa_if->inpw(port) : 0xffff; }

/*----------------------------------------------------------------------
 *
 * MS-DOS compatible functions for easy porting
 * 
 *--------------------------------------------------------------------*/
#ifndef ISA_EXCLUDE_LIB_MSDOS
_ISA_INL void outp(uint16_t port, uint8_t data) { isa_writeb(port, data); }
_ISA_INL void outpw(uint16_t port, uint16_t data) { isa_writew(port, data); }
_ISA_INL uint8_t inp(uint16_t port) { return isa_readb(port); }
_ISA_INL uint16_t inpw(uint16_t port) { return isa_readw(port); }
#endif /* ISA_EXCLUDE_LIB_MSDOS */

#endif /* ISA_EXCLUDE_LIB */

#endif /* _ISA_H_ */
