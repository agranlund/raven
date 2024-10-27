/* m68k is big-endian.  */

#ifndef _ENDIAN_H
# error "Never use <bits/endian.h> directly; include <endian.h> instead."
#endif

#ifndef __BYTE_ORDER__
#define __BYTE_ORDER__ __ORDER_BIG_ENDIAN__
#endif
#ifndef __FLOAT_WORD_ORDER__
#define __FLOAT_WORD_ORDER__ __ORDER_BIG_ENDIAN__
#endif
