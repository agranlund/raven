#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#ifndef _FEATURES_H
# include <features.h>
#endif

#if defined(_PUREC_SOURCE) && !defined(__USE_MINT_SIGNAL)

#include <purec/signal.h>

#else

#include <mint/signal.h>

#endif

#endif /* __SIGNAL_H__ */
