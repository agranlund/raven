/* defines for spawnve, etc. */

#ifndef _PROCESS_H
#define _PROCESS_H

#ifndef	_FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

#define P_WAIT		0
#define P_NOWAIT	1
#define P_OVERLAY	2

extern int spawnv (int, const char *, char *const *) __THROW;
extern int spawnve (int, const char *, char *const *, char *const *) __THROW;
extern int _spawnve (int, const char *, char *const *, char *const *) __THROW;
extern int spawnvp (int, const char *, char *const *) __THROW;
extern int spawnle (int, const char *, ...) __THROW;
extern int spawnl (int, const char *, ...) __THROW;
extern int spawnlp (int, const char *, ...) __THROW;

#ifdef __USE_PUREC
int exec(const char *filename, const char *cmdlstr, const char *envstr, int *retcode);
#endif

__END_DECLS

#endif /* _PROCESS_H */
