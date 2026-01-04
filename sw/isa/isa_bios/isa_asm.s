
    .globl isabios_disable_interrupts
    .globl isabios_restore_interrupts
    .globl irq_data

isabios_disable_interrupts:
    move.w  sr,d0
    or.w    #$0700,sr
    and.w   #$0700,d0
    rts

isabios_restore_interrupts:
    move.w  sr,d1
    and.w   #$0700,d0
    and.w   #$f0ff,d1
    or.w    d0,d1
    move.w  d1,sr
    rts
