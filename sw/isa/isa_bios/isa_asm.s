
    .globl disable_interrupts
    .globl restore_interrupts
    .globl irq_lists

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


;-----------------------------------------------------
;
;       Raven specific interrupt vectors
;
;-----------------------------------------------------


;-----------------------------------------------------
; generic amount of callbacks
;-----------------------------------------------------
MACRO   mfpirqx num,maddr,mdata
    local .1
    local .2
    local .3
;    move.w  #$2700,sr
    movem.l d0-d2/a0-a2,-(sp)
    move.l  #irq_lists+(0+(64*num)),a0
    move.l  (a0)+,d1            ; d1 is number of handlers
    subq.l  #1,d1
    bmi.b   .3
    move.l  #.3,-(sp)           ; last jump is end of interrupt
.1: move.l  (a0)+,-(sp)         ; push jump address
.2: dbf.w   d1,.1
    rts                         ; call all handlers
.3: movem.l (sp)+,d0-d2/a0-a2   ; restore and end interrupt
    move.b  #&mdata,maddr       ; clear in-service bit
    rte
ENDM

;-----------------------------------------------------
; no user callbacks
;-----------------------------------------------------
MACRO   mfpirq0 num,maddr,mdata
    move.b  #&mdata,maddr       ; clear in-service bit
    rte
ENDM

;-----------------------------------------------------
; one user callback
;-----------------------------------------------------
MACRO   mfpirq1 num,maddr,mdata
    local .3
;    move.w  #$2700,sr
    movem.l d0-d2/a0-a2,-(sp)
    move.l  #irq_lists+(4+(64*num)),a0
    move.l  #.3,-(sp)           ; push return address
    move.l  (a0),-(sp)          ; push first address
    rts
.3: movem.l (sp)+,d0-d2/a0-a2   ; restore and end interrupt
    move.b  #&mdata,maddr       ; clear in-service bit
    rte
ENDM

;-----------------------------------------------------
; two user callbacks
;-----------------------------------------------------
MACRO   mfpirq2 num,maddr,mdata
    local .3
;    move.w  #$2700,sr
    movem.l d0-d2/a0-a2,-(sp)
    move.l  #irq_lists+(4+(64*num)),a0
    move.l  #.3,-(sp)           ; push return address
    move.l  (a0)+,-(sp)         ; push second handler
    move.l  (a0),-(sp)          ; push first handler
    rts
.3: movem.l (sp)+,d0-d2/a0-a2   ; restore and end interrupt
    move.b  #&mdata,maddr       ; clear in-service bit
    rte
ENDM

;-----------------------------------------------------
; three user callbacks
;-----------------------------------------------------
MACRO   mfpirq3 num,maddr,mdata
    local .3
;    move.w  #$2700,sr
    movem.l d0-d2/a0-a2,-(sp)
    move.l  #irq_lists+(4+(64*num)),a0
    move.l  #.3,-(sp)           ; push return address
    movem.l (a0)+,d0-d2         ; push three handlers
    movem.l d0-d2,-(sp)
    rts
.3: movem.l (sp)+,d0-d2/a0-a2   ; restore and end interrupt
    move.b  #&mdata,maddr       ; clear in-service bit
    rte
ENDM

;-----------------------------------------------------
; four user callbacks
;-----------------------------------------------------
MACRO   mfpirq4 num,maddr,mdata
    local .3
;    move.w  #$2700,sr
    movem.l d0-d2/a0-a2,-(sp)
    move.l  #irq_lists+(4+(64*num)),a0
    move.l  #.3,-(sp)           ; push return address
    movem.l (a0)+,d0-d2/a1      ; push four handlers
    movem.l d0-d2/a1,-(sp)
    rts
.3: movem.l (sp)+,d0-d2/a0-a2   ; restore and end interrupt
    move.b  #&mdata,maddr       ; clear in-service bit
    rte
ENDM

;-----------------------------------------------------
; five user callbacks
;-----------------------------------------------------
MACRO   mfpirq5 num,maddr,mdata
    local .3
;    move.w  #$2700,sr
    movem.l d0-d2/a0-a2,-(sp)
    move.l  #irq_lists+(4+(64*num)),a0
    move.l  #.3,-(sp)           ; push return address
    movem.l (a0)+,d0-d2/a1-a2   ; push five handlers
    movem.l d0-d2/a1-a2,-(sp)
    rts
