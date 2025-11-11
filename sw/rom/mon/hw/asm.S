
    .global cpu_GetIPL
    .global cpu_SetIPL
    .global cpu_GetCACR
    .global cpu_SetCACR
    .global cpu_GetPCR
    .global cpu_SetPCR
    .global cpu_GetVBR
    .global cpu_SetVBR
    .global cpu_GetMMU
    .global cpu_SetMMU
    .global cpu_FlushMMU
    .global mmu_Flush
    .global cpu_Call
    .global cpu_TriggerNMI

    .global cpu_Lock
    .global cpu_Unlock

    .global cpu_CacheOn
    .global cpu_CacheOff
    .global cpu_CacheFlush

    .global vecNMI
    .global vecRTE

    .global NMIFunc
    .global NMIBusy

    .global cpu_SafeReadByte
    .global cpu_SafeReadWord
    .global cpu_SafeReadLong

    .global cpu_SafeWriteByte
    .global cpu_SafeWriteWord
    .global cpu_SafeWriteLong

    .global cpu_InstallSP

    .section .bss.asm
NMIFunc:    .space 4
NMIBusy:    .space 4

    .section .note.GNU-stack,"",%progbits
    .text

// ----------------------------------------------------------
    .macro pushrl,r
        sub.l   %d0,%d0
        movec.l \r,%d0
        move.l  %d0,-(%a7)
    .endm

// ----------------------------------------------------------
    .macro callNMI
        move.l  %a7,-(%a7)           // registers
        movem.l %d0-%d7/%a0-%a6,-(%a7)
        pushrl  %usp
        pushrl  %pcr
        pushrl  %vbr
        pushrl  %sfc
        pushrl  %dfc
        pushrl  %cacr
        pushrl  %urp
        pushrl  %srp
        pushrl  %tc
        pushrl  %dtt0
        pushrl  %dtt1
        pushrl  %itt0
        pushrl  %itt1
        pushrl  %buscr
        move.l  %a7,-(%a7)          // push args
        move.l  #1f,-(%a7)          // push return address from call
        cmp.l   #0,NMIFunc
        beq.b   1f
        move.l  NMIFunc,-(%a7)      // push nmi handler address
        rts                         // call it
1:      addq.l  #4,%a7              // get rid of args
        add.l   #56,%a7             // get rid of special regs
        movem.l (%a7)+,%d0-%d7/%a0-%a6 // restore regs
        add.l   #4,%a7              // ignore a7
    .endm

// ----------------------------------------------------------
cpu_TriggerNMI:

    // todo: run in our own stack

    move.w  #0,-(%a7)           // fake frame
    move.l  2(%a7),-(%a7)
    move.w  %sr,-(%a7)
    callNMI
    add.l   #8,%a7              // fake frame
    rts

// ----------------------------------------------------------
vecNMI:
    move.w  #0x2700,%sr
    bset    #0,NMIBusy
    bne.w   2f
    callNMI
    bclr    #0,NMIBusy
2:  rte

// ----------------------------------------------------------
vecRTE:
    rte



// ----------------------------------------------------------
cpu_Lock:
    move.l  4(%a7),%a0
    bset.b  #0,(%a0)
    bne.b   1f
    moveq.l #1,%d0
    rts
1:  moveq.l #0,%d0
    rts

cpu_Unlock:
    move.l  4(%a7),%a0
    bclr.b  #0,(%a0)
    rts

// ----------------------------------------------------------
cpu_GetIPL:
    move.w  %sr,%d0
    lsr.w   #8,%d0
    and.l   #0x00000007,%d0
    rts

cpu_SetIPL:
    move.l  4(%a7),%d0
    lsl.w   #8,%d0
    and.w   #0x0700,%d0
    move.w  %sr,%d1
    and.w   #0xF0FF,%d1
    or.w    %d0,%d1
    move.w  %sr,%d0
    lsr.w   #8,%d0
    and.l   #0x00000007,%d0
    move.w  %d1,%sr
    nop
    rts


cpu_CacheFlush:
    move.w  %sr,%d1         // disable interrupts
    or.w    #0x0700,%sr
    movec.l %cacr,%d0
    bset.l  #22,%d0         // clear branch cache
    btst.l  #31,%d0         // data cache enabled?
    beq.b   1f
    nop
    cpusha  %dc             // push data cache
1:  nop
    cinva   %bc             // invalidate both caches
    nop
    movec.l %d0,%cacr       // clear branch cache
    nop
    move.w  %d1,%sr         // restore interrupts
    rts

cpu_CacheOn:
    move.l  #0xa0c08000,%d1
    bra.b   cpu_SetCACR_asmcall

cpu_CacheOff:
    moveq.l #0,%d1
    bra.b   cpu_SetCACR_asmcall

cpu_GetCACR:
    movec.l %cacr,%d0
    rts

cpu_SetCACR:
    move.l  4(%a7),%d1          // d1 = new cacr
cpu_SetCACR_asmcall:
    movec.l %cacr,%d0           // d0 = old cacr
    cmp.l   %d0,%d1             // ignore if same
    bne.b   1f
    rts
1:  move.w  %sr,-(%a7)          // save sr
    or.w    #0x0700,%sr         // disable interrupts
    move.l  %d2,-(%a7)          // save d2
    move.l  #0x00400000,%d2     // d2 = tmp cacr
    nop
    movec.l %d2,%cacr           // disabled caches
    btst    #31,%d0             // datacache was enabled?
    beq.b   2f
    nop
    cpusha  %dc                 // yes, push data cache
