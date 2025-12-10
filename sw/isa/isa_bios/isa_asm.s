
    .globl disable_interrupts
    .globl restore_interrupts
    .globl irq_lists

    .globl irqvec_isa02_mfp2B0
    .globl irqvec_isa03_mfp2B1
    .globl irqvec_isa04_mfp2B2
    .globl irqvec_isa05_mfp2B3
    .globl irqvec_isa07_mfp2B6
    .globl irqvec_isa10_mfp2B7
    .globl irqvec_isa11_mfp2A6
    .globl irqvec_isa14_mfp2A7

disable_interrupts:
    move.w  sr,d0
    or.w    #$0700,sr
    and.w   #$0700,d0
    rts

restore_interrupts:
    move.w  sr,d1
    and.w   #$0700,d0
    and.w   #$f0ff,d1
    or.w    d0,d1
    move.w  d1,sr
    rts


MACRO   mfpirq num,maddr,mdata
    local .1
    local .2
    local .3
    movem.l d0-d2/a0-a2,-(sp)
    move.l  #irq_lists+(256*num),a0
    move.l  (a0)+,d1            ; d1 is number of handlers
    subq.l  #1,d1
    bmi.b   .3
    move.l  #.3,-(sp)           ; last jump is end of interrupt
.1: move.l  (a0)+,-(sp)         ; push jump address
.2: dbf.w   d1,.1
    rts                         ; call all handlers
.3: movem.l (sp)+,d0-d2/a0-a2   ; restore and end interrupt
    move.l  #&mdata,maddr       ; clear in-service bit
    rte
ENDM

irqvec_isa02_mfp2B0: mfpirq 2,$A0000A11,$FE
irqvec_isa03_mfp2B1: mfpirq 3,$A0000A11,$FD
irqvec_isa04_mfp2B2: mfpirq 4,$A0000A11,$FB
irqvec_isa05_mfp2B3: mfpirq 5,$A0000A11,$F7
irqvec_isa07_mfp2B6: mfpirq 7,$A0000A11,$BF
irqvec_isa10_mfp2B7: mfpirq 10,$A0000A11,$7F
irqvec_isa11_mfp2A6: mfpirq 11,$A0000A0F,$BF
irqvec_isa14_mfp2A7: mfpirq 14,$A0000A0F,$7F
