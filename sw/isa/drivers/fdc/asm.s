
    .text
    .globl floppy_devno

    .globl xbios_old
    .globl xbios_new

    .globl xb_floprd
    .globl xb_flopwr
    .globl xb_flopfmt
    .globl xb_flopver
    .globl xb_floprate

    .globl hdv_rw
    .globl hdv_old_rw
    .globl hdv_new_rw

    .globl hdv_getbpb
    .globl hdv_old_getbpb
    .globl hdv_new_getbpb

    .globl hdv_mediach
    .globl hdv_old_mediach
    .globl hdv_new_mediach

;--------------------------------------------------
; XBIOS
;--------------------------------------------------
	dc.b "XBRA"
	dc.b "_FDC"
xbios_old:
	dc.l 0x00B8
xbios_new:
	move	usp,a0
	btst	#5,(sp)			; already super?
	beq.b	.1
    lea     8(sp),a0
;    lea     6(sp),a0
;    tst.w   0x59e.w         ; long stackframes?
;    beq.b   .1
;    addq.l  #2,a0
.1: move.w	(a0)+,d0		; d0.w is opcode
    move.w  floppy_devno,d1 ; d1.w is devno
    move.l  xbios_done(pc),-(sp)
    cmp.w   #41,d0          ; floprate
    bne.b   .2
    cmp.w   (a0),d1         ; match devno?
    bne.b   xbios_skip
    bra     xb_floprate
.2: cmp.w   8(a0),d1        ; match devno?
    bne.b   xbios_skip
    cmp.w   #8,d0
    beq     xb_floprd
    cmp.w   #9,d0
    beq     xb_flopwr
    cmp.w   #10,d0
    beq     xb_flopfmt
    cmp.w   #19,d0
    beq     xb_flopver
xbios_skip:
    move.l  xbios_old,(sp)
    rts
xbios_done:
    rte

;--------------------------------------------------
; BIOS
;--------------------------------------------------
	dc.b "XBRA"
	dc.b "_FDC"
hdv_old_getbpb:
    dc.l    0x472
hdv_new_getbpb:
    move.w  4(sp),d0
    cmp.w   floppy_devno,d0
    beq.b   .1
    move.l  hdv_old_getbpb,-(sp)
    rts
.1: movem.l d2/a2,-(sp)
    bsr     hdv_getbpb
    movem.l (sp)+,d2/a2
    rts

	dc.b "XBRA"
	dc.b "_FDC"
hdv_old_rw:
    dc.l    0x476
hdv_new_rw:
    move.w  14(sp),d0
    cmp.w   floppy_devno,d0
    beq.b   .1
    move.l  hdv_old_rw,-(sp)
    rts
.1: lea.l   4(sp),a0
    movem.l d2/a2,-(sp)
    bsr     hdv_rw
    movem.l (sp)+,d2/a2
    rts

	dc.b "XBRA"
	dc.b "_FDC"
hdv_old_mediach:
    dc.l    0x47e
hdv_new_mediach:
    move.w  4(sp),d0
    cmp.w   floppy_devno,d0
    beq.b   .1
    move.l  hdv_old_mediach,-(sp)
    rts
.1: movem.l d2/a2,-(sp)
    bsr     hdv_mediach
    movem.l (sp)+,d2/a2
    rts

