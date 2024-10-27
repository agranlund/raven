;-------------------------------------------------------------------------------
; cpu temperature and other support functions
; Derived from ct60conf.cpx/temp.s (c) Didier Mequignon
;-------------------------------------------------------------------------------
;  This library is free software; you can redistribute it and/or
;  modify it under the terms of the GNU Lesser General Public
;  License as published by the Free Software Foundation; either
;  version 2.1 of the License, or (at your option) any later version.
;
;  This library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;  Lesser General Public License for more details.
;
;  You should have received a copy of the GNU Lesser General Public
;  License along with this library; if not, write to the Free Software
;  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;-------------------------------------------------------------------------------

	.export ct60_read_temp
	.export ct60_stop
	.export ct60_cpu_revision
	.export mes_delay
	.export value_supexec

ct60_read_temp:
	movem.l d1-d3/a0-a2,-(sp)				; save regs
	move.w	sr,-(sp)						; save sr
	or.w	#0x700,sr						; disable interrupts
	move.l	#0,d0							; fake temp
	move.w	(sp)+,sr						; restore sr
	tst.l 	d0								; set flag
	movem.l (sp)+,d1-d3/a0-a2				; restore regs
	rts										; return
	

ct60_stop:
	or.w 	#0x700,sr						; disable interrupts
	dc.w	0xf800,0x01c0,0x2700			; fpstop #$2700
	rts
	

ct60_cpu_revision:
	dc.l 	0x4E7A0808						;movec.l pcr,d0
	lsr.w 	#8,d0							;revision
	ext.l 	d0
	rts


mes_delay:
	move.l value_supexec,d0
	move.l 0x4ba,d1							; hz200
.1:	subq.l #1,d0
	bne.s .1
	move.l 0x4ba,d0							; hz200
	sub.l d1,d0
	rts

value_supexec:	dc.l 0

	end
