#ifndef __MT_GEMX_H__
#define __MT_GEMX_H__

#include <mt_aes.h>
#include <vdi.h>

extern void mt_EVNT_multi( WORD evtypes, WORD nclicks, WORD bmask, WORD bstate,
                    MOBLK *m1, MOBLK *m2, ULONG ms, EVNT *event, WORD *aes_global );

#endif
