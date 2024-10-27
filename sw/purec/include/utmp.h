/* Copyright (C) 1993, 1996, 1997 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Modified for MiNTLib by Guido Flohr, <guido@freemint.de>,
   Sep 12 1999.  */

/* This is a little tricky.  The old and new utmp interface conflict.
   If you want to use the old interface define "_OLD_UTMP", if you
   want to use the new one, don't.  If you need both you have to
   split your sources into two files because you cannot use the
   two interfaces in parallel in one module.  This is mainly caused
   because "ut_time" is again for backwards compatibility within the
   new interface defined to "ut_time.tv_sec".  */

#ifndef _OLD_UTMP_H
# ifdef _OLD_UTMP
#  ifdef _UTMP_H
#   error You cannot use the old and the new utmp interface in parallel.
#  endif
/* We don't define _OLD_UTMP_H here on purpose!  */
#  define _UTMP_H 1
# endif
#endif

#ifndef _UTMP_H
# define	_UTMP_H	1

# ifdef _OLD_UTMP_H
#  error You cannot use the old and the new utmp interface in parallel.
# endif

/* Prevent the compatibility stuff to get included.  */
# define _OLD_UTMP_H 1

#ifndef _FEATURES_H
# include <features.h>
#endif

#ifndef _SYS_TYPES_H
# include <sys/types.h>
#endif

__BEGIN_DECLS

/* Get system dependent values and data structures.  */
#include <utmpbits.h>

/* Compatibility names for the strings of the canonical file names.  */
#define UTMP_FILE	_PATH_UTMP
#define UTMP_FILENAME	_PATH_UTMP
#define WTMP_FILE	_PATH_WTMP
#define WTMP_FILENAME	_PATH_WTMP

/* Make FD be the controlling terminal, stdin, stdout, and stderr;
   then close FD.  Returns 0 on success, nonzero on error.  */
extern int login_tty (int __fd);

/* Write the given entry into utmp and wtmp.  */
extern void login (const struct utmp *__entry);

/* Write the utmp entry to say the user on UT_LINE has logged out.  */
extern int logout (const char *__ut_line);

/* Append to wtmp an entry for the current time and the given info.  */
extern void logwtmp (const char *__ut_line, const char *__ut_name,
		     const char *__ut_host);

/* Append entry UTMP to the wtmp-like file WTMP_FILE.  */
extern void updwtmp (const char *__wtmp_file,
		     const struct utmp *__utmp);

/* Change name of the utmp file to be examined.  */
extern int utmpname (const char *__file);

/* Read next entry from a utmp-like file.  */
extern struct utmp *getutent (void);

/* Reset the input stream to the beginning of the file.  */
extern void setutent (void);

/* Close the current open file.  */
extern void endutent (void);

/* Search forward from the current point in the utmp file until the
   next entry with a ut_type matching ID->ut_type.  */
extern struct utmp *getutid (const struct utmp *__id);

/* Search forward from the current point in the utmp file until the
   next entry with a ut_line matching LINE->ut_line.  */
extern struct utmp *getutline (const struct utmp *__line);

/* Write out entry pointed to by UTMP_PTR into the utmp file.  */
extern struct utmp *pututline (const struct utmp *__utmp_ptr);


#ifdef	__USE_MISC
/* Reentrant versions of the file for handling utmp files.  */
extern int getutent_r (struct utmp *__buffer, struct utmp **__result);
extern int getutid_r (const struct utmp *__id, struct utmp *__buffer, struct utmp **__result);
extern int getutline_r (const struct utmp *__line, struct utmp *__buffer, struct utmp **__result);

#endif	/* Use misc.  */

__END_DECLS

#endif /* !_UTMP_H */

/* This is the backwards compatibility stuff.  */
#ifndef _OLD_UTMP_H
# define _OLD_UTMP_H 1

#include <features.h>

/* This is for backwards compatibility.  Use these names if you need to
   compile old sources and let oldutmpd convert your entries.  */

#ifdef UTMP_FILE
# undef UTMP_FILE
#endif
#ifdef WTMP_FILE
# undef WTMP_FILE
#endif
#ifdef UTMP_FILENAME
# undef UTMP_FILENAME
#endif
#ifdef WTMP_FILENAME
# undef WTMP_FILENAME
#endif
#define UTMP_FILE     "/etc/utmp"
#define WTMP_FILE     "/var/adm/wtmp"
#define UTMP_FILENAME UTMP_FILE
#define WTMP_FILENAME WTMP_FILE

/*
 * Structure of utmp and wtmp files.
 *
 */
struct utmp {
	char	ut_line[8];		/* tty name */
	char	ut_name[8];		/* user id */
	char	ut_host[16];		/* host name, if remote */
	long	ut_time;		/* time on */
};

/*
 * This is to determine if a utmp entry does not correspond to a genuine user
 * (pseudo tty)
 */

#define nonuser(ut) ((ut).ut_host[0] == 0 && \
		strncmp((ut).ut_line, "tty", 3) == 0 \
			&& ((ut).ut_line[3] == 'p' \
			|| (ut).ut_line[3] == 'q' \
			|| (ut).ut_line[3] == 'r' \
			|| (ut).ut_line[3] == 's'))

__BEGIN_DECLS

extern void _write_utmp (const char *__line, const char *__name,
			 const char *__host, unsigned long __time);
extern void _write_wtmp (const char *__line, const char *__name,
			 const char *__host, unsigned long __time);

__END_DECLS

#endif /* !_OLD_UTMP_H */
