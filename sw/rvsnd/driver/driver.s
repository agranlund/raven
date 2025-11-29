;-------------------------------------------------------------------------------
; rvsnd : common driver startup code
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
    .xref init
    .export rvsnd
    .export rvini

; parameters passed on stack
;   0 = return pointer
;   4 = rvsnd_driver_api_t*
;   8 = ini_t* 

rvxdrv_start:
    move.l  4(sp),rvsnd
    move.l  8(sp),rvini
    sub.l   d0,d0
    jsr     init
    rts

rvsnd:
    dc.l    0

rvini:
    dc.l    0


    .export _ulmul
_ulmul:
	move.l	d0,d2
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
	move.l	d0,d2
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
