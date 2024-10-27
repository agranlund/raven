/*  rtent.h -- MiNTLib.
    Copyright (C) 1999, 2000 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifndef _RTENT_H
#define _RTENT_H

#ifndef _FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

/* Note:  Don't rely on the existence of this file.  It may vanish.  */
extern void setrtent (void);
extern void endrtent (void);
extern struct rtentry* getrtent (void);

__END_DECLS

#endif
