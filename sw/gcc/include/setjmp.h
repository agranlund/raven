#ifndef _SETJMP_H_
#define _SETJMP_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef long jmp_buf[6 + 1 + 6 + 8 * 3]; /* 6 data regs, retaddr, 6 addr regs, 8 fpu regs */

int setjmp(jmp_buf buf);
void longjmp(jmp_buf, int);
int sigsetjmp(jmp_buf buf, int mask);

#ifdef __cplusplus
}
#endif

#endif /* _SETJMP_H_ */