2:  nop
    cinva   %bc                 // invalidate both caches
    nop
    movec.l %d1,%cacr           // set new cache state
    nop
    move.l  (%a7)+,%d2          // restore d2
    move.w  (%a7)+,%sr          // restore interrupts
    rts

// ----------------------------------------------------------
cpu_GetPCR:
    movec.l %pcr,%d0
    rts

cpu_SetPCR:
    move.l  4(%a7),%d1
    nop
    movec.l %pcr,%d0
    movec.l %d1,%pcr
    nop
    rts

// ----------------------------------------------------------
cpu_GetVBR:
    movec.l %vbr,%d0
    rts

cpu_SetVBR:
    move.l  4(%a7),%d1
    nop
    movec.l %vbr,%d0
    movec.l %d1,%vbr
    nop
    cpusha  %bc
    nop
    rts

// ----------------------------------------------------------
cpu_GetMMU:
    move.l  4(%a7),%a0      // mmu struct
    movec.l %urp,%d0
    move.l  %d0,(%a0)+
    movec.l %srp,%d0
    move.l  %d0,(%a0)+
    movec.l %tc,%d0
    move.l  %d0,(%a0)+
    movec.l %itt0,%d0
    move.l  %d0,(%a0)+
    movec.l %itt1,%d0
    move.l  %d0,(%a0)+
    movec.l %dtt0,%d0
    move.l  %d0,(%a0)+
    movec.l %dtt1,%d0
    move.l  %d0,(%a0)+
    rts

cpu_SetMMU:
    move.l  4(%a7),%a0
    moveq.l #0,%d0          // turn off mmu
    movec.l %d0,%tc
    nop
    cpusha %bc
    nop
    pflusha
    nop
    move.l  (%a0)+,%d0
    movec.l %d0,%urp
    move.l  (%a0)+,%d0
    movec.l %d0,%srp
    move.l  (%a0)+,%d0
    movec.l %d0,%tc
    move.l  (%a0)+,%d0
    movec.l %d0,%itt0
    move.l  (%a0)+,%d0
    movec.l %d0,%itt1
    move.l  (%a0)+,%d0
    movec.l %d0,%dtt0
    move.l  (%a0)+,%d0
    movec.l %d0,%dtt0
    nop
    cpusha %bc
    nop
    pflusha
    nop
    rts

cpu_FlushMMU:
mmu_Flush:
    nop
    pflusha
    nop
    rts


// ----------------------------------------------------------
cpu_Call:
    move.l  4(%a7),%d0              // d0 = target address
    movem.l %d2-%d7/%a2-%a6,-(%a7)  // save C registers
    move.l  #1f,-(%a7)              // return address
    move.l  %d0,-(%a7)              // call address
    sub.l   %d0,%d0                 // clear return value
    nop
    cpusha  %bc                     // invalidate cache
    nop
    rts                             // jump
1:  movem.l (%a7)+,%d2-%d7/%a2-%a6  // restore C registers
    rts                             // d0 = return value



// ----------------------------------------------------------
cpu_SafeReadLong:
    move.l  %a7,%a1             // save stack pointer
    move.l  0x08,%d1            // save berr
    move.l  #cpu_SafeDone,0x8   // set berr
    move.l  #0xffffffff,%d0     // default value
    move.l  4(%a7),%a0          // get address
    move.l  (%a0),%d0           // read from address
    bra.b   cpu_SafeDone

cpu_SafeReadWord:
    move.l  %a7,%a1             // save stack pointer
    move.l  0x08,%d1            // save berr
    move.l  #cpu_SafeDone,0x8   // set berr
    move.w  #0xffff,%d0         // default value
    move.l  4(%a7),%a0          // get address
    move.w  (%a0),%d0           // read from address
    bra.b   cpu_SafeDone

cpu_SafeReadByte:
    move.l  %a7,%a1             // save stack pointer
    move.l  0x08,%d1            // save berr
    move.l  #cpu_SafeDone,0x8   // set berr
    move.b  #0xff,%d0           // default value
    move.l  4(%a7),%a0          // get address
    move.b  (%a0),%d0           // read from address

cpu_SafeDone:
    move.l  %a1,%a7             // restore stack
    move.l  %d1,0x08            // restore berr
    rts

cpu_SafeWriteByte:
    move.l  %a7,%a1             // save stack pointer
    move.l  0x08,%d1            // save berr
    move.l  #cpu_SafeDone,0x8   // set berr
    move.l  4(%a7),%a0          // address
    move.l  8(%a7),%d0          // data
    move.b  %d0,(%a0)           // write
    bra.b   cpu_SafeDone

cpu_SafeWriteWord:
    move.l  %a7,%a1             // save stack pointer
    move.l  0x08,%d1            // save berr
    move.l  #cpu_SafeDone,0x8   // set berr
    move.l  4(%a7),%a0          // address
    move.l  8(%a7),%d0          // data
    move.w  %d0,(%a0)           // write
    bra.b   cpu_SafeDone

cpu_SafeWriteLong:
    move.l  %a7,%a1             // save stack pointer
    move.l  0x08,%d1            // save berr
    move.l  #cpu_SafeDone,0x8   // set berr
    move.l  %a7,%a1             // save stack pointer
    move.l  4(%a7),%a0          // address
    move.l  8(%a7),%d0          // data
    move.l  %d0,(%a0)           // write
    bra.b   cpu_SafeDone


// ----------------------------------------------------------
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
