#ifndef _WAIT_H
#define _WAIT_H

#ifndef _FEATURES_H
# include <features.h>
#endif

#ifndef _SYS_TYPES_H
# include <sys/types.h>
#endif

#ifndef __USE_POSIX
# include <sys/resource.h>
#endif /* __USE_POSIX */

__BEGIN_DECLS

struct __wait {
#ifndef __MSHORT__
        unsigned        junk:16;        /* padding to make it 32 bits */
#endif
        /* MiNT does actually return the exit value in the lower 8 bits, and
           coredump and termsig in the upper bits.  The value is byteswapped
           within the library to make it compliant with most other systems.  */
        unsigned        retcode:8;
        unsigned        coredump:1;
        unsigned        termsig:7;
};

union wait {
        struct __wait   _w;
        int             _i;
};

union __waitp {
        int *__wi;
        union wait *__wu;
};
typedef union wait __union_wait_t;

/* Allow W* to get parameter in POSIX-Style (int) or BSD-Style (union wait)*/
#ifdef _EXPERIMENTAL_WAIT_MACROS
#define __W(x)  ({union{typeof(x) __in; __union_wait_t __out;}__wu; \
                __wu.__in=(x); __wu.__out; })
#define __WP    union __waitp           
#else           
#define __W(x)          (*(__union_wait_t *) &(x))
#define __WP    int *
#endif

#define w_termsig       _w.termsig
#define w_stopsig       _w.retcode
#define w_coredump      _w.coredump
#define w_retcode       _w.retcode

/* I don't know if this next one is right or not */
#define w_status        _i

#define __WSTOPPED      0177    /* fake "signal" for stopped processes */

#ifndef __USE_POSIX
#define WSTOPPED __WSTOPPED
#endif

#define WIFSIGNALED(x)  ((__W(x)._w.termsig != __WSTOPPED) && (__W(x)._w.termsig != 0))
#define WIFSTOPPED(x)   (__W(x)._w.termsig == __WSTOPPED)
#define WIFEXITED(x)    (__W(x)._w.termsig == 0)
#define WIFCOREDUMPED(x) (__W(x)._w.coredump != 0)

#define WSTOPSIG(x)     (__W(x)._w.retcode)
#define WTERMSIG(x)     (__W(x)._w.termsig)
#define WEXITSTATUS(x)  (__W(x)._w.retcode)

#ifdef __USE_BSD
#define WCOREFLAG               0x80
#define WCOREDUMP(status)       (__W(status)._i & WCOREFLAG)
#define W_EXITCODE(ret, sig)    ((ret) << 8 | (sig))
#define W_STOPCODE(sig)         ((sig) << 8 | 0x7f)
#endif

#define WNOHANG         1
#define WUNTRACED       2

#ifdef __USE_BSD
/* Special values for the PID argument to `waitpid' and `wait4'.  */
#define WAIT_ANY        (-1)    /* Any process.  */
#define WAIT_MYPGRP     0       /* Any process in my process group.  */
#endif

extern pid_t wait (__WP status) __THROW;
extern pid_t __wait (__WP status) __THROW;

#ifdef __USE_BSD
struct rusage;  /* Don't depend on sys/resource.h.  */

extern pid_t wait3 (__WP status, int mode, struct rusage *rusage) __THROW;
extern pid_t __wait3 (__WP status, int mode, struct rusage *rusage) __THROW;
extern pid_t wait4 (pid_t pid, __WP status, int options, struct rusage *rusage) __THROW;
extern pid_t __wait4 (pid_t pid, __WP status, int options, struct rusage *rusage) __THROW;
#endif /* __USE_BSD */

extern pid_t waitpid (pid_t pid, int* status, int options) __THROW;
extern pid_t __waitpid (pid_t pid, int* status, int options) __THROW;

__END_DECLS

#endif
