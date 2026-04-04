#ifndef _ISA_BIOS_H_
#define _ISA_BIOS_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define ISA_EXCLUDE_LIB
#include "../isa.h"


/*----------------------------------------------------------------------
 * general config
 *--------------------------------------------------------------------*/

#ifndef ISABIOS_STANDALONE
#define ISABIOS_STANDALONE          (1)
#endif

#ifndef ISABIOS_DEBUG
#define ISABIOS_DEBUG               (0)
#endif

#ifndef ISABIOS_ENABLE_INF
#define ISABIOS_ENABLE_INF          (ISABIOS_STANDALONE)
#endif

#ifndef ISABIOS_ENABLE_LOG_FILE
#define ISABIOS_ENABLE_LOG_FILE     (ISABIOS_STANDALONE)
#endif
#ifndef ISABIOS_ENABLE_LOG_SCREEN
#define ISABIOS_ENABLE_LOG_SCREEN   (ISABIOS_STANDALONE)
#endif
#ifndef ISABIOS_ENABLE_LOG_DEBUG
#define ISABIOS_ENABLE_LOG_DEBUG    (ISABIOS_DEBUG)
#endif

/*----------------------------------------------------------------------
 * platforms
 *--------------------------------------------------------------------*/

#ifndef ISABIOS_MACH_HADES
#define ISABIOS_MACH_HADES      (ISABIOS_STANDALONE)
#endif
#ifndef ISABIOS_MACH_MILAN
#define ISABIOS_MACH_MILAN      (ISABIOS_STANDALONE)
#endif
#ifndef ISABIOS_MACH_PANTHER
#define ISABIOS_MACH_PANTHER    (ISABIOS_STANDALONE)
#endif
#ifndef ISABIOS_MACH_RAVEN
#define ISABIOS_MACH_RAVEN      (ISABIOS_STANDALONE)
#endif

#ifndef C_hade
#define C_hade  0x68616465L     /* 'hade' */
#endif
#ifndef C__MIL
#define C__MIL  0x5F4D494CL     /* '_MIL' */
#endif
#ifndef C__P2I
#define C__P2I  0x502F3249L     /* 'P/2I' */
#endif
#ifndef C_RAVN
#define C_RAVN  0x5241564EL     /* 'RAVN' */
#endif

extern bool isabios_setup_hades(void);
extern bool isabios_setup_milan(void);
extern bool isabios_setup_panther(void);
extern bool isabios_setup_raven(void);


/*----------------------------------------------------------------------
 * 
 *--------------------------------------------------------------------*/
#define ISA_MAX_NAME        64

#define ISA_FLG_INVALID     0x8000
#define ISA_FLG_DISABLED    0x4000
#define ISA_FLG_ACTIVE      0x2000


#include "isa_pnp.h"


typedef struct
{
    struct {
        uint16_t    adport;
        uint16_t    wrport;
        uint16_t    rdport;
        pnp_card_t* cards;
    } pnp;

    isa_t bus;
} isabios_t;


/*----------------------------------------------------------------------
 * globals
 *--------------------------------------------------------------------*/
#define ISA_MAX_IRQFUNCS 14
typedef struct { uint32_t count; uint32_t func[ISA_MAX_IRQFUNCS+1]; } irqlist_t;
extern irqlist_t isa_irq_list[16];
extern void(*isa_irq_assign)(void);
extern isabios_t isa; /* todo: rename */

/*----------------------------------------------------------------------
 * generic helpers
 *--------------------------------------------------------------------*/
#if defined(__GNUC__)
static inline void isabios_nop(void) { __asm__ volatile ( "nop\n\t" : : : ); }
static inline uint16_t isabios_swap16(uint16_t d) { return __builtin_bswap16(d); }
static inline uint32_t isabios_swap32(uint32_t d) { return __builtin_bswap32(d); }
/* todo: interrupt disable/restore*/
#else
static void     isabios_nop(void) 0x4E71;
static uint16_t isabios_swap16(uint16_t d) { return ((d >> 8) & 0xff) | (( d & 0xff) << 8); }
static uint32_t isabios_swap32(uint32_t d) { return (((d >> 24) & 0x000000ffUL) | ((d & 0x000000ffUL) << 24) | ((d >> 8) & 0x0000ff00UL) | ((d & 0x0000ff00UL) << 8) ); }
extern uint16_t isabios_disable_interrupts(void);
extern void     isabios_restore_interrupts(uint16_t);
extern uint32_t isabios_getcacr20(void);
extern uint32_t isabios_getcacr40(void);
extern void     isabios_setcacr20(uint32_t cacr);
extern void     isabios_setcacr40(uint32_t cacr);
#endif

/*----------------------------------------------------------------------
 * isabios utils
 *--------------------------------------------------------------------*/

extern bool     isabios_createcookie(uint32_t id, uint32_t value);

extern void     isabios_mem_init(uint32_t size);
extern void*    isabios_mem_alloc(uint32_t size);
extern void*    isabios_mem_alloc_temp(uint32_t size);
extern void     isabios_mem_close(void);

extern void     isabios_inf_init(void);
extern void     isabios_inf_close(void);

extern void     isabios_log_init(void);
extern void     isabios_log_close(void);

#if ISABIOS_ENABLE_LOG_SCREEN
extern void     isabios_print(char* fmt,...) __attribute__((__format__(__printf__, 1, 2)));
#endif
#if ISABIOS_ENABLE_LOG_FILE
extern void     isabios_log(char* fmt,...) __attribute__((__format__(__printf__, 1, 2)));
#endif
#if ISABIOS_ENABLE_LOG_DEBUG
extern void     isabios_dbg(char* fmt,...) __attribute__((__format__(__printf__, 1, 2)));
#endif


#endif /* _ISA_BIOS_H_ */
