/*  sys/utsname.h -- MiNTLib.
    Copyright (C) 1999 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifndef	_SYS_UTSNAME_H
#define	_SYS_UTSNAME_H	1

#ifndef _FEATURES_H
# include <features.h>
#endif

#ifndef _SYS_PARAM_H
# include <sys/param.h>
#endif

__BEGIN_DECLS

#include <bits/utsname.h>

#ifndef _UTSNAME_SYSNAME_LENGTH
# define _UTSNAME_SYSNAME_LENGTH _UTSNAME_LENGTH
#endif
#ifndef _UTSNAME_NODENAME_LENGTH
# define _UTSNAME_NODENAME_LENGTH _UTSNAME_LENGTH
#endif
#ifndef _UTSNAME_RELEASE_LENGTH
# define _UTSNAME_RELEASE_LENGTH _UTSNAME_LENGTH
#endif
#ifndef _UTSNAME_VERSION_LENGTH
# define _UTSNAME_VERSION_LENGTH _UTSNAME_LENGTH
#endif
#ifndef _UTSNAME_MACHINE_LENGTH
# define _UTSNAME_MACHINE_LENGTH _UTSNAME_LENGTH
#endif

/* Structure describing the system and machine.  */
struct utsname {
    /* Name of the implementation of the operating system. sysinfo (SI_SYSNAME, ...) */
    char sysname[_UTSNAME_SYSNAME_LENGTH];

    /* Name of this node on the network. sysinfo (SI_HOSTNAME, ...) */
    char nodename[_UTSNAME_NODENAME_LENGTH];

    /* Current release level of this implementation. sysinfo (SI_RELEASE, ...) */
    char release[_UTSNAME_RELEASE_LENGTH];

    /* Current version level of this release. sysinfo (SI_VERSION, ...) */
    char version[_UTSNAME_VERSION_LENGTH];

    /* Name of the hardware type the system is running on. sysinfo (SI_PLATFORM, ...) */
    char machine[_UTSNAME_MACHINE_LENGTH];

#if defined(_UTSNAME_DOMAIN_LENGTH)
    /* Name of the domain of this node on the network. sysinfo (SI_DOMAINNAME, ...) */
# ifdef __USE_GNU
    char domainname[_UTSNAME_DOMAIN_LENGTH];
# else
    char __domainname[_UTSNAME_DOMAIN_LENGTH];
# endif
#endif

#if defined(_UTSNAME_ARCHITECTURE_LENGTH)
  /* Non-portable extension.  Since the GNU sh-utils write such
     a field for the uname command it doesn't seem to be such a bad
     idea. sysinfo (SI_ARCHITECTURE, ...) */
  char architecture[_UTSNAME_ARCHITECTURE_LENGTH];   /* sysinfo (SI_ARCHITECTURE, ...).  */
#endif
};

#ifdef __USE_SVID
/* Note that SVID assumes all members have the same size.  */
# define SYS_NMLN  _UTSNAME_LENGTH
#endif


/* Fill INFO with the system information obtained via sysinfo.  */
extern int uname (struct utsname *__name) __THROW;

__END_DECLS

#endif /* sys/utsname.h */
