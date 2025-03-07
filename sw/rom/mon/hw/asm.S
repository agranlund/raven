
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
//
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

cpu_CacheFlush:
    move.w %sr,%d1         // disable interrupts
    or.w    #0x0700,%sr
    movec.l %cacr,%d0
    btst.l  #31,%d0         // data cache enabled?
    beq.b   1f
    nop
    cpusha  %dc             // push data cache
1:  nop
    btst.l  #15,%d0         // instruction cache enabled?
    beq.b   2f
    nop
    cpusha  %ic             // push instruction cache
2:  nop
    bset.l  #22,%d0         // clear branch cache
    nop
    movec.l %d0,%cacr
    nop
    move.w  %d1,%sr         // restore interrupts
    rts

cpu_CacheOn:
    move.w  %sr,%d1         // disable interrupts
    or.w    #0x0700,%sr
    movec.l %cacr,%d0
    btst.l  #31,%d0         // data cache enabled?
    beq.b   1f
    nop
    cpusha  %dc             // push data cache
1:  nop
    btst.l  #15,%d0         // instruction cache enabled?
    beq.b   2f
    nop
    cpusha  %ic             // push instruction cache
2:  nop
    move.l  #0xa0c08000,%d0
    nop
    cinva   %bc             // invalidate caches
    nop
    movec.l %d0,%cacr       // enable caches
    nop
    move.w  %d1,%sr         // restore interrupts
    rts

cpu_CacheOff:
    move.w  %sr,%d1         // disable interrupts
    or.w    #0x0700,%sr
    movec.l %cacr,%d0
    btst.l  #31,%d0         // data cache enabled?
    beq.b   1f
    nop
    cpusha  %dc             // push data cache
1:  nop
    btst.l  #15,%d0         // instruction cache enabled?
    beq.b   2f
    nop
    cpusha  %ic             // push instruction cache
2:  nop
    move.l  #0x00000000,%d0
    movec.l %d0,%cacr       // disable caches
    nop
    move.w  %d1,%sr
    rts


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

cpu_GetCACR:
    movec.l %cacr,%d0
    rts

cpu_SetCACR:
    move.l  4(%a7),%d1
    or.l    #0x00400000,%d1       // clear branch cache
    nop
    cpusha  %bc
    nop
    movec.l %cacr,%d0
    movec.l %d1,%cacr
    nop
    rts

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
