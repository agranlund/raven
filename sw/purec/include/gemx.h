#ifndef __GEMX_H__
#define __GEMX_H__

#include <gem.h>
#include <wdlgevnt.h>
#include <wdlgpdlg.h>
#include <wdlgfslx.h>
#include <wdlglbox.h>
#include <wdlgedit.h>

void EVNT_multi(_WORD evtypes, _WORD nclicks, _WORD bmask, _WORD bstate, const MOBLK *m1, const MOBLK *m2, unsigned long ms, EVNT *event);

#ifdef __USE_REENTRANT
#include <mt_gemx.h>

#define EVNT_multi( a, b, c, d, e, f, g, h )  mt_EVNT_multi( a, b, c, d, e, f, g, h, aes_global )
#endif

#endif /* __GEMX_H__ */
