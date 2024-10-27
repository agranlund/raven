/*  
 * Copyright (C) 2000 Konrad Kokoszkiewicz <draco@atari.org.pl>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA
 * 
 */

# ifndef _MINT_SLB_H
# define _MINT_SLB_H 1

/* MagiC 5.20 Share Library Support */

typedef struct _slb_handle { int dummy; } *SLB_HANDLE, *SHARED_LIB;
#ifdef __NO_CDECL
typedef void *SLB_EXEC;
#else
typedef long  __CDECL (*SLB_EXEC)(SLB_HANDLE slb, long fn, short nwords, ...);
#endif

typedef struct {
	SLB_HANDLE	handle;
	SLB_EXEC	exec;
} SLB;


long _slbopen (const char *fname, const char *path, long ver, void *hnd, void *exec);


# endif /* mint/slb.h */
