#ifndef _RTC_H_
#define _RTC_H_
#ifndef __ASM__
#include "../sys.h"

#define I2C_DS1307  0x68

// todo: rtc + generic i2c

bool    rtc_Init();


#endif //!__ASM__
#endif // _RTC_H_

