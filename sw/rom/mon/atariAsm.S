    #include "sys.h"
    #include "atari.h"
    #include "hw/mfp.h"
    #include "hw/uart.h"
    
    .text
    .global _vecRTE_MFP1I0
    .global _vecRTE_MFP1I1
    .global _vecRTE_MFP1I2
    .global _vecRTE_MFP1I3
    .global _vecRTE_MFP1I4
    .global _vecRTE_MFP1I5
    .global _vecRTE_MFP1I6
    .global _vecRTE_MFP1I7
    .global _vecRTE_MFP2I0

    .global _vecVBL_MPF2TC
    .global _vecVBL_MPF2I0
    .global _vecKBD_MFP1I4
    .extern _vecKBD_Busy

// ----------------------------------------------------------
// MFP interrupt blockers
// ----------------------------------------------------------

_vecRTE_MFP1I0:  // 0x100
    move.b  #0xfe,0xfffffa11        // clear lpt1 in service
    rte
_vecRTE_MFP1I1:  // 0x104
    move.b  #0xfd,0xfffffa11        // clear cd in service
    rte
_vecRTE_MFP1I2:  // 0x108
    move.b  #0xfb,0xfffffa11        // clear cts in service
    rte
_vecRTE_MFP1I3:  // 0x10C
    move.b  #0xf7,0xfffffa11        // clear blitter in service
    rte
_vecRTE_MFP1I4:  // 0x118
    move.b  #0xbf,0xfffffa11        // clear acia in service
    rte
_vecRTE_MFP1I5:  // 0x11C
    move.b  #0x7f,0xfffffa11        // clear fdd/hdd in service
    rte
_vecRTE_MFP1I6:  // 0x138
    move.b  #0xbf,0xfffffa13        // clear ri in service
    rte
_vecRTE_MFP1I7:  // 0x13C
    move.b  #0x7f,0xfffffa13        // clear mono in service
    rte


// ----------------------------------------------------------
// Vblank emulation
// ----------------------------------------------------------

_vecVBL_MPF2TC:
    move.w   #0x2700,sr
    move.l  d0,-(sp)
    move.w  4(sp),d0                // get current ipl
    and.w   #0x0F00,d0
    cmp.w   #0x0300,d0              // skip if >= 4
    bhi.b   0f
    move.l  (sp)+,d0
	move.l	0x70,-(sp)              // target vector
    move.b  #0xDF,PADDR_MFP2+0x11   // clear timerC in-service
    move.w  #0x2400,sr              // pretend ipl level 4
	rts                             // go IRQ4
0:  move.l  (sp)+,d0
    move.b  #0xDF,PADDR_MFP2+0x11   // clear timerC in-service
    rte


_vecVBL_MFP2I0:
    move.w   #0x2700,sr
    move.l  d0,-(sp)
    move.w  4(sp),d0                // get current ipl
    and.w   #0x0F00,d0
    cmp.w   #0x0300,d0              // skip if >= 4
    bhi.b   0f
    move.l  (sp)+,d0
	move.l	0x70,-(sp)              // target vector
    move.b  #0xFE,PADDR_MFP2+0x11   // clear ISA9 in-service
    move.w  #0x2400,sr              // pretend ipl level 4
	rts                             // go IRQ4
0:  move.l  (sp)+,d0
    move.b  #0xFE,PADDR_MFP2+0x11   // clear ISA9 in-service
    rte


// ----------------------------------------------------------
// ACIA emulation
// ----------------------------------------------------------

_vecKBD_MFP1I4:
    bset.b  #0,_vecKBD_Busy
    beq.b   0f
    rte

0:  movem.l d0-d1,-(sp)                         // save regs
    moveq.l #7,d1                               // max number of bytes to get from fifo
1:  btst.b  #0,PADDR_UART1+UART_LSR             // rx fifo empty?
    bne.b   3f
    
2:  movem.l (sp)+,d0-d1                         // restore regs
    move.b  #0xbf,PADDR_MFP1+MFP_ISRB           // clear in-service
    bclr.b  #0,_vecKBD_Busy
    rte                                         // and return

3:  move.b  PADDR_UART1+UART_RHR,d0             // get rx byte from uart
    move.b  d0,PADDR_ACIA+ACIA_KEYB_DATA        // acia data
    move.b  #0x83,PADDR_ACIA+ACIA_KEYB_CTRL     // acia ctrl : interrupt, tx empty, rx full

    move.l  d1,-(sp)                            // save regs
    clr.w   -(sp)                               // fake stackframe
    pea     4f
    move.w  sr,-(sp)
    jmp     ([0x118.w])                         // call acia interrupt handler, may clear in-service flag

4:  move.b  #0x02,PADDR_ACIA+ACIA_KEYB_CTRL     // acia ctrl: no interrupt, tx empty, rx empty
    move.l  (sp)+,d1                            // restore regs
    dbra.w  d1,1b                               // next byte from fifo

