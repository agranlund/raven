
    .global cpu_InstallSP

    .section .note.GNU-stack,"",%progbits
    .text

cpu_InstallSP:
    movec.l     %vbr,%a1
    move.l      #sp060_isp+0x80+0x00,0xf4(%a1) // unsupported integer instruction
    movec.l     %pcr,%d0
    btst        #16,%d0
    bne.b       1f
    move.l      #sp060_fsp+0x80+0x00,0xd8(%a1) // snan
    move.l      #sp060_fsp+0x80+0x08,0xd0(%a1) // operr
    move.l      #sp060_fsp+0x80+0x10,0xd4(%a1) // overflow
    move.l      #sp060_fsp+0x80+0x18,0xcc(%a1) // underflow
    move.l      #sp060_fsp+0x80+0x20,0xc8(%a1) // divide by zero
    move.l      #sp060_fsp+0x80+0x28,0xc4(%a1) // inexact result
    move.l      #sp060_fsp+0x80+0x30,0x2c(%a1) // line-f
    move.l      #sp060_fsp+0x80+0x38,0xdc(%a1) // unsupported fpu instruction
    move.l      #sp060_fsp+0x80+0x40,0xf0(%a1) // unsupported effective address
    fmovem.l    #0,%fpcr
1:  rts

sp060_real_chk:
sp060_real_divbyzero:
    tst.b       (%sp)               // trace enabled?
    bpl.b       sp060_rte
    move.b      #0x24,0x07(%sp)     // set trace vec
sp060_real_trace:
    move.l      0x24,-(%sp)
    rts

sp060_real_trap:
sp060_isp_done:
sp060_fpsp_done:
sp060_rte:
    rte    

sp060_real_access:
    move.l      0x08,-(%sp)
    rts

sp060_real_cas:
    bra.l       sp060_isp+0x80+0x08

sp060_real_cas2:
    bra.l       sp060_isp+0x80+0x10

sp060_real_lock_page:
sp060_real_unlock_page:
    clr.l       %d0
    rts

sp060_real_fline:
    move.l      0x2c,-(%sp)
    rts

sp060_real_bsun:
    fsave       -(%sp)
    fmovem.l    %fpsr,-(%sp)
    andi.b      #0xfe,(%sp)
    fmovem.l    (%sp)+,%fpsr
    lea         12(%sp),%sp
    fmovem.l    #0,%fpcr
    rte

sp060_real_fpu_disabled:
    move.l      %d0,-(%sp)      // save regs
    movec.l     %pcr,%d0
    bclr        #1,%d0          // enable fpu
    movec.l     %d0,%pcr
    move.l      (%sp)+,%d0      // restore regs
    move.l      12(%sp),2(%sp)
    fmovem.l    #0,%fpcr
    rte


#if 1
sp060_real_ovfl:
sp060_real_unfl:
sp060_real_operr:
sp060_real_snan:
sp060_real_dz:
sp060_real_inex:
    fsave       -(%sp)
    move.w      #0x6000,2(%sp)
    frestore    (%sp)+
    fmovem.l    #0,%fpcr
    rte
#else
sp060_real_ovfl:
    move.l      0xd4,-(%sp)
    bra.b       sp060_clear_fpu_exception_and_go
sp060_real_unfl:
    move.l      0xcc,-(%sp)
    bra.b       sp060_clear_fpu_exception_and_go
sp060_real_operr:
    move.l      0xd0,-(%sp)
    bra.b       sp060_clear_fpu_exception_and_go
sp060_real_snan:
    move.l      0xd8,-(%sp)
    bra.b       sp060_clear_fpu_exception_and_go
sp060_real_dz:
    move.l      0xc8,-(%sp)
    bra.b       sp060_clear_fpu_exception_and_go
sp060_real_inex:
    move.l      0xc4,-(%sp)
    bra.b       sp060_clear_fpu_exception_and_go
sp060_clear_fpu_exception_and_go:
    fsave       -(%sp)
    move.w      #0x6000,2(%sp)
    frestore    (%sp)+
    fmovem.l    #0,%fpcr
    rts
#endif

sp060_imem_read:
sp060_dmem_read:
    subq.l      #1,%d0          // d0 = count
    btst        #5,4(%a6)       // super?
    bne.s       2f
    moveq.l     #1,%d1          // user read
    movec       %d1,%sfc        // source = userspace
1:  moves.b     (%a0)+,%d1
    move.b      %d1,(%a1)+
    dbra        %d0,1b
    bra.b       3f
2:  move.b      (%a0)+,(%a1)+   // super read
    dbra        %d0,2b
3:  clr.l       %d1
    rts

sp060_dmem_write:
    subq.l      #1,%d0          // d0 = count
    btst        #5,4(%a6)       // super?
    bne.s       2f
    moveq.l     #1,%d1          // user write
    movec       %d1,%dfc        // dest = userspace
1:  move.b      (%a0)+,%d1
    moves.b     %d1,(%a1)+
    dbra        %d0,1b
    bra.b       3f
