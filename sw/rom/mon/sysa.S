    #include "sys.h"
    
    .text
    .global  _StartMonitor
    .global  _GetIPL
    .global  _SetIPL
    .global  _GetCACR
    .global  _SetCACR
    .global  _GetPCR
    .global  _SetPCR
    .global  _GetVBR
    .global  _SetVBR
    .global  _GetMMU
    .global  _SetMMU
    .global  _Call
    .global  _vecMFP_I0
    .global  _vecMFP_I1
    .global  _vecMFP_I2
    .global  _vecMFP_I3
    .global  _vecMFP_I4
    .global  _vecMFP_I5
    .global  _vecMFP_I6
    .global  _vecMFP_I7
    .global  _vecNMI
    .global  _vecRTE


// ----------------------------------------------------------
    .macro pushrl,r
        sub.l   d0,d0
        movec.l \r,d0
        move.l  d0,-(a7)
    .endm

// ----------------------------------------------------------
    .macro callMonitor
        move.l  a7,-(a7)            // registers
        movem.l d0-d7/a0-a6,-(a7)
        pushrl  usp
        pushrl  pcr
        pushrl  vbr
        pushrl  sfc
        pushrl  dfc
        pushrl  cacr
        pushrl  urp
        pushrl  srp
        pushrl  tc
        pushrl  dtt0
        pushrl  dtt1
        pushrl  itt0
        pushrl  itt1
        pushrl  buscr
        move.l  a7,-(a7)            // push args
        bsr     _Monitor            // Monitor(regs*)
        add.l   #4,a7               // get rid of args
        add.l   #56,a7              // get rid of special regs
        movem.l (a7)+,d0-d7/a0-a6   // restore regs
        add.l   #4,a7               // ignore a7
    .endm

// ----------------------------------------------------------
_StartMonitor:
    move.w  #0,-(a7)            // fake frame
    move.l  2(a7),-(a7)
    move.w  sr,-(a7)
    callMonitor
    add.l   #8,a7               // fake frame
    rts

// ----------------------------------------------------------
_vecNMI:
    move.w  #0x2700,sr
    callMonitor
    rte

// ----------------------------------------------------------
_vecRTE:
    rte

// ----------------------------------------------------------
_vecMFP_I0:  // 0x100
    move.b  #0xfe,0xfffffa11        // clear lpt1 in service
    rte
_vecMFP_I1:  // 0x104
    move.b  #0xfd,0xfffffa11        // clear cd in service
    rte
_vecMFP_I2:  // 0x108
    move.b  #0xfb,0xfffffa11        // clear cts in service
    rte
_vecMFP_I3:  // 0x10C
    move.b  #0xf7,0xfffffa11        // clear blitter in service
    rte
_vecMFP_I4:  // 0x118
    move.b  #0xbf,0xfffffa11        // clear acia in service
    rte
_vecMFP_I5:  // 0x11C
    move.b  #0x7f,0xfffffa11        // clear fdd/hdd in service
    rte
_vecMFP_I6:  // 0x138
    move.b  #0xbf,0xfffffa13        // clear ri in service
    rte
_vecMFP_I7:  // 0x13C
    move.b  #0x7f,0xfffffa13        // clear mono in service
    rte


// ----------------------------------------------------------

// ----------------------------------------------------------


// ----------------------------------------------------------
_GetIPL:
    move.w  sr,d0
    lsr.w   #8,d0
    and.w   #0x000F,d0
    rts

_SetIPL:
    move.w  4(a7),d0
    lsl.w   #8,d0
    and.w   #0x0F00,d0
    move.w  sr,d1
    and.w   #0xF0FF,d1
    or.w    d0,d1
    move.w  d1,sr
    nop
    rts

// ----------------------------------------------------------
_GetCACR:
    movec.l cacr,d0
    rts

_SetCACR:
    move.l  4(a7),d0
    or.l    #0x00400000,d0        // clear branch cache
    nop
    cpusha  bc
    nop
    movec.l d0,cacr
    nop
    rts

// ----------------------------------------------------------
_GetPCR:
    movec.l pcr,d0
    rts

_SetPCR:
    move.l  4(a7),d0
    nop
    movec.l d0,pcr
    nop
    rts

// ----------------------------------------------------------
_GetVBR:
    movec.l vbr,d0
    rts

_SetVBR:
    move.l  4(a7),d0
    nop
    movec.l d0,vbr
    nop
    cpusha  bc
    nop
    rts

// ----------------------------------------------------------
_GetMMU:
    move.l  4(a7),a0        // mmu struct
    movec.l urp,d0
    move.l  d0,(a0)+
    movec.l srp,d0
    move.l  d0,(a0)+
    movec.l tc,d0
    move.l  d0,(a0)+
    movec.l itt0,d0
    move.l  d0,(a0)+
    movec.l itt1,d0
    move.l  d0,(a0)+
    movec.l dtt0,d0
    move.l  d0,(a0)+
    movec.l dtt1,d0
    move.l  d0,(a0)+
    rts

_SetMMU:
    move.l  4(a7),a0
    moveq.l #0,d0           // turn off mmu
    movec.l d0,tc
    nop
    cpusha bc
    nop
    pflusha
    nop
    move.l  (a0)+,d0
    movec.l d0,urp
    move.l  (a0)+,d0
    movec.l d0,srp
    move.l  (a0)+,d0
    movec.l d0,tc
    move.l  (a0)+,d0
    movec.l d0,itt0
    move.l  (a0)+,d0
    movec.l d0,itt1
    move.l  (a0)+,d0
    movec.l d0,dtt0
    move.l  (a0)+,d0
    movec.l d0,dtt0
    nop
    cpusha bc
    nop
    pflusha
    nop
    rts

_Call:
    move.l  4(a7),d0            // d0 = target address
    movem.l d2-d7/a2-a6,-(a7)   // save C registers
    move.l  #_CallRet,-(a7)     // return address
    move.l  d0,-(a7)            // call address
    nop
    cpusha  bc
    cpusha  dc
    cpusha  ic
    nop
    rts                         // jump
_CallRet:
    movem.l (a7)+,d0-d7/a0-a6   // restore C registers
    rts                         // d0 = return value

