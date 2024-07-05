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

    .global _raven_int_ikbd
    .global _raven_int_vbl
    .global _raven_int_50hz
    .extern ikbdraw
    .extern _int_vbl

/******************************************************************************/
/* Asm helpers                                                                */
/******************************************************************************/


/******************************************************************************/
/* Interrupts                                                                 */
/******************************************************************************/

.bss
raven_vblcnt:
        .ds.l   1

.text

_raven_int_ikbd:
    move.w  #0x2700,sr
    movem.l d0-d2/a0-a1,-(sp)
    move.w  20(sp),d0
    and.w   #0x0F00,d0                      /* get current ipl          */
    cmp.w   #0x0500,d0                      /* skip if >= 6             */
    bhi.b   1f
;    move.w  #0x2600,sr                      /* set level 6              */
    moveq.l #7,d2
0:  btst.b  #0,RAVEN_UART1_BASE+0x14
    beq.b   1f
    moveq   #0,d0
    move.b  RAVEN_UART1_BASE+0x00,d0
    jsr     ikbdraw
;    bra.b   0b
    dbra.w  d2,0b
1:  movem.l (sp)+,d0-d2/a0-a1
    rte

_raven_int_vbl:
    jsr     _int_vbl
    rte

_raven_int_50hz:
    move.w   #0x2700,sr
    movem.l d0-d1/a0-a1,-(sp)
    move.w  16(sp),d0
    and.w   #0x0F00,d0                      // get current ipl
    cmp.w   #0x0300,d0                      // skip if >= 4
    bhi.b   0f
;    move.w  #0x2400,d0                     // set level 4
;    jsr     _int_vbl                        // call tos vbl

	movem.l (sp)+,d0-d1/a0-a1
	move.l	0x70,-(sp)
    move.b  #0xDF,RAVEN_PADDR_MFP2+0x11      // clear timerC in-service
	rts

0:  movem.l (sp)+,d0-d1/a0-a1
    move.b  #0xDF,RAVEN_PADDR_MFP2+0x11      // clear timerC in-service
    rte


/******************************************************************************/
/* BSS                                                                        */
/******************************************************************************/

        .bss


#endif /* MACHINE_RAVEN */
