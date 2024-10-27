/*  sys/times.h -- MiNTLib.
    Copyright (C) 1999 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifndef	_SYS_TIMES_H
#define	_SYS_TIMES_H

#ifndef _FEATURES_H
# include <features.h>
#endif

#ifndef _TIME_H
# define __need_clock_t
# include <time.h>
#endif

__BEGIN_DECLS

/* CPU usage.  */
struct tms {
  clock_t tms_utime;     /* User CPU time.  */
  clock_t tms_stime;     /* System (kernel) CPU time.  */
  clock_t tms_cutime;    /* Terminated childrens' user CPU time.  */
  clock_t tms_cstime;    /* Terminated childrens' system CPU time.  */
};

extern clock_t times (struct tms *) __THROW;

__END_DECLS

#endif /* SYS_TIMES_H */
