;-------------------------------------------------------------------------------
; rvsnd : common driver long math for plain 68000
; (c)2025 Anders Granlund
;-------------------------------------------------------------------------------
; This file is free software  you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation  either version 2, or (at your option)
; any later version.
;
; This file is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY  without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program  if not, write to the Free Software
; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;-------------------------------------------------------------------------------

    .text
    .xref cpu

;-------------------------------------------------------------------------------
; plain 68000 support
; todo: runtime cpu check and use 68020+ instructions when possible
;-------------------------------------------------------------------------------

    .export _ulmul
_ulmul:
.0:	move.l	d0,d2
	swap	d2
	tst.w	d2
	bne.s	.2
	move.l	d1,d2
	swap	d2
	tst.w	d2
	bne.s	.1
	mulu	d1,d0
	rts
.1: mulu	d0,d2
	swap	d2
	mulu	d1,d0
	add.l	d2,d0
	rts
.2:	mulu	d1,d2
	swap	d2
	mulu	d1,d0
	add.l	d2,d0
	rts

    .export _lmul
_lmul:
.0:	move.l	d0,d2
	bpl.s	.1
	neg.l	d0
.1:	eor.l	d1,d2
	movea.l	d2,a0
	tst.l	d1
	bpl.s	.2
	neg.l	d1
.2: move.l	d0,d2
	swap	d2
	tst.w	d2
	bne.s	.6
	move.l	d1,d2
	swap	d2
	tst.w	d2
	bne.s	.4
	mulu	d1,d0
	move.l	a0,d2
	bpl.s	.3
	neg.l	d0
.3:	rts
.4:	mulu	d0,d2
	swap	d2
	mulu	d1,d0
	add.l	d2,d0
	move.l	a0,d2
	bpl.s	.5
	neg.l	d0
.5:	rts
.6:	mulu	d1,d2
	swap	d2
	mulu	d1,d0
	add.l	d2,d0
	move.l	a0,d2
	bpl.s	.7
	neg.l	d0
.7:	rts

    .export _uldiv
_uldiv:
.0:	move.l	d1,d2
	swap	d2
	tst.w	d2
	bne.s	.2
	move.l	d0,d2
	swap	d2
	tst.w	d2
	bne.s	.1
	divu	d1,d0
	swap	d0
	clr.w	d0
	swap	d0
	rts
.1:	clr.w	d0
	swap	d0
	swap	d2
	divu	d1,d0
	movea	d0,a0
	move.w	d2,d0
	divu	d1,d0
	swap	d0
	move.w	a0,d0
	swap	d0
	rts
.2:	movea.l	d1,a0
	swap	d0
	moveq	#0,d1
	move.w	d0,d1
	clr.w	d0
	moveq	#15,d2
	add.l	d0,d0
	addx.l	d1,d1
.3:	sub.l	a0,d1
	bcc.s	.4
	add.l	a0,d1
.4:	addx.l	d0,d0
	addx.l	d1,d1
	dbf	    d2,.3
	not.w	d0
	rts

    .export _ldiv
_ldiv:
.0:	move.l	d0,d2
	bpl.s	.1
	neg.l	d0
.1:	eor.l	d1,d2
	movea.l	d2,a1
	tst.l	d1
	bpl.s	.2
	neg.l	d1
.2:	move.l	d1,d2
	swap	d2
	tst.w	d2
	bne.s	.6
	move.l	d0,d2
	swap	d2
	tst.w	d2
	bne.s	.4
	divu	d1,d0
	swap	d0
	clr.w	d0
	swap	d0
	move.l	a1,d2
	bpl.s	.3
	neg.l	d0
.3:	rts
.4:	clr.w	d0
	swap	d0
	swap	d2
	divu	d1,d0
	movea	d0,a0
	move	d2,d0
	divu	d1,d0
	swap	d0
	move	a0,d0
	swap	d0
	move.l	a1,d2
	bpl.s	.5
	neg.l	d0
.5:	rts
.6:	movea.l	d1,a0
	swap	d0
	moveq	#0,d1
	move.w	d0,d1
	clr.w	d0
	moveq	#15,d2
	add.l	d0,d0
	addx.l	d1,d1
.7:	sub.l	a0,d1
	bcc.s	.8
	add.l	a0,d1
.8:	addx.l	d0,d0
	addx.l	d1,d1
	dbf	    d2,.7
	not.w	d0
	move.l	a1,d2
	bpl.s	.9
	neg.l	d0
.9:	rts

    .export _ulmod
_ulmod:
.0:	move.l	d1,d2
	swap	d2
	tst.w	d2
	bne.s	.2
	move.l	d0,d2
	swap	d2
	tst.w	d2
	bne.s	.1
	divu	d1,d0
	clr.w	d0
	swap	d0
	rts
.1:	clr.w	d0
	swap	d0
	swap	d2
	divu	d1,d0
	move.w	d2,d0
	divu	d1,d0
	clr.w	d0
	swap	d0
	rts
.2:	movea.l	d1,a0
	move.l	d0,d1
	clr.w	d0
	swap	d0
	swap	d1
	clr.w	d1
	moveq	#15,d2
	add.l	d1,d1
	addx.l	d0,d0
.3:	sub.l	a0,d0
	bcc.s	.4
	add.l	a0,d0
.4:	addx.l	d1,d1
	addx.l	d0,d0
	dbf	    d2,.3
	roxr.l	#1,d0
	rts

    .export _lmod
lmod:
.0:	movea.l	d0,a1
	move.l	d0,d2
	bpl.s	.1
	neg.l	d0
.1:	tst.l	d1
	bpl.s	.2
	neg.l	d1
.2:	move.l	d1,d2
	swap	d2
	tst.w	d2
	bne.s	.6
	move.l	d0,d2
	swap	d2
	tst.w	d2
	bne.s	.4
	divu	d1,d0
	clr.w	d0
	swap	d0
	move.l	a1,d2
	bpl.s	.3
	neg.l	d0
.3:	rts
.4:	clr.w	d0
	swap	d0
	swap	d2
	divu	d1,d0
	move.w	d2,d0
	divu	d1,d0
	clr.w	d0
	swap	d0
	move.l	a1,d2
	bpl.s	.5
	neg.l	d0
.5:	rts
.6:	movea.l	d1,a0
	move.l	d0,d1
	clr.w	d0
	swap	d0
	swap	d1
	clr.w	d1
	moveq	#15,d2
	add.l	d1,d1
	addx.l	d0,d0
.7:	sub.l	a0,d0
	bcc.s	.8
	add.l	a0,d0
.8:	addx.l	d1,d1
	addx.l	d0,d0
	dbf	    d2,.7
	roxr.l	#1,d0
	move.l	a1,d2
	bpl.s	.9
	neg.l	d0
.9:	rts
