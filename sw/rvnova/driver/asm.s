    .TEXT

    .XREF cpu_di
    .XREF cpu_ei

    .XREF xbios_old
    .XREF xbios_new


;----------------------------------------------------------
; uint16_t cpu_di(void)
; uint16_t cpu_ei(uint16_t ipl)
;   d0 = ipl
;----------------------------------------------------------
cpu_di:
    move.w  #7,d0
cpu_ei:
    lsl.w   #8,d0
    and.w   #0x0700,d0
    move.w  sr,d1
    and.w   #0xF0FF,d1
    or.w    d0,d1
    move.w  sr,d0
    lsr.w   #8,d0
    and.l   #7,d0
    move.w  d1,sr
    nop
    rts


;----------------------------------------------------------
; xbios handler
;----------------------------------------------------------
    .align 16
	DC.B "XBRA"
	DC.B "SPIO"
xbios_old:
	DC.L 0
xbios_new:
    move.l  xbios_old,-(sp)
.1: rts

;----------------------------------------------------------
; xbios install
;----------------------------------------------------------
xcb_p_xbios_install:
    rts

