#ifndef __ULIMIT_H__
#define __ULIMIT_H__

#ifndef	_FEATURES_H
# include <features.h>
#endif

#ifndef __SYS_ULIMIT_H__
#include <sys/ulimit.h>
#endif

__BEGIN_DECLS

extern long ulimit(int, ...) __THROW;

__END_DECLS

#endif	/* __ULIMIT_H__ */
