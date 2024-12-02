/*
 * Copyright (c) 2022-2024 Anders Granlund
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//#define FPESTART    move.w    #0x2700,sr
#define FPESTART

    .global _fpu_emulate

    .global _fpe_vec_68000
    .global _fpe_vec_68010
    .global _fpe_vec_68020
    .global _fpe_vec_68030
    .global _fpe_vec_68040
    .global _fpe_vec_68060

    .global _fpe_vec_68000_tos1
    .global _fpe_vec_68010_tos1
    .global _fpe_vec_68020_tos1
    .global _fpe_vec_68030_tos1
    .global _fpe_tos1bot
    .global _fpe_tos1top

    .global _fpe_vec
    .global _fpe_vec_sf         // 68000       : 3 word frame
    .global _fpe_vec_0f         // 68010-68030 : 4 word frame $0
    .global _fpe_vec_4f         // 68040-68060 : 8 word frame $4

    .global _fpe_SetIPL
    .global _fpe_SetVBR_010_060
    .global _fpe_GetVBR_010_060
    .global _fpe_GetPCR_060
    .global _fpe_SetPCR_060
    .global _fpe_FlushCache_020_030
    .global _fpe_FlushCache_040_060
    .global _fpe_DetectFpu_020_060
    .global _fpe_DisableFpu_060

    .bss
    .even

_fpe_tos1bot:   ds.l 1
_fpe_tos1top:   ds.l 1
stackbot:       ds.l 128*4
stacktop:

    .text
    .even


// todo: fixme when this is in ROM
_fpe_vec:       .long 0
_fpe_xbra:      .long 0
_fpe_xbra_id:   .long 0
_fpe_xbra_old:  .long 0
    move.l  _fpe_vec,-(sp)
    rts



//------------------------------------------------------
//
// helpers
//
//------------------------------------------------------

_fpe_SetIPL:                    // uint16_t fpe_SetIPL(uint16_t ipl)
    move.l  4(sp),d1
    and.w   #0x0F00,d1
    move.w  sr,d0
    and.w   #0xF00,d0
    move.w  d0,-(sp)
    move.w  sr,d0
    and.w   #0xF0FF,d0
    or.w    d1,d0
    move.w  d0,sr
    move.w  (sp)+,d0
    rts

_fpe_GetVBR_010_060:            // uint16_t fpe_GetVBR_010_060(void)
    nop
    .dc.l   0x4e7a0801              // movec vbr,d0
    rts

_fpe_SetVBR_010_060:            // uint16_t fpe_GetVBR_010_060(uint16_t vbr)
    nop
    .dc.l 0x4e7a0801                // movec vbr,d0
    move.l    4(sp),d1
    nop
    .dc.l   0x4e7b1801              // movec d1,vbr
    nop
    rts

_fpe_GetPCR_060:                // uint32_t fpe_GetPCR_060(void)
    nop
    .dc.l   0x4e7a0808              // movec pcr,d0
    rts

_fpe_SetPCR_060:                // uint32_t fpe_SetPCR_060(uint32_t pcr)
    nop
    .dc.l   0x4e7a0808              // movec pcr,d0
    move.l  4(sp),d1
    nop
    .dc.l   0x4e7b1808              // movec d1,pcr
    nop
    rts

_fpe_FlushCache_020_030:        // void fpe_FlushCache_020_030(void)
    nop
    .dc.l   0x4e7a0002              // movec cacr,d0
    ori.w   #0x0808,d0              // flush
    nop
    .dc.l   0x4e7b0002              // movec d0,cacr
    nop
    rts

_fpe_FlushCache_040_060:        // void fpe_FlushCache_040_060(void)
    nop
    .dc.w   0xf4f8                  // cpusha bc
    nop
    rts

_fpe_DetectFpu_020_060:         // uint32_t fpe_DetectFpu_020_060(void)
    move.l  a2,-(sp)
    move.l  sp,a0
    move.l  0x08.w,a2
    move.l  0x2c.w,a1
    move.l  #2f,d1
    move.l  d1,0x2c.w
    move.l  d1,0x08.w
    nop
    moveq.l #0,d0
    move.l  d0,-(sp)
    move.l  d0,-(sp)
    move.l  d0,-(sp)
    .dc.w   0xf35f                  // frestore (sp)+
    .dc.l   0xf2800000              // fnop
    .dc.w   0xf35f                  // fsave -(sp)
    moveq.l #1,d0
2:  move.l  a2,0x08.w
    move.l  a1,0x2c.w
    move.l  a0,sp
    nop
    move.l  (sp)+,a2
    rts

_fpe_DisableFpu_060:            // void fpe_DisableFpu_060(void)
    .dc.l   0x4e7a0808              // movec pcr,d0
    // todo: check if already disabled
    nop
    .dc.l   0xf2800000              // fnop
    or.l    #2,d0
    nop
    .dc.l   0x4e7b0808              // movec d0,pcr
    nop
    rts
   


//------------------------------------------------------
//
// lineF vector - 68040/60
//
//  0x2C
//      format 0: unimplemented fline
//      format 2: unimplemented fpu instruction
//      format 4: fpu disabled / unavailable
//
//------------------------------------------------------

//------------------------------------------------------
// TOS2+
//------------------------------------------------------
_fpe_vec_68040:
_fpe_vec_68060:
    FPESTART
    btst.b  #6,6(sp)                // format4?
    bne.b   fpe_vec_4f
    //bra.s fpe_vec_0f
    move.l  _fpe_xbra_old,-(sp)
    rts
fpe_vec_4f:
    btst    #5,(sp)                 // super?
    beq.s   fpe_vec_4f_user

    //---------------------------------------
    // Supervisor
    //---------------------------------------
fpe_vec_4f_super:
    move.l  a5,-(sp)                // save a5 to real stack
    move.l  a6,-(sp)                // save a6 to real stack
    move.l  sp,a5
    add.l   #8+16,a5                // a5 = exception frame
    move.l  #stacktop,a6            // a6 = emulator stack
    move.w  -(a5),-(a6)             // format
    move.l  -(a5),-(a6)             // pc
    move.w  -(a5),-(a6)             // sr
    move.l  -(a5),-(a6)             // addr
    move.l  -(a5),-(a6)             // fslw
    clrl    -(a6)                   // stackadj
    movem.l #0xFFFF,-(a6)           // registers
    move.l  (sp)+,56(a6)            // move saved a6 to emulator stack
    move.l  (sp)+,52(a6)            // move saved a5 to emulator stack
    add.l   #16,sp                  // discard stack frame
    move.l  sp,60(a6)               // save sp from before exception to emulator stack frame
    move.l  a6,sp                   // emulator stack is now active

    move.l  a6,-(sp)
    jbsr    _fpu_emulate            // fpu_emulate((frame*)sp)
    lea     4(sp),sp

    move.l  sp,a6
    move.l  60(a6),sp               // real stack is now active
    move.l  #stacktop,a5                    
    move.w  -(a5),-(sp)             // push format
    move.l  -(a5),-(sp)             // push pc
    move.w  -(a5),-(sp)             // push sr
    move.l  -(a5),-(sp)             // push addr
    move.l  -(a5),-(sp)             // push fslw
    cmp.l   #0,d0
    bne.s   2f
1:  movem.l (a6),#0x7FFF            // restore most regs
    rte        
2:  movem.l (a6),#0x7FFF            // restore most regs
    move.l  _fpe_xbra_old,-(sp)     // push previous vector
    rts                             // and jump

    //---------------------------------------
    // Usermode
    //---------------------------------------
fpe_vec_4f_user:
    clrl    -(sp)                   // stack adjust
    movem.l #0xFFFF,-(sp)           // save all regs
    move.l  usp,a0                  // get usp
    move.l  a0,60(sp)               // save to frame

    move.l  sp,-(sp)
    jbsr    _fpu_emulate            // fpu_emulate((frame*)sp)
    lea     4(sp),sp                // restore stack

    move.l  60(sp),a0               // get usp from frame
    move.l  a0,usp                  // restore it
    cmp.l   #0,d0
    bne.s   2f
1:  movem.l (sp)+,#0x7FFF           // restore most regs
    addq.l  #8,sp                   // pop SP and stack adjust
    rte
2:  movem.l (sp)+,#0x7FFF           // restore most regs
    addq.l  #8,sp                   // pop SP and stack adjust
    move.l  _fpe_xbra_old,(sp)      // push old vector
    rte                             // and jump



//------------------------------------------------------
//
// lineF vector - 68010/20/30 (long stack frames)
//
//------------------------------------------------------

//------------------------------------------------------
// TOS1
//------------------------------------------------------
_fpe_vec_68010_tos1:
_fpe_vec_68020_tos1:
_fpe_vec_68030_tos1:
    FPESTART
    move.l  d0,-(sp)                // save regs
    move.l  6(sp),d0                // get caller address
    cmp.l   _fpe_tos1bot,d0         // from before tos rom?
    blt.s   1f
    cmp.l   _fpe_tos1top,d0         // from after tos rom?
    bge.s   1f
    move.l  (sp)+,d0
    move.l  _fpe_xbra_old,-(sp)     // push previous vector
    rts                             // and jump
1:  move.l  (sp)+,d0
//  bra.b   _fpe_vec_0f

//------------------------------------------------------
// TOS2+
//------------------------------------------------------
_fpe_vec_68010:
_fpe_vec_68020:
_fpe_vec_68030:
    FPESTART
_fpe_vec_0f:
    btst    #5,(sp)
    beq.s   fpe_vec_0f_user
    
    //---------------------------------------
    // Supervisor
    //---------------------------------------
fpe_vec_0f_super:
    move.l  a5,-(sp)                // save a5 to real stack
    move.l  a6,-(sp)                // save a6 to real stack
    move.l  sp,a5
    add.l   #16,a5                  // a5 = exception frame
    move.l  #stacktop,a6            // a6 = emulator stack
    move.w  -(a5),-(a6)             // format
    move.l  -(a5),-(a6)             // pc
    move.w  -(a5),-(a6)             // sr
    clrl    -(a6)                   // stackadj
    movem.l #0xFFFF,-(a6)           // registers
    move.l  (sp)+,56(a6)            // move saved a6 to emulator stack
    move.l  (sp)+,52(a6)            // move saved a5 to emulator stack
    addq.l  #8,sp                   // discard stack frame
    move.l  sp,60(a6)               // save sp from before exception to emulator stack frame
    move.l  a6,sp                   // emulator stack is now active

    move.l  a6,-(sp)
    jbsr    _fpu_emulate            // fpu_emulate((frame*)sp)
    lea     4(sp),sp

    move.l  sp,a6
    move.l  60(a6),sp               // real stack is now active
    move.l  #stacktop,a5                    
    move.w  -(a5),-(sp)             // push format
    move.l  -(a5),-(sp)             // push pc
    move.w  -(a5),-(sp)             // push sr
    cmp.l   #0,d0
    bne.s   2f
1:  movem.l (a6),#0x7FFF            // restore most regs
    rte
2:  movem.l (a6),#0x7FFF            // restore most regs
    move.l  _fpe_xbra_old,-(sp)     // push previous vector
    rts                             // and jump

    //---------------------------------------
    // Usermode
    //---------------------------------------
fpe_vec_0f_user:
    clrl    -(sp)                   // stack adjust
    movem.l #0xFFFF,-(sp)           // save all regs
    move.l  usp,a0                  // get usp
    move.l  a0,60(sp)               // save to frame

    move.l  sp,-(sp)
    jbsr    _fpu_emulate            // fpu_emulate((frame*)sp)
    lea     4(sp),sp                // restore stack

    move.l  60(sp),a0               // get usp from frame
    move.l  a0,usp                  // restore it
    cmp.l   #0,d0
    bne.s   2f
1:  movem.l (sp)+,#0x7FFF           // restore most regs
    addq.l  #8,sp                   // pop SP and stack adjust
    rte
2:  movem.l (sp)+,#0x7FFF           // restore most regs
    addq.l  #8,sp                   // pop SP and stack adjust
    move.l  _fpe_xbra_old,(sp)      // push old vector
    rte



//------------------------------------------------------
//
// lineF vector - 68000 (short stack frames)
//
//------------------------------------------------------ 

//------------------------------------------------------
// TOS1
//------------------------------------------------------
_fpe_vec_68000_tos1:
    FPESTART
    move.l  d0,-(sp)                // save regs
    move.l  6(sp),d0                // get caller address
    cmp.l   _fpe_tos1bot,d0         // from before tos rom?
    blt.b   1f
    cmp.l   _fpe_tos1top,d0         // from after tos rom?
    bge.b   1f
    move.l  (sp)+,d0                // restore regs
    move.l  _fpe_xbra_old,-(sp)     // store previous vector
    rts                             // and jump
1:  move.l  (sp)+,d0
//  bra.b   _fpe_vec_sf

//------------------------------------------------------
// TOS2+
//------------------------------------------------------
_fpe_vec_68000:
    FPESTART
_fpe_vec_sf:
    btst    #5,(sp)
    beq.s   fpe_vec_sf_user

    //---------------------------------------
    // Supervisor
    //---------------------------------------
fpe_vec_sf_super:
    move.l  a5,-(sp)                // save a5 to real stack
    move.l  a6,-(sp)                // save a6 to real stack
    move.l  sp,a5
    add.l   #14,a5                  // a5 = exception frame
    move.l  #stacktop,a6            // a6 = emulator stack
    move.w  #0,-(a6)                // fake format
    move.l  -(a5),-(a6)             // pc
    move.w  -(a5),-(a6)             // sr
    clrl    -(a6)                   // stackadj
    movem.l #0xFFFF,-(a6)           // registers
    move.l  (sp)+,56(a6)            // move saved a6 to emulator stack
    move.l  (sp)+,52(a6)            // move saved a5 to emulator stack
    addq.l  #6,sp                   // discard stack frame
    move.l  sp,60(a6)               // save sp from before exception to emulator stack frame
    move.l  a6,sp                   // emulator stack is now active

    move.l  sp,-(sp)
    jbsr    _fpu_emulate            // fpu_emulate((frame*)sp)
    lea     4(sp),sp

    move.l  sp,a6
    move.l  60(a6),sp               // real stack is now active
    move.l  #stacktop-2,a5                    
    move.l  -(a5),-(sp)             // push pc
    move.w  -(a5),-(sp)             // push sr
    cmp.l   #0,d0                   // failed?
    bne.b   2f
1:  movem.l (a6),#0x7FFF            // restore most regs
    rte                                
2:  movem.l (a6),#0x7FFF            // restore most regs
    move.l  _fpe_xbra_old,-(sp)     // push old vector
    rts                             // and jump

    //---------------------------------------
    // Usermode
    //---------------------------------------
fpe_vec_sf_user:
    subq.l  #8,sp
    move.w  #0,6(sp)                // fake format
    move.l  10(sp),2(sp)            // pc
    move.w  8(sp),(sp)              // sr
    clrl    -(sp)                   // stack adjust
    movem.l #0xFFFF,-(sp)           // save all regs
    move.l  usp,a0                  // get usp
    move.l  a0,60(sp)               // save to frame

    move.l  sp,-(sp)
    jbsr    _fpu_emulate            // fpu_emulate((frame*)sp)
    lea     4(sp),sp                // restore stack

    move.l  60(sp),a0               // get usp from frame
    move.l  a0,usp                  // restore it
    cmp.l   #0,d0                   // failed?
    bne.b   2f
1:  movem.l (sp)+,#0x7FFF           // restore most regs
    addq.l  #8,sp                   // pop SP and stack adjust
    move.w  (sp),8(sp)              // restore sr
    move.l  2(sp),10(sp)            // restore pc
    addq.l  #8,sp                   // get rid of fake frame
    rte
2:  movem.l (sp)+,#0x7FFF           // restore most regs
    addq.l  #8,sp                   // pop SP and stack adjust
    move.w  (sp),8(sp)              // restore sr
    move.l  2(sp),10(sp)            // restore pc
    addq.l  #8,sp                   // get rid of fake frame
    move.l  _fpe_xbra_old,-(sp)     // push old vector
    rts                             // and jump

