#ifndef _RTC_H_
#define _RTC_H_
#ifndef __ASM__
#include "../sys.h"

// Time and date in gemdos format
// yyyyyyymmmmdddddhhhhhmmmmmmsssss
//  y = year        (0-99)
//  m = month       (1-12)
//  d = day         (1-07)
//  h = hour        (0-23)
//  m = minute      (0-59)
//  s = seconds/2   (0-29)

bool        rtc_Init();
void        rtc_SetDateTime(uint32_t dt, uint32_t mask);
uint32_t    rtc_GetDateTime();
void        rtc_GetRam(uint8_t addr, uint8_t* buf, uint8_t siz);
void        rtc_SetRam(uint8_t addr, uint8_t* buf, uint8_t siz);

#endif //!__ASM__
#endif // _RTC_H_

