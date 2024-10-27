/*  libgen.h -- MiNTLib.
    Copyright (C) 1999, 2000 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifndef _LIBGEN_H
# define _LIBGEN_H 1

#ifndef	_FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

/* Return directory part of PATH or "." if none is available.  */
extern char* dirname (char *__path);

/* Return filename part of PATH.
   This is the weird XPG version of this function.  It sometimes will
   modify its argument.  Therefore we normally use the GNU version (in
   <string.h>) and only if this header is included make the XPG
   version available under the real name.  */
#ifdef basename
# undef basename
#endif
extern char *__xpg_basename (char *__path);
#define basename(path)	__xpg_basename (path)

__END_DECLS

#endif /* _LIBGEN_H */
