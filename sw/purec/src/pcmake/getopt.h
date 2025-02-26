/* Declarations for getopt.
   Copyright (C) 1989-1994, 1996-1999, 2001, 2003-2007, 2009-2010 Free Software
   Foundation, Inc.
   This file is part of the GNU C Library.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _GETOPT_H
#define _GETOPT_H 1
#define _GETOPT_CORE_H
#define _GETOPT_EXT_H
#define _GETOPT_POSIX_H 1

/* Standalone applications should #define __GETOPT_PREFIX to an
   identifier that prefixes the external functions and variables
   defined in this header.  When this happens, include the
   headers that might declare getopt so that they will not cause
   confusion if included after this file (if the system had <getopt.h>,
   we have already included it).  Then systematically rename
   identifiers so that they do not collide with the system functions
   and variables.  Renaming avoids problems with some compilers and
   linkers.  */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#if defined __GETOPT_PREFIX
# undef getopt
# undef getopt_long
# undef getopt_long_only
# undef optarg
# undef opterr
# undef optind
# undef optopt
# undef option
# define __GETOPT_CONCAT(x, y) x ## y
# define __GETOPT_XCONCAT(x, y) __GETOPT_CONCAT (x, y)
# define __GETOPT_ID(y) __GETOPT_XCONCAT (__GETOPT_PREFIX, y)
# define getopt __GETOPT_ID (getopt)
# define getopt_long __GETOPT_ID (getopt_long)
# define getopt_long_only __GETOPT_ID (getopt_long_only)
# define getopt_r __GETOPT_ID (getopt_r)
# define getopt_long_r __GETOPT_ID (getopt_long_r)
# define getopt_long_only_r __GETOPT_ID (getopt_long_only_r)
# define optarg __GETOPT_ID (optarg)
# define opterr __GETOPT_ID (opterr)
# define optind __GETOPT_ID (optind)
# define optopt __GETOPT_ID (optopt)
# define option __GETOPT_ID (option)
#endif

/* If __GNU_LIBRARY__ is not already defined, either we are being used
   standalone, or this is the first header included in the source file.
   If we are being used with glibc, we need to include <features.h>, but
   that does not exist if we are standalone.  So: if __GNU_LIBRARY__ is
   not defined, include <ctype.h>, which will pull in <features.h> for us
   if it's from glibc.  (Why ctype.h?  It's guaranteed to exist and it
   doesn't flood the namespace with stuff the way some other headers do.)  */
#if !defined __GNU_LIBRARY__
# include <ctype.h>
#endif

#ifndef __THROW
# ifndef __GNUC_PREREQ
#  define __GNUC_PREREQ(maj, min) (0)
# endif
# if defined __cplusplus && __GNUC_PREREQ (2,8)
#  define __THROW       throw ()
# else
#  define __THROW
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* For communication from `getopt' to the caller.
   When `getopt' finds an option that takes an argument,
   the argument value is returned here.
   Also, when `ordering' is RETURN_IN_ORDER,
   each non-option ARGV-element is returned here.  */

#ifdef __APPLE__
extern char *optarg;
#else
extern const char *optarg;
#endif

/* Index in ARGV of the next element to be scanned.
   This is used for communication to and from the caller
   and for communication between successive calls to `getopt'.

   On entry to `getopt', zero means this is the first call; initialize.

   When `getopt' returns EOF, this is the index of the first of the
   non-option elements that the caller should itself scan.

   Otherwise, `optind' communicates from one call to the next
   how much of ARGV has been scanned so far.  */

extern int optind;

/* Callers store zero here to inhibit the error message `getopt' prints
   for unrecognized options.  */

	extern int opterr;

/* Set to an option character which was unrecognized.  */

extern int optopt;

#if (!defined(HAVE_GETOPT_H) || defined(__GETOPT_PREFIX))
/* Describe the long-named options requested by the application.
   The LONG_OPTIONS argument to getopt_long or getopt_long_only is a vector
   of `struct option' terminated by an element containing a name which is
   zero.

   The field `has_arg' is:
   no_argument          (or 0) if the option does not take an argument,
   required_argument    (or 1) if the option requires an argument,
   optional_argument    (or 2) if the option takes an optional argument.

   If the field `flag' is not NULL, it points to a variable that is set
   to the value given in the field `val' when the option is found, but
   left unchanged if the option is not found.

   To have a long-named option do something other than set an `int' to
   a compiled-in constant, such as set a value from `optarg', set the
   option's `flag' field to zero and its `val' field to a nonzero
   value (the equivalent single-letter option character, if there is
   one).  For long options that have a zero `flag' field, `getopt'
   returns the contents of the `val' field.  */

struct option
{
	const char *name;
	/* has_arg can't be an enum because some compilers complain about
	   type mismatches in all the code that assumes it is an int.  */
	int has_arg;
	int *flag;
	int val;
};

/* Names for the values of the `has_arg' field of `struct option'.  */

# define no_argument            0
# define required_argument      1
# define optional_argument      2
#endif


/* Get definitions and prototypes for functions to process the
   arguments in ARGV (ARGC of them, minus the program name) for
   options given in OPTS.

   Return the option character from OPTS just read.  Return -1 when
   there are no more options.  For unrecognized options, or options
   missing arguments, `optopt' is set to the option letter, and '?' is
   returned.

   The OPTS string is a list of characters which are recognized option
   letters, optionally followed by colons, specifying that that letter
   takes an argument, to be placed in `optarg'.

   If a letter in OPTS is followed by two colons, its argument is
   optional.  This behavior is specific to the GNU `getopt'.

   The argument `--' causes premature termination of argument
   scanning, explicitly telling `getopt' that there are no more
   options.

   If OPTS begins with `-', then non-option arguments are treated as
   arguments to the option '\1'.  This behavior is specific to the GNU
   `getopt'.  If OPTS begins with `+', or POSIXLY_CORRECT is set in
   the environment, then do not permute arguments.  */

struct _getopt_data;
#ifndef __XGETOPT_IMPLEMENTATION__
struct _getopt_data { int dummy; };
#endif

#ifdef __APPLE__
int getopt(int argc, char * const argv[], const char *shortopts) __THROW;
#else
int getopt(int argc, const char **argv, const char *shortopts) __THROW;
#endif

int getopt_r(int argc, const char **argv, const char *optstring, struct _getopt_data *d) __THROW;

int __posix_getopt(int argc, const char **argv, const char *optstring) __THROW;

int getopt_long(int argc, const char **argv, const char *shortopts, const struct option *longopts, int *longind) __THROW;

int getopt_long_only(int argc, const char **argv, const char *shortopts, const struct option *longopts, int *longind) __THROW;


int getopt_long_r(int argc, const char **argv, const char *options,
					   const struct option *long_options, int *opt_index, struct _getopt_data *d);
int getopt_long_only_r(int argc, const char **argv, const char *options,
							const struct option *long_options, int *opt_index, struct _getopt_data *d);


void getopt_init_r(const char *argv0, struct _getopt_data **pd);
int getopt_finish_r(struct _getopt_data **pd);
int getopt_ind_r(const struct _getopt_data *d);
const char *getopt_arg_r(const struct _getopt_data *d);
int getopt_opt_r(const struct _getopt_data *d);
int getopt_switch_r(const struct _getopt_data *d);
void getopt_seterrprint_r(struct _getopt_data *d, void (*f)(const char *format, ...));


#ifdef __cplusplus
}
#endif

#endif	/* _GETOPT_H */
