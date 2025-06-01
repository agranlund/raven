#ifndef	__SYS_H__
#define __SYS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ch559.h"

#ifndef FREQ_SYS
#define	 FREQ_SYS	48000000ul
#endif
#ifndef BAUD_IKBD
#define BAUD_IKBD   7812
#endif
#ifndef BAUD_DEBUG
#define BAUD_DEBUG  115200
#endif

#define fint16_t     int16_t
#define fuint16_t    uint16_t
#define fint16_min  ((-127) << 8)
#define fint16_max  (( 127) << 8)
#define fuint16_min ((   0) << 8)
#define fuint16_max (( 255) << 8)

#define ABS(x)          (x < 0 ? -x : x)
#define CLAMP(a,b,c)    ((a < b) ? b : (a > c) ? c : a)
#define FIXED16(f)      (256 * (f))
#define SIGNEX(v, sb)   ((v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0))

#define st(x)      do { x } while (__LINE__ == -1)

#define HAL_ENABLE_INTERRUPTS()         st( EA = 1; )
#define HAL_DISABLE_INTERRUPTS()        st( EA = 0; )
#define HAL_INTERRUPTS_ARE_ENABLED()    (EA)

typedef unsigned char halIntState_t;
#define HAL_ENTER_CRITICAL_SECTION(x)   st( x = EA;  HAL_DISABLE_INTERRUPTS(); )
#define HAL_EXIT_CRITICAL_SECTION(x)    st( EA = x; )
#define HAL_CRITICAL_STATEMENT(x)       st( halIntState_t _s; HAL_ENTER_CRITICAL_SECTION(_s); x; HAL_EXIT_CRITICAL_SECTION(_s); )

extern __xdata volatile uint32_t msnow;
extern __xdata volatile uint16_t SoftWatchdog;


int getchar(void);
int putchar(int c);
void delayus(UINT16 n);
void delayms(UINT16 n);
void reset(bool bootloader);

#if 1
uint32_t elapsed(uint32_t from);
#elif 1
// handles wraparound after 49 days uptime
//static inline uint32_t elapsed(uint32_t from) { return (msnow >= from) ? (msnow - from) : (1 + msnow + (0xfffffffful - from)); }
#else
// breaks after 49 days uptime
//static inline uint32_t elapsed(uint32_t from) { return (msnow - from); }
#endif

#if defined(DEBUG)
    #define STRINGIZE(x) STRINGIZE2(x)
    #define STRINGIZE2(x) #x
    #define __LINE_STR__ STRINGIZE(__LINE__)

    #include <stdio.h>
    #define dbg_printf(...) { printf(__VA_ARGS__); printf("\n"); }
    #define dbg_trace(...)  { dbg_printf(__FILE__ ":" __LINE_STR__ ": "__VA_ARGS__); }
#else
    #define dbg_printf(...) { }
    #define dbg_trace(...)  { }
#endif

#define TRACE(...)      { dbg_printf(__VA_ARGS__); }


#if defined(DEBUG) && defined(BOARD_DEVKIT)
    extern void dbg_led(UINT8 led, UINT8 onoff);
    #define DEBUGLED(x,y)   { dbg_led((UINT8)(x),(UINT8)(y)); }
#else
    #define DEBUGLED(x,y)   { }
#endif

#endif /* SYS_H_ */

