/* 
 * Definitions for getting information about a filesystem.
 */

#ifndef	_SYS_STATFS_H
#define	_SYS_STATFS_H	1

#ifndef	_FEATURES_H
# include <features.h>
#endif

/* Get the system-specific definition of `struct statfs'.  */
#include <bits/statfs.h>

__BEGIN_DECLS

/* Return information about the filesystem on which FILE resides.  */
#ifndef __USE_FILE_OFFSET64
extern int statfs (const char *__file, struct statfs *__buf)
     __THROW __nonnull ((1, 2));
#else
# ifdef __REDIRECT_NTH
extern int __REDIRECT_NTH (statfs,
			   (const char *__file, struct statfs *__buf),
			   statfs64) __nonnull ((1, 2));
# else
#  define statfs statfs64
# endif
#endif
#ifdef __USE_LARGEFILE64
extern int statfs64 (const char *__file, struct statfs64 *__buf)
     __THROW __nonnull ((1, 2));
#endif

/* Return information about the filesystem containing the file FILDES
   refers to.  */
#ifndef __USE_FILE_OFFSET64
extern int fstatfs (int __fildes, struct statfs *__buf)
     __THROW __nonnull ((2));
#else
# ifdef __REDIRECT_NTH
extern int __REDIRECT_NTH (fstatfs, (int __fildes, struct statfs *__buf),
			   fstatfs64) __nonnull ((2));
# else
#  define fstatfs fstatfs64
# endif
#endif
#ifdef __USE_LARGEFILE64
extern int fstatfs64 (int __fildes, struct statfs64 *__buf)
     __THROW __nonnull ((2));
#endif

__END_DECLS

#endif	/* sys/statfs.h */
