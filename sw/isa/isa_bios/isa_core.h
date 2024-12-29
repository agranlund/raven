#ifndef _ISA_CORE_H_
#define _ISA_CORE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <mint/cookie.h>

#define ISA_EXCLUDE_LIBRARY
#include "../isa.h"

#define ISA_BIOS_VERSION    0x0001

/*----------------------------------------------------------------------
 * 
 *--------------------------------------------------------------------*/
#define ISA_MAX_NAME                64
#define ISA_FLG_ENABLED             0x8000
#define ISA_FLG_PNP                 0x4000


typedef struct
{
    char            name[ISA_MAX_NAME];
    uint32_t        id[ISA_MAX_DEV_IDS];
    uint16_t        flags;
    uint8_t         csn;
    uint8_t         ldn;
} isa_device_t;

typedef struct
{
    char            name[ISA_MAX_NAME];
    uint16_t        flags;
    uint32_t        vendor;
    uint32_t        serial;
    uint8_t         pnp_version;
    uint8_t         card_version;
    uint8_t         csn;
    uint8_t         numdevices;
    isa_device_t    devices[ISA_MAX_CARD_DEVS];
} isa_card_t;

typedef struct
{
    uint16_t        numcards;
    isa_card_t      cards[ISA_MAX_CARDS];
    isa_t           bus;
} isa_core_t;



/*----------------------------------------------------------------------
 * cookies
 *--------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
 * globals
 *--------------------------------------------------------------------*/
extern isa_core_t isa;

/*----------------------------------------------------------------------
 * helpers
 *--------------------------------------------------------------------*/
#define ENTER_SUPER()   uint32_t sstack = Super(1) ? Super(0) : 0;
#define EXIT_SUPER()    if (sstack) { Super(sstack); }


#if defined(__GNUC__) && defined(__builtin_bswap16)
static uint16_t swap16(uint16_t d) { return __builtin_bswap16(d); }
static uint32_t swap32(uint32_t d) { return __builtin_bswap32(d); }
#else
static uint16_t swap16(uint16_t d) { return ((d >> 8) & 0xff) | (( d & 0xff) << 8); }
static uint32_t swap32(uint32_t d) { return (((d >> 24) & 0x000000ffUL) | ((d & 0x000000ffUL) << 24) | ((d >> 8) & 0x0000ff00UL) | ((d & 0x0000ff00UL) << 8) ); }
#endif

/*----------------------------------------------------------------------
 * helper functions
 *--------------------------------------------------------------------*/
extern void delayus(uint32_t us);

extern bool Createcookie(uint32_t id, uint32_t value);
extern void ExitTsr(void);

extern void OpenFiles(void);
extern void CloseFiles(void);

extern const char* IdToStr(uint32_t id);
extern uint32_t StrToId(const char* str);

extern int StrToInt(const char* s);
extern uint32_t StrToHex(const char* s);

extern const char* GetInfStr(const char* key);
extern bool GetInfInt(const char* key, int* val);
extern bool GetInfHex(const char* key, uint32_t* val);

extern const char* GetInfCommand(const char* find, const char* start, char** outc, int* outs);


extern void Log(char* fmt,...) __attribute__((__format__(__printf__, 1, 2)));


#endif /* _ISA_CORE_H_ */
