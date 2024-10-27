#ifndef __need_mintlib_version
# error "Never use <mintlib/version.h> directly"
#endif
#undef __need_mintlib_version

#ifdef _FAKE_GLIBC
/* This macro indicates that the installed library is the GNU C Library.
   For historic reasons the value now is 6 and this will stay from now
   on.  The use of this variable is deprecated.  Use __GLIBC__ and
   __GLIBC_MINOR__ now (see below) when you want to test for a specific
   GNU C library version and use the values in <gnu/lib-names.h> to get
   the sonames of the shared libraries.  
   
   Of course the MiNTLib is not the GNU libc but quite close already.
   It seems that it causes more problems if the macros from the glibc
   are not defined than it causes if they are defined.  If you run
   into difficulties, #undef the macros in the sources.  */
# undef  __GNU_LIBRARY__
# define __GNU_LIBRARY__ 6

/* Major and minor version number of the GNU C library package.  Use
   these macros to test for features in specific releases.
   
   Again, this is a plain lie for the MiNTLib but it is hopefully
   reasonable to define them.  */
# define __GLIBC__	2
# define __GLIBC_MINOR__	1
#endif

/* Major and minor version number of the MiNTLib package.  Use these macros 
   to test for features in specific releases.  */
#define __MINTLIB__		__MINTLIB_MAJOR__
#define	__MINTLIB_MAJOR__	0
#define	__MINTLIB_MINOR__	60
#define __MINTLIB_REVISION__	0
#define __MINTLIB_BETATAG__     ""

