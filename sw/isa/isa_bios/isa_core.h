#ifndef _ISA_CORE_H_
#define _ISA_CORE_H_

#include "../isa.h"
#include "mint/cookie.h"

#define ISA_BIOS_VERSION    0x0001

//---------------------------------------------------------------------
// types
//---------------------------------------------------------------------
#ifndef uint8
typedef unsigned char       uint8;
#endif
#ifndef uint16
typedef unsigned short      uint16;
#endif
#ifndef uint32
typedef unsigned int        uint32;
#endif
#ifndef int8
typedef char                int8;
#endif
#ifndef int16
typedef short               int16;
#endif
#ifndef int32
typedef int                 int32;
#endif
#ifndef bool
typedef int                 bool;
#endif
#ifndef true
#define true                1
#endif
#ifndef false
#define false               0
#endif
#ifndef null
#define null                0
#endif



#define ISA_MAX_NAME                64
#define ISA_FLG_ENABLED             0x8000
#define ISA_FLG_PNP                 0x4000


//---------------------------------------------------------------------
// 
//---------------------------------------------------------------------

typedef struct
{
    char        name[ISA_MAX_NAME];
    uint32      id[ISA_MAX_DEV_IDS];
    uint16      flags;
    uint8       csn;
    uint8       ldn;
} isa_device_t;

typedef struct
{
    char            name[ISA_MAX_NAME];
    uint16          flags;
    uint32          vendor;
    uint32          serial;
    uint8           pnp_version;
    uint8           card_version;
    uint8           csn;
    uint8           numdevices;
    isa_device_t    devices[ISA_MAX_CARD_DEVS];
} isa_card_t;

typedef struct
{
    uint16      numcards;
    isa_card_t  cards[ISA_MAX_CARDS];
    isa_t       bus;
} isa_core_t;



//---------------------------------------------------------------------
// cookies
//---------------------------------------------------------------------
#ifndef C_hade
#define C_hade  0x48616465      
#endif
#ifndef C__MIL
#define C__MIL  0x5F4D494C
#endif
#ifndef C_RAVN
#define C_RAVN  0x5241564E
#endif

//---------------------------------------------------------------------
// globals
//---------------------------------------------------------------------
extern isa_core_t isa;

//---------------------------------------------------------------------
// helpers
//---------------------------------------------------------------------
#define ENTER_SUPER()   uint32 sstack = Super(1) ? Super(0) : 0;
#define EXIT_SUPER()    if (sstack) { Super(sstack); }

static inline uint16 swap16(uint16 d) {
    __asm__ volatile ( "ror.w #8,%0\n\t" \
        : "=d"(d) : "0"(d) : ); 
    return d;
}

static inline uint32 swap32(uint32 d) {
    __asm__ volatile (  "ror.w #8,%0\n\t" \
                        "swap %0\n\t" \
                        "ror.w #8,%0\n\t" \
        : "=d"(d) : "0"(d) : ); 
    return d;
}


//---------------------------------------------------------------------
// helper functions
//---------------------------------------------------------------------
extern void delayus(uint32 us);

extern bool Createcookie(uint32 id, uint32 value);
extern void ExitTsr();

extern void OpenFiles();
extern void CloseFiles();

extern const char* IdToStr(uint32 id);
extern uint32 StrToId(const char* str);

extern int StrToInt(const char* s);
extern uint32 StrToHex(const char* s);

extern const char* GetInfStr(const char* key);
extern bool GetInfInt(const char* key, int* val);
extern bool GetInfHex(const char* key, uint32* val);

extern const char* GetInfCommand(const char* find, const char* start, char** outc, int* outs);


extern void Log(char* fmt,...);

#endif // _ISA_CORE_H_