2:  move.b      (%a0)+,(%a1)+   // super write
    dbra        %d0,2b
3:  clr.l       %d1
    rts

sp060_dmem_read_byte:
    clr.l       %d0
    btst        #5,4(%a6)
    bne.b       2f
    moveq.l     #1,%d1
    movec       %d1,%sfc
    moves.b     (%a0),%d0
    bra.b       3f
2:  move.b      (%a0),%d0
3:  clr.l       %d1
    rts

sp060_dmem_read_word:
sp060_imem_read_word:
    clr.l       %d0
    btst        #5,4(%a6)
    bne.b       2f
    moveq.l     #1,%d1
    movec       %d1,%sfc
    moves.w     (%a0),%d0
    bra.b       3f
2:  move.w      (%a0),%d0
3:  clr.l       %d1
    rts

sp060_dmem_read_long:
sp060_imem_read_long:
    btst        #5,4(%a6)
    bne.b       2f
    moveq.l     #1,%d1
    movec       %d1,%sfc
    moves.l     (%a0),%d0
    bra.b       3f
2:  move.l      (%a0),%d0
3:  clr.l       %d1
    rts

sp060_dmem_write_byte:
    btst        #5,4(%a6)
    bne.b       2f
    moveq.l     #1,%d1
    movec       %d1,%dfc
    moves.b     %d0,(%a0)
    bra.b       3f
2:  move.b      %d0,(%a0)
3:  clr.l       %d1
    rts

sp060_dmem_write_word:
    btst        #5,4(%a6)
    bne.b       2f
    moveq.l     #1,%d1
    movec       %d1,%dfc
    moves.w     %d0,(%a0)
    bra.b       3f
2:  move.w      %d0,(%a0)
3:  clr.l       %d1
    rts

sp060_dmem_write_long:
    btst        #5,4(%a6)
    bne.b       2f
    moveq.l     #1,%d1
    movec       %d1,%dfc
    moves.l     %d0,(%a0)
    bra.b       3f
2:  move.l      %d0,(%a0)
3:  clr.l       %d1
    rts

align 16
.global sp060_isp
sp060_isp:
    .dc.l sp060_real_chk-sp060_isp        // or 0xF4
    .dc.l sp060_real_divbyzero-sp060_isp  // or 0xF4
    .dc.l sp060_real_trace-sp060_isp
    .dc.l sp060_real_access-sp060_isp
    .dc.l sp060_isp_done-sp060_isp
    .dc.l sp060_real_cas-sp060_isp
    .dc.l sp060_real_cas2-sp060_isp
    .dc.l sp060_real_lock_page-sp060_isp
    .dc.l sp060_real_unlock_page-sp060_isp
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .dc.l sp060_imem_read-sp060_isp
    .dc.l sp060_dmem_read-sp060_isp
    .dc.l sp060_dmem_write-sp060_isp
    .dc.l sp060_imem_read_word-sp060_isp
    .dc.l sp060_imem_read_long-sp060_isp
    .dc.l sp060_dmem_read_byte-sp060_isp
    .dc.l sp060_dmem_read_word-sp060_isp
    .dc.l sp060_dmem_read_long-sp060_isp
    .dc.l sp060_dmem_write_byte-sp060_isp
    .dc.l sp060_dmem_write_word-sp060_isp
    .dc.l sp060_dmem_write_long-sp060_isp
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .include "hw/motorola_isp.sa"

align 16
.global sp060_fsp
sp060_fsp:
    .dc.l sp060_real_bsun-sp060_fsp
    .dc.l sp060_real_snan-sp060_fsp
    .dc.l sp060_real_operr-sp060_fsp
    .dc.l sp060_real_ovfl-sp060_fsp
    .dc.l sp060_real_unfl-sp060_fsp
    .dc.l sp060_real_dz-sp060_fsp
    .dc.l sp060_real_inex-sp060_fsp
    .dc.l sp060_real_fline-sp060_fsp          // or 0x2C
    .dc.l sp060_real_fpu_disabled-sp060_fsp
    .dc.l sp060_real_trap-sp060_fsp
    .dc.l sp060_real_trace-sp060_fsp
    .dc.l sp060_real_access-sp060_fsp
    .dc.l sp060_fpsp_done-sp060_fsp
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .dc.l sp060_imem_read-sp060_fsp
    .dc.l sp060_dmem_read-sp060_fsp
    .dc.l sp060_dmem_write-sp060_fsp
    .dc.l sp060_imem_read_word-sp060_fsp
    .dc.l sp060_imem_read_long-sp060_fsp
    .dc.l sp060_dmem_read_byte-sp060_fsp
    .dc.l sp060_dmem_read_word-sp060_fsp
    .dc.l sp060_dmem_read_long-sp060_fsp
    .dc.l sp060_dmem_write_byte-sp060_fsp
    .dc.l sp060_dmem_write_word-sp060_fsp
    .dc.l sp060_dmem_write_long-sp060_fsp
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .dc.l 0
    .include "hw/motorola_fpsp.sa"
