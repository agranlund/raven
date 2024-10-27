
#ifndef _SYS_FILE_H
# define _SYS_FILE_H 1

#ifndef _FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

#ifdef __MINT__

/* lockf() comands */
#define LOCK_SH	1	/* shared lock */
#define LOCK_EX	2	/* exclusive lock */
#define LOCK_NB	4	/* non-blocking */
#define LOCK_UN	8	/* unlock */

#define FREAD  1
#define FWRITE 2

extern int flock (int, int) __THROW;
extern int __flock (int, int) __THROW;

#endif /* __MINT__ */

__END_DECLS

#include <fcntl.h>

#endif /* _SYS_FILE_H */
