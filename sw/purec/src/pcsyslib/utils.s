
                GLOBL   Supmain
                GLOBL   SupmainEx

                GLOBL   DisableInterrupts
                GLOBL   RestoreInterrupts
                GLOBL   SetIPL

                GLOBL   FlushCache
                GLOBL   FlushCache020
                GLOBL   FlushCache030
                GLOBL   FlushCache040
                GLOBL   FlushCache060

                TEXT

                bra.b   SupmainGo

; long SupmainEx(int args, char** argv, long(*func)(int, char**), void* stack)
SupmainEx:      movem.l d2-d4/a2-a5,-(sp)   * save regs
                move.l  32(sp),d3           * d3 = override stack
                bra.b   SupmainGo

; long Supmain(int args, char** argv, long(*func)(int, char**))
Supmain:        movem.l d2-d4/a2-a5,-(sp)   * save regs
                move.l  #0,d3               * d3 = default program stack
SupmainGo:      sub.l   d4,d4
                move.w  d0,d4               * d4 = args
                move.l  a0,a4               * a4 = argv
                move.l  a1,a6               * a6 = func

                move.l  #0,-(sp)            * oldssp = Super(0)
                move.w  #$20,-(sp)
                trap    #1
                addq.l  #6,sp

                cmp.l   #0,d3               * optional stack override
                bne.b   .1
                move.l  sp,d3
.1:             move.l  sp,a5               * a5 = saved sp
                subq.l  #4,d3
                and.w   #-16,d3
                move.l  d3,sp

                move.l  d0,d3               * d3 = oldssp returned by Super(0)

                move.l  d4,d0               * func(args, argv)
                move.l  a4,a0
                jsr     (a6)

                move.l  a5,sp               * restore sp to normal
                move.l  d3,-(sp)            * SuperToUser(oldssp)
                move.l  d0,d3               * d3 = return value from func()
                move.w  #$20,-(sp)
                trap    #1
                move.l  a5,sp

                move.l  d3,d0               * d0 = return value
                movem.l (sp)+,d2-d4/a2-a5   * restore regs
                rts

 ; uint16_t DisableInterrupts(void)
 DisableInterrupts:
    move.w  sr,d1
    move.w  d1,d0
    or.w    #$0700,d1
    move.w  d1,sr
    and.w   #$0700,d0
    rts

 ; void RestoreInterrupts(uint16_t)
RestoreInterrupts:
    move.w  sr,d1
    and.w   #$0700,d0
    and.w   #$f8ff,d1
    or.w    d0,d1
    move.w  d1,sr
    rts

 ; uint16_t SetIPL(uint16_t)
SetIPL:
    move.w  sr,d1       ; d1 = new ipl
    and.w   #$0700,d0
    and.w   #$f8ff,d1
    or.w    d0,d1
    move.w  sr,d0       ; d0 = old ipl
    and.w   #$0700,d0
    move.w  d1,sr       ; set ipl
    rts                 ; return old

; void FlushCache(long cpu)
FlushCache:
    cmp.w   #20,d0
    beq.b   FlushCache020
    cmp.w   #30,d0
    beq.b   FlushCache030
    cmp.w   #40,d0
    beq.b   FlushCache040
    cmp.w   #60,d0
    beq.b   FlushCache060
    rts

FlushCache020:
FlushCache030:
    nop
    .dc.l   0x4e7a0002  ; movec cacr,d0
    or.w    #0x0808,d0  ; instruction + data
    nop
    .dc.l   0x4e7b0002  ; movec d0,cacr
    nop
    rts

FlushCache040:
FlushCache060:
    nop
    .dc.w 0xf4f8        ; cpusha bc
    nop
    rts
