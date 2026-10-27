#ifndef _RTC_H_
#define _RTC_H_

#define RTC_RAM_START       0x08
#define RTC_RAM_END         0x40

#ifndef __ASM__
#include "sys.h"

bool        rtc_Init();
void        rtc_Reset();
void        rtc_ClearRam();
void        rtc_Read(uint8_t addr, uint8_t* buf, uint8_t siz);
void        rtc_Write(uint8_t addr, uint8_t* buf, uint8_t siz);
bool        rtc_Valid();

#endif //!__ASM__
#endif // _RTC_H_

