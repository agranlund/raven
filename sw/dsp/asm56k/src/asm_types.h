/*

Project:    asm56k
Author:     M.Buras (sqward)


*/
#ifndef _ASM_TYPES_H_
#define _ASM_TYPES_H_

typedef unsigned int uint;
typedef unsigned char uchar;

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 202311L)
typedef int bool;
#endif
typedef unsigned long long u64;
typedef long long s64;
typedef unsigned long u32;
typedef long s32;
typedef unsigned char u8;
typedef char s8;

#endif /* _ASM_TYPES_H_ */
