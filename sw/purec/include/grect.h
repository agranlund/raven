/*****************************************************************************
 * GRECT.H
 *****************************************************************************/

#ifndef __GRECT_H__
#define __GRECT_H__

#ifndef __PORTAB_H__
#  include <portab.h>
#endif

EXTERN_C_BEG


#ifndef __GRECT
# define __GRECT
typedef struct _grect {
	_WORD g_x;
	_WORD g_y;
	_WORD g_w;
	_WORD g_h;
} GRECT;
#endif


EXTERN_C_END

#endif /* __GRECT_H__ */
