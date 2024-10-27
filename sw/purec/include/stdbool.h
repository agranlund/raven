/*  (c) 1991-2008 by H. Robbers te Amsterdam
 *
 * This file is part of AHCC.
 *
 * AHCC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * AHCC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AHCC; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _STDBOOL_H
#define _STDBOOL_H

#if defined(__AHCC__)
/*
 * AHCC erroneously declares 'false' and 'true'
 * as builtin constants, and also defines
 * __bool_true_false_are_defined but only declares _Bool, not bool
 */
#undef __bool_true_false_are_defined
#endif

#if ! defined __bool_true_false_are_defined

#ifndef __cplusplus

#define bool	_Bool
#if !defined(__AHCC__) && !defined(__GNUC__)
#define _Bool	int
#endif
#define true	1
#define false	0

#else

/* Supporting _Bool in C++ is a GCC extension.  */
#define _Bool	bool

#if __cplusplus < 201103L
/* Defining these macros in C++98 is a GCC extension.  */
#define bool	bool
#define false	false
#define true	true
#endif

#endif /* __cplusplus */

#endif

/* Signal that all the definitions are present.  */
#define __bool_true_false_are_defined 1

#endif /* _STDBOOL_H */
