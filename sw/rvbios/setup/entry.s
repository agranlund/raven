/*
	Startup code, setting stack
	(C) 2009 Patrice Mandin

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define STACK_SIZE 8192

	.globl	_init_setup, ___main

	.text

/* Calculate length to shrink memory */

_init_setup:
	movl	sp@(4),a0	/* Read basepage pointer */

	movl	#0x100.w,d0	/* Length of basepage */
	addl	a0@(12),d0	/* Add TEXT length */
	addl	a0@(20),d0	/* Add DATA length */
	addl	a0@(28),d0	/* Add BSS length */
	addl	#STACK_SIZE.w,d0

	lea	a0@(d0.l),sp	/* Set new stack */

	movel	d0,sp@-
	movel	a0,sp@-
	clrw	sp@-
	movew	#0x4a,sp@-	/* Mshrink() */
	trap	#1

	jsr	___main

	clrw	sp@-		/* Pterm0() */
	trap	#1
