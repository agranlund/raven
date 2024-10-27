
#ifndef _UTIME_H
#define _UTIME_H

#ifndef _FEATURES_H
# include <features.h>
#endif

#define __need_time_t
#include <time.h>

__BEGIN_DECLS

struct utimbuf {			/* type for times() call */
	time_t	actime;
	time_t	modtime;
};

extern int utime (const char *path, const struct utimbuf *times);
extern int __utime (const char *path, const struct utimbuf *times);

__END_DECLS

#endif  /* _UTIME_H */
