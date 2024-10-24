#ifndef _RTC_H_
#define _RTC_H_
#ifndef __ASM__
#include "../sys.h"

#define RTC_RAM_START       0x08
#define RTC_RAM_END         0x40

bool        rtc_Init();
void        rtc_Clear();
void        rtc_Read(uint8_t addr, uint8_t* buf, uint8_t siz);
void        rtc_Write(uint8_t addr, uint8_t* buf, uint8_t siz);

#endif //!__ASM__
#endif // _RTC_H_

