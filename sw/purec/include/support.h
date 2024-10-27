/*
 * support.h
 *  prototypes for miscellaneous functions in the library
 *		++jrb
 */
#ifndef _SUPPORT_H
#define _SUPPORT_H

#ifndef	_FEATURES_H
# include <features.h>
#endif

#include <time.h>	/* for time_t */

__BEGIN_DECLS

int	_unx2dos (const char *, char *, size_t);
int	_dos2unx (const char *, char *, size_t);

int	_path_dos2unx (const char *, char *, size_t);
int	_path_unx2dos (const char *, char *, size_t);

int	unx2dos (const char *, char *);
int	dos2unx (const char *, char *);

time_t	__dostime (time_t);
time_t	__unixtime (unsigned int, unsigned int);

char *	_buffindfile (const char *, const char *,
			char const *const *, char *);

char *	_ultoa (unsigned long, char *, int);
char *	_ltoa (long, char *, int);
#ifndef __NO_LONGLONG
char *	_itoa (unsigned long long int, char *, unsigned int, int);
#endif

__EXITING _exit (int) __NORETURN;

long a64l (const char *);
char *l64a (long);

long tfork (int (*func)(long), long);

int _isctty (int);

int _console_read_byte (int);
void _console_write_byte (int, int);

__END_DECLS

#endif /* _SUPPORT_H */