.3: movem.l (sp)+,d0-d2/a0-a2   ; restore and end interrupt
    move.b  #&mdata,maddr       ; clear in-service bit
    rte
ENDM



;-----------------------------------------------------
; implementations
;-----------------------------------------------------
    .globl irqvec_isa02_mfp2B0
    .globl irqvec_isa03_mfp2B1
    .globl irqvec_isa04_mfp2B2
    .globl irqvec_isa05_mfp2B3
    .globl irqvec_isa07_mfp2B6
    .globl irqvec_isa10_mfp2B7
    .globl irqvec_isa11_mfp2A6
    .globl irqvec_isa14_mfp2A7

    .globl irqvec_isa02_mfp2B0_0
    .globl irqvec_isa03_mfp2B1_0
    .globl irqvec_isa04_mfp2B2_0
    .globl irqvec_isa05_mfp2B3_0
    .globl irqvec_isa07_mfp2B6_0
    .globl irqvec_isa10_mfp2B7_0
    .globl irqvec_isa11_mfp2A6_0
    .globl irqvec_isa14_mfp2A7_0

    .globl irqvec_isa02_mfp2B0_1
    .globl irqvec_isa03_mfp2B1_1
    .globl irqvec_isa04_mfp2B2_1
    .globl irqvec_isa05_mfp2B3_1
    .globl irqvec_isa07_mfp2B6_1
    .globl irqvec_isa10_mfp2B7_1
    .globl irqvec_isa11_mfp2A6_1
    .globl irqvec_isa14_mfp2A7_1

    .globl irqvec_isa02_mfp2B0_2
    .globl irqvec_isa03_mfp2B1_2
    .globl irqvec_isa04_mfp2B2_2
    .globl irqvec_isa05_mfp2B3_2
    .globl irqvec_isa07_mfp2B6_2
    .globl irqvec_isa10_mfp2B7_2
    .globl irqvec_isa11_mfp2A6_2
    .globl irqvec_isa14_mfp2A7_2

    .globl irqvec_isa02_mfp2B0_3
    .globl irqvec_isa03_mfp2B1_3
    .globl irqvec_isa04_mfp2B2_3
    .globl irqvec_isa05_mfp2B3_3
    .globl irqvec_isa07_mfp2B6_3
    .globl irqvec_isa10_mfp2B7_3
    .globl irqvec_isa11_mfp2A6_3
    .globl irqvec_isa14_mfp2A7_3

    .globl irqvec_isa02_mfp2B0_4
    .globl irqvec_isa03_mfp2B1_4
    .globl irqvec_isa04_mfp2B2_4
    .globl irqvec_isa05_mfp2B3_4
    .globl irqvec_isa07_mfp2B6_4
    .globl irqvec_isa10_mfp2B7_4
    .globl irqvec_isa11_mfp2A6_4
    .globl irqvec_isa14_mfp2A7_4

    .globl irqvec_isa02_mfp2B0_5
    .globl irqvec_isa03_mfp2B1_5
    .globl irqvec_isa04_mfp2B2_5
    .globl irqvec_isa05_mfp2B3_5
    .globl irqvec_isa07_mfp2B6_5
    .globl irqvec_isa10_mfp2B7_5
    .globl irqvec_isa11_mfp2A6_5
    .globl irqvec_isa14_mfp2A7_5

irqvec_isa02_mfp2B0: mfpirqx 2,$A0000A11,$FE
irqvec_isa03_mfp2B1: mfpirqx 3,$A0000A11,$FD
irqvec_isa04_mfp2B2: mfpirqx 4,$A0000A11,$FB
irqvec_isa05_mfp2B3: mfpirqx 5,$A0000A11,$F7
irqvec_isa07_mfp2B6: mfpirqx 7,$A0000A11,$BF
irqvec_isa10_mfp2B7: mfpirqx 10,$A0000A11,$7F
irqvec_isa11_mfp2A6: mfpirqx 11,$A0000A0F,$BF
irqvec_isa14_mfp2A7: mfpirqx 14,$A0000A0F,$7F

