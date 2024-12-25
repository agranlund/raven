;-------------------------------------------------------------------------------
; cacheoff
; (c) 2024 Anders Granlund
;
; disable all cpu caches
; inspired by CT60 tool with same name
;-------------------------------------------------------------------------------
;
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
;
;-------------------------------------------------------------------------------

	BSS
gstackb:	ds.l	256
gstackt:	ds.l	4

	TEXT
init:
	move.l	4(sp),a0
	lea		gstackt(pc),sp
	move.l	#0x1100,d0
	add.l	0x0c(a0),d0
	add.l	0x14(a0),d0
	add.l	0x1c(a0),d0

	move.l	d0,-(sp)			; Mshrink
	move.l	a0,-(sp)
	clr.w	-(sp)
	move.w	#0x4a,-(sp)
	trap	#1
	add.l	#12,sp

main:
	move.w	#0,-(sp)			; ct60_cache 0
	move.w	#0xC60C,-(sp)
	trap	#14
	addq.l	#4,sp

done:
	clr.w	-(sp)				; Pterm0
	trap	#1
