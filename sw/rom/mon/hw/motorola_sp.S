
    .global cpu_InstallSP
    .global cpu_UninstallSP

	.bss
sp060_old_00: ds.l 1
sp060_old_f4: ds.l 1
sp060_old_d8: ds.l 1
sp060_old_d0: ds.l 1
sp060_old_d4: ds.l 1
sp060_old_cc: ds.l 1
sp060_old_c8: ds.l 1
sp060_old_c4: ds.l 1
sp060_old_2c: ds.l 1
sp060_old_dc: ds.l 1
sp060_old_f0: ds.l 1

    .section .note.GNU-stack,"",%progbits
    .text

cpu_InstallSP:
	move.w		%sr,-(%sp)
	or.w		#0x0700,%sr
    move.l      6(%sp),%a0
	move.l		%a0,sp060_old_00

	move.l		0xf4(%a0),sp060_old_f4
    move.l      #sp060_isp+0x80+0x00,0xf4(%a0) // unsupported integer instruction

    movec.l     %pcr,%d0
    btst        #16,%d0
    bne.w       1f

	move.l		0xd8(%a0),sp060_old_d8
    move.l      #sp060_fsp+0x80+0x00,0xd8(%a0) // snan

	move.l		0xd0(%a0),sp060_old_d0
    move.l      #sp060_fsp+0x80+0x08,0xd0(%a0) // operr

	move.l		0xd4(%a0),sp060_old_d4
    move.l      #sp060_fsp+0x80+0x10,0xd4(%a0) // overflow

	move.l		0xcc(%a0),sp060_old_cc
    move.l      #sp060_fsp+0x80+0x18,0xcc(%a0) // underflow

	move.l		0xc8(%a0),sp060_old_c8
    move.l      #sp060_fsp+0x80+0x20,0xc8(%a0) // divide by zero

	move.l		0xc4(%a0),sp060_old_c4
    move.l      #sp060_fsp+0x80+0x28,0xc4(%a0) // inexact result

	move.l		0x2c(%a0),sp060_old_2c
    move.l      #sp060_fsp+0x80+0x30,0x2c(%a0) // line-f

	move.l		0xdc(%a0),sp060_old_dc
    move.l      #sp060_fsp+0x80+0x38,0xdc(%a0) // unsupported fpu instruction

	move.l		0xf0(%a0),sp060_old_f0
    move.l      #sp060_fsp+0x80+0x40,0xf0(%a0) // unsupported effective address

    fmovem.l    #0,%fpcr
1:	move.w		(%sp)+,%sr
	rts

cpu_UninstallSP:
	move.w		%sr,-(%sp)
	or.w		#0x0700,%sr
    move.l      sp060_old_00,%a0
    move.l      sp060_old_f4,0xf4(%a0) // unsupported integer instruction
    movec.l     %pcr,%d0
    btst        #16,%d0
    bne.b       1f
    move.l      sp060_old_d8,0xd8(%a0) // snan
    move.l      sp060_old_d0,0xd0(%a0) // operr
    move.l      sp060_old_d4,0xd4(%a0) // overflow
    move.l      sp060_old_cc,0xcc(%a0) // underflow
    move.l      sp060_old_c8,0xc8(%a0) // divide by zero
    move.l      sp060_old_c4,0xc4(%a0) // inexact result
    move.l      sp060_old_2c,0x2c(%a0) // line-f
    move.l      sp060_old_dc,0xdc(%a0) // unsupported fpu instruction
    move.l      sp060_old_f0,0xf0(%a0) // unsupported effective address
    fmovem.l    #0,%fpcr
1:	move.w		(%sp)+,%sr
	rts


sp060_xmem_copy:
	move.b	(%a0)+,(%a1)+
sp060_imem_read:
sp060_dmem_read:
sp060_dmem_write:
	dbf.w	%d0,sp060_xmem_copy
	clr.l	%d1
	rts

sp060_dmem_read_byte:
    clr.l       %d0
	clr.l       %d1
	move.b      (%a0),%d0
    rts

sp060_dmem_read_word:
    clr.l       %d0
sp060_imem_read_word:
	clr.l       %d1
	move.w      (%a0),%d0
    rts

sp060_dmem_read_long:
sp060_imem_read_long:
	clr.l       %d1
	move.l      (%a0),%d0
    rts

sp060_dmem_write_byte:
	clr.l       %d1
	move.b      %d0,(%a0)
    rts

sp060_dmem_write_word:
	clr.l       %d1
	move.w      %d0,(%a0)
    rts

sp060_dmem_write_long:
	clr.l       %d1
	move.l      %d0,(%a0)
    rts

sp060_real_chk:
sp060_real_divbyzero:
sp060_real_trace:
sp060_real_access:
sp060_isp_done:
	rte

sp060_real_lock_page:
sp060_real_unlock_page:
    clr.l       %d0
    rts

sp060_real_fline:
	move.l		sp060_old_2c,-(%sp)
	rts

sp060_real_trap:
sp060_fpsp_done:
    rte    

sp060_real_cas:
    bra.l       sp060_isp+0x80+0x08

sp060_real_cas2:
    bra.l       sp060_isp+0x80+0x10

sp060_real_bsun:
    fsave       -(%sp)
    fmovem.l    %fpsr,-(%sp)
    andi.b      #0xfe,(%sp)
    fmovem.l    (%sp)+,%fpsr
    lea         12(%sp),%sp
    rte

sp060_real_snan:
sp060_real_operr:
sp060_real_ovfl:
sp060_real_unfl:
sp060_real_dz:
sp060_real_inex:
    fsave       -(%sp)
    move.w      #0x6000,2(%sp)
    frestore    (%sp)+
    rte							// ct60tos jumps to old vec, fpsp does rte

sp060_real_fpu_disabled:
    move.l      %d0,-(%sp)      // save regs
    movec.l     %pcr,%d0
    bclr        #1,%d0          // enable fpu
    movec.l     %d0,%pcr
    move.l      (%sp)+,%d0      // restore regs
    move.l      12(%sp),2(%sp)
    fmovem.l    #0,%fpcr
    rte


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