irqvec_isa02_mfp2B0_0: mfpirq0 2,$A0000A11,$FE
irqvec_isa03_mfp2B1_0: mfpirq0 3,$A0000A11,$FD
irqvec_isa04_mfp2B2_0: mfpirq0 4,$A0000A11,$FB
irqvec_isa05_mfp2B3_0: mfpirq0 5,$A0000A11,$F7
irqvec_isa07_mfp2B6_0: mfpirq0 7,$A0000A11,$BF
irqvec_isa10_mfp2B7_0: mfpirq0 10,$A0000A11,$7F
irqvec_isa11_mfp2A6_0: mfpirq0 11,$A0000A0F,$BF
irqvec_isa14_mfp2A7_0: mfpirq0 14,$A0000A0F,$7F

irqvec_isa02_mfp2B0_1: mfpirq1 2,$A0000A11,$FE
irqvec_isa03_mfp2B1_1: mfpirq1 3,$A0000A11,$FD
irqvec_isa04_mfp2B2_1: mfpirq1 4,$A0000A11,$FB
irqvec_isa05_mfp2B3_1: mfpirq1 5,$A0000A11,$F7
irqvec_isa07_mfp2B6_1: mfpirq1 7,$A0000A11,$BF
irqvec_isa10_mfp2B7_1: mfpirq1 10,$A0000A11,$7F
irqvec_isa11_mfp2A6_1: mfpirq1 11,$A0000A0F,$BF
irqvec_isa14_mfp2A7_1: mfpirq1 14,$A0000A0F,$7F

irqvec_isa02_mfp2B0_2: mfpirq2 2,$A0000A11,$FE
irqvec_isa03_mfp2B1_2: mfpirq2 3,$A0000A11,$FD
irqvec_isa04_mfp2B2_2: mfpirq2 4,$A0000A11,$FB
irqvec_isa05_mfp2B3_2: mfpirq2 5,$A0000A11,$F7
irqvec_isa07_mfp2B6_2: mfpirq2 7,$A0000A11,$BF
irqvec_isa10_mfp2B7_2: mfpirq2 10,$A0000A11,$7F
irqvec_isa11_mfp2A6_2: mfpirq2 11,$A0000A0F,$BF
irqvec_isa14_mfp2A7_2: mfpirq2 14,$A0000A0F,$7F

irqvec_isa02_mfp2B0_3: mfpirq3 2,$A0000A11,$FE
irqvec_isa03_mfp2B1_3: mfpirq3 3,$A0000A11,$FD
irqvec_isa04_mfp2B2_3: mfpirq3 4,$A0000A11,$FB
irqvec_isa05_mfp2B3_3: mfpirq3 5,$A0000A11,$F7
irqvec_isa07_mfp2B6_3: mfpirq3 7,$A0000A11,$BF
irqvec_isa10_mfp2B7_3: mfpirq3 10,$A0000A11,$7F
irqvec_isa11_mfp2A6_3: mfpirq3 11,$A0000A0F,$BF
irqvec_isa14_mfp2A7_3: mfpirq3 14,$A0000A0F,$7F

irqvec_isa02_mfp2B0_4: mfpirq4 2,$A0000A11,$FE
irqvec_isa03_mfp2B1_4: mfpirq4 3,$A0000A11,$FD
irqvec_isa04_mfp2B2_4: mfpirq4 4,$A0000A11,$FB
irqvec_isa05_mfp2B3_4: mfpirq4 5,$A0000A11,$F7
irqvec_isa07_mfp2B6_4: mfpirq4 7,$A0000A11,$BF
irqvec_isa10_mfp2B7_4: mfpirq4 10,$A0000A11,$7F
irqvec_isa11_mfp2A6_4: mfpirq4 11,$A0000A0F,$BF
irqvec_isa14_mfp2A7_4: mfpirq4 14,$A0000A0F,$7F

irqvec_isa02_mfp2B0_5: mfpirq5 2,$A0000A11,$FE
irqvec_isa03_mfp2B1_5: mfpirq5 3,$A0000A11,$FD
irqvec_isa04_mfp2B2_5: mfpirq5 4,$A0000A11,$FB
irqvec_isa05_mfp2B3_5: mfpirq5 5,$A0000A11,$F7
irqvec_isa07_mfp2B6_5: mfpirq5 7,$A0000A11,$BF
irqvec_isa10_mfp2B7_5: mfpirq5 10,$A0000A11,$7F
irqvec_isa11_mfp2A6_5: mfpirq5 11,$A0000A0F,$BF
irqvec_isa14_mfp2A7_5: mfpirq5 14,$A0000A0F,$7F
