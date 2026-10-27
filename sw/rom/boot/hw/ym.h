#ifndef _YM_H_
#define _YM_H_
#ifndef __ASM__
#include "sys.h"

bool        ym_Init();
void		ym_Volume(uint8_t vol);
void		ym_Speaker(bool enable);

#endif //!__ASM__
#endif // _YM_H_
