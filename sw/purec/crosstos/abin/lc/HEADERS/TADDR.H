/**
 * $Id: taddr.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Macros for object access
 *
 * (c) Copyright 1991, 1993 HiSoft
**/

#ifndef _TADDR_H
#define _TADDR_H

#define OB_NEXT(x)	(tree[x]->ob_next)
#define OB_HEAD(x)	(tree[x]->ob_head)
#define OB_TAIL(x)	(tree[x]->ob_tail)
#define OB_TYPE(x)	(tree[x]->ob_type)
#define OB_FLAGS(x)	(tree[x]->ob_flags)
#define OB_STATE(x)	(tree[x]->ob_state)
#define OB_SPEC(x)	(tree[x]->ob_spec)
#define OB_X(x)	(tree[x]->ob_x)
#define OB_Y(x)	(tree[x]->ob_y)
#define OB_WIDTH(x)	(tree[x]->ob_width)
#define OB_HEIGHT(x)	(tree[x]->ob_height)
#endif
