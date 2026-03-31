    .TEXT

    .XREF cpu_di
    .XREF cpu_ei
    
    .XREF timer_start
    .XREF timer_stop

    .XREF nv_banksw_installvector
    .XREF nv_banksw_handler

    .XREF xbios_trap_old
    .XREF xbios_trap_new
    .XREF xbios_trap_install

    .XREF nova_p_setscreen_first_asm
    .XREF nova_p_setscreen_first

    .XREF vt_clear_rows_asm
    .XREF vt_clear_rows_new
    .XREF vt_copy_rows_asm
    .XREF vt_copy_rows_new

    .XREF vd_hline_noclip_asm
    .XREF vd_hline_noclip_new
    .XREF vd_hline_noclip_old

    .XREF vd_hline_asm
    .XREF vd_hline_new
    .XREF vd_hline_old

    .XREF vd_line_asm
    .XREF vd_line_new
    .XREF vd_line_old


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
; high resolution timer
;----------------------------------------------------------
timer_old_vec:  ds.l 1
timer_sr:       ds.l 1
timer_count:    ds.l 1

timer_int:
    addq.l  #1,timer_count
    move.b  #0xdf,0xfffffa11.w
    rte

timer_start:
    ; disable interrupts
    move.w  #0x2700,d0
    bsr     cpu_ei
    move.w  d0,timer_sr
    move.l  #0,timer_count
    ; replace timer-c
    move.l  0x00000114.w,d0
    move.l  d0,timer_old_vec
    and.b   #0x8f,0xfffffa1d.w      ; stop
    move.l  #timer_int,0x00000114.w
    move.b  #15,0xfffffa23.w
    or.b    #0x10,0xfffffa1d.w      ; start 40960hz
    ; and start
    move.w  #0x2600,d0
    bsr     cpu_ei
    rts

timer_stop:
    move.w  #0x2700,sr
    and.b   #0x8f,0xfffffa1d.w      ; stop
    move.l  timer_old_vec,d0
    move.l  d0,0x00000114.w
    move.b  #192,d0
    move.b  d0,0xfffffa23.w
    and.b   #0xdf,0xfffffa11.w
    or.b    #0x50,0xfffffa1d.w      ; start 200hz
    move.w  timer_sr,d0
    bsr     cpu_ei
    move.l  timer_count,d0
    rts

;----------------------------------------------------------
; entry point for patching vdi
;----------------------------------------------------------
nova_p_setscreen_first_asm:
    move.l  sp,a1
    jmp     nova_p_setscreen_first

;----------------------------------------------------------
; entry point for vt_clear_rows
;----------------------------------------------------------
vt_clear_rows_asm:
    movem.l d0-d2/a0-a2,-(sp)
    jsr     vt_clear_rows_new
    movem.l (sp)+,d0-d2/a0-a2
    rts

;----------------------------------------------------------
; entry point for vt_copy_rows
;----------------------------------------------------------
vt_copy_rows_asm:
    movem.l d0-d2/a0-a2,-(sp)
    jsr     vt_copy_rows_new
    movem.l (sp)+,d0-d2/a0-a2
    rts


;----------------------------------------------------------
; internal: hline_noclip
;   d0.w = x0
;   d1.w = x1
;   d6.w = y0 (nope, get from wks)
;   a6   = wks
;
; original function trash everything
;----------------------------------------------------------
vd_hline_noclip_asm:
    move.w  d1,d2
    sub.w   d0,d2
    cmp.w   #32,d2
    blt.b   .2
    move.l  d1,-(sp)
    move.l  d0,-(sp)
    movea.l a6,a0
    jsr     vd_hline_noclip_new
    tst.w   d0
    beq.b   .1
    addq.l  #8,sp
    rts
.1: move.l  (sp)+,d0
    move.l  (sp)+,d1
.2: move.l  vd_hline_noclip_old,a3
    jmp     (a3)
vd_hline_noclip_old:
    ds.l    1

;----------------------------------------------------------
; internal: hline
;   a0 = wks
;----------------------------------------------------------
vd_hline_asm:
    move.w  (0x8c8,a0),d0   ; x0
    move.w  (0x8cc,a0),d1   ; x1
    move.w  (0x8ca,a0),d2   ; y0
    cmp.w   (0x4b8,a0),d2   ; y0 < min_y ?
    blt.b   .3
    cmp.w   (0x4bc,a0),d2   ; y0 > max_y ?
    bgt.b   .3
    cmp.w   (0x4ba,a0),d0   ; x0 > max_x ?
    bgt.b   .3
    cmp.w   (0x4b6,a0),d1   ; x1 < min_x ?
    blt.b   .3
    cmp.w   (0x4b6,a0),d0   ; x0 > min_x ?
    bgt.b   .1
    move.w  (0x4b6,a0),d0   ; clip x0
.1: cmp.w   (0x4ba,a0),d1   ; x1 < min_x ?
    blt.b   .2
    move.w  (0x4ba,a0),d1   ; clip x1
.2: move.w  d1,d2
    sub.w   d0,d2
    cmp.w   #32,d2          ; don't bother if short
    blt.b   .4
    movem.l d0-d1/a0,-(sp)
    jsr     vd_hline_noclip_new
    tst.w   d0
    movem.l (sp)+,d0-d1/a0
    beq.b   .4
.3: rts
.4: movem.l d3-d7/a2-a6,-(sp)
    movea.l a0,a6
    move.l  vd_hline_noclip_old,a0
    jsr     (a0)
    movem.l (sp)+,d3-d7/a2-a6
    rts
vd_hline_old:
    ds.l    1


;----------------------------------------------------------
; internal: line
;   d0.w = num points
;   d1.w = 
;   a0  = points data (x0,y0,x1,y1,....)
;   a1  = wks pointer
;----------------------------------------------------------
vd_line_asm:
    cmp.w   #2,d0
    bne.b   .2
    movem.l d0-d2/a0-a2,-(sp)
    jsr     vd_line_new
    tst.w   d0
    beq.b   .1
    movem.l (sp)+,d0-d2/a0-a2
    addq.l  #4,a0
    subq.w  #2,d0
    rts
.1: movem.l (sp)+,d0-d2/a0-a2
.2: move.l  vd_line_old,-(sp)
    rts
vd_line_old:
    ds.l    1


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
    bsr     nv_banksw_handler
    cmp.b   #0,d0
    bne.b   nv_bankvec_skip
    movem.l (sp)+,d0-d2/a0-a2       ; re-run fault instruction
    rte
nv_bankvec_skip:
    movem.l (sp)+,d0-d2/a0-a2
    move.l  nv_bankvec_old,-(sp)    ; run original exception handler
    rts

nv_banksw_installvector:
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

