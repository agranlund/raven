/*
 * raven2.S - Raven specific assembler functions
 *
 * Copyright (C) 2024 The EmuTOS development team
 *
 * Authors:
 *  Anders Granlund
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

#define __RAVEN__ASM__
#include "raven.h"
#undef __RAVEN__ASM__

#ifdef MACHINE_RAVEN

    .globl  raven_memconf
    .global _raven_int_ikbd
    .global _raven_int_vbl
    .extern ikbdraw
    .extern _int_vbl

/******************************************************************************/
/* Startup                                                                    */
/******************************************************************************/

        .globl  raven_startup
raven_startup:
        // detect warm reset and perform cold reset instead
        tst.w   _longframe.w
        jne     _cold_reset
        jmp     (a6)



/******************************************************************************/
/* Asm helpers                                                                */
/******************************************************************************/

raven_memconf:
        move.l  #0x00000000,a0              // range min
        move.l  #0x00400000,a1              // range max
        move.l  #1f,0x08                    // bus error at end of ram
0:      move.l  (a0),d0                     // try read
        add.l   #0x00100000,a0              // check next block
        cmp.l   a0,a1
        bne.b   0b
1:      move.l  a0,d5                       // d5 = st ram size
        moveq   #0,d6                       // fake memctrl
        jmp     (a6)                        // done


/******************************************************************************/
/* Interrupts                                                                 */
/******************************************************************************/

.text

_raven_int_ikbd:
    move.w  #0x2700,sr
    movem.l d0-d2/a0-a1,-(sp)
    move.w  20(sp),d0
    and.w   #0x0F00,d0                      /* get current ipl          */
    cmp.w   #0x0500,d0                      /* skip if >= 6             */
    bhi.b   1f
    move.w  #0x2600,sr                      /* set level 6              */
    moveq.l #7,d2
0:  btst.b  #0,RAVEN_UART1_BASE+0x14
    beq.b   1f
    moveq   #0,d0
    move.b  RAVEN_UART1_BASE+0x00,d0
    jsr     ikbdraw
    dbra.w  d2,0b
1:  movem.l (sp)+,d0-d2/a0-a1
    rte

_raven_int_vbl:
    jsr     _int_vbl        /* call it since it doesn't return with RTE on clone hardware */
    rte


/******************************************************************************/
/* BSS                                                                        */
/******************************************************************************/

        .bss


#endif /* MACHINE_RAVEN */
