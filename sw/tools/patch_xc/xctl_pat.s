;
; the following is the source for a patch to XCONTROL v1.31, to support
; the copyback cache of the CT60.  this requires flushing the cache after
; program relocation, via a (new) xbios call.
;
;
; original author:	didier mequignon
; modified by:		roger burrows (august/2003)
;	. create source in devpac 3.10 format
;	. add test for no fixup data
;	. change xbios opcode to 'original' flush cache value
;
;
; known offsets for this patch in XCONTROL v1.31:
;	country    size   offset
;	-------    ----   ------
;	France    44990    26194
;	UK        44685    26222
;	USA       44681    26218
; all values are decimal
;
perform_relocation:
	move.l	a2,-(a7)		;save work reg
	move.l	(a1)+,d0		;a1 -> relocation bytes, d0 = offset of first in text
	beq.s	exit			;zero means no fixup ...
	movea.l	(a0),a2			;a2 -> start of text
	bra.s	normal			; & go calc offset of first byte to fixup

check_special:
	cmp.b	#1,d0			;is it special?
	bne.s	normal			;no
	lea	254(a2),a2		;yes - up offset
	bra.s	next			; & go check next byte

normal:
	adda.l	d0,a2			;up offset
	move.l	(a0),d1			;d1 = start of text
	add.l	d1,(a2)			;do relocation
next:
	moveq	#0,d0
	move.b	(a1)+,d0		;get next relocation byte
	bne.s	check_special		; & loop if not end
	move	#$c60d,-(a7)		;end - flush cache
	trap	#14
	addq	#2,a7
exit:
	movea.l	(a7)+,a2		;restore work reg
	rts
	end
