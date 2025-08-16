    .TEXT

    .XREF cpu_di
    .XREF cpu_ei
    
    .XREF nv_bankvec_handler
    .XREF nv_bankvec_install

    .XREF xbios_trap_old
    .XREF xbios_trap_new
    .XREF xbios_trap_install

    .XREF vdi_get_dispatcher


;----------------------------------------------------------
; uint16_t cpu_di(void)
; uint16_t cpu_ei(uint16_t ipl)
;   d0 = ipl
;----------------------------------------------------------
    .align 16
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
; trap2(-1)
;----------------------------------------------------------
vdi_get_dispatcher:
    move.l  #-1,d0
    trap    #2
    rts


;----------------------------------------------------------
; exception vector for bankswitcher mechanism
; maintains a virtual linear framebuffer by trapping
; access violations and swapping in physical banks
;
; 00 SP
; 02 PC
; 06 0100_vector
; 08 logical fault address
; 12 fault status longword
;
; 9 PF = 1 (access invalid page descriptor)
; 8 SP = 1 (supervisor protection violation detected by paged MMU)
; 7 WP = 1 (write protection violation detected by paged MMU)
; 5 RE = 1 (bus error on read)
; 4 WE = 1 (bus error on write)
;----------------------------------------------------------
    .align 16
    DC.L    0x00000000
    DC.B    "XBRA"
    DC.B    "IMNE"
nv_bankvec_old:
    DC.L    0x00000008
nv_bankvec_new:
    movem.l d0-d2/a0-a2,-(sp)
    move.l  sp,a0
    add.l   #24,a0
    bsr     nv_bankvec_handler
    cmp.b   #0,d0
    bne.b   nv_bankvec_skip
    movem.l (sp)+,d0-d2/a0-a2       ; re-run fault instruction
    rte
nv_bankvec_skip:
    movem.l (sp)+,d0-d2/a0-a2
    move.l  nv_bankvec_old,-(sp)    ; run original exception handler
    rts

nv_bankvec_install:
    bsr     cpu_di
    move.l  0x0008.w,d1
    move.l  d1,nv_bankvec_old
    move.l  #nv_bankvec_new,d1
    move.l  d1,0x0008.w
    bsr     cpu_ei
    rts



;----------------------------------------------------------
; xbios handler
;----------------------------------------------------------
    .align 16
    DC.L 0
	DC.B "XBRA"
	DC.B "IMNE"
xbios_trap_old:
	DC.L 0
xbios_trap_new:
    move.l  xbios_trap_old,-(sp)
.1: rts

;----------------------------------------------------------
; xbios install
;----------------------------------------------------------
xbios_trap_install:
    rts

