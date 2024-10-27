
#ifndef _MINIMAL_H
#define _MINIMAL_H

#ifndef	_FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

int errno;

#ifndef __MINT__
int _console_dev = 2;

extern __EXITING __exit (long) __NORETURN; /* def in crt0.c */
int raise (int sig);
void _exit (int status);

void _exit(int status) { __exit((long)status); }

int raise (int sig) { return 0; }

#else

extern __EXITING _exit (int) __NORETURN;
#define __exit _exit

#endif /* __MINT__ */

void _init_signal (void);
void exit (int status);
void _main (long __argc, char **__argv, char **__environ);
void __main (void);


void _init_signal (void) { }

__EXITING exit (int status) { __exit(status); }

void __main (void) { }

void _main (long __argc, char **__argv, char **__environ)
{
	/* Avoid prototyping main, it will conflict with user's version */
	extern int main ();
	exit(main((int)__argc, __argv, __environ));
}

__END_DECLS

#endif /* _MINIMAL_H */
