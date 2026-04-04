
    .globl isabios_disable_interrupts
    .globl isabios_restore_interrupts

    .globl isabios_getcacr20
    .globl isabios_setcacr20

    .globl isabios_getcacr40
    .globl isabios_setcacr40
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


isabios_getcacr20:
isabios_getcacr40:
    sub.l   d0,d0
    movec   cacr,d0
    rts

isabios_setcacr20:
    move.w  sr,-(sp)
    or.w    #$0700,sr
    nop
    movec   d0,cacr
    nop
    move.w  (sp)+,sr
    rts

isabios_setcacr40:
    move.w  sr,-(sp)
    or.w    #$0700,sr
    nop
    cpusha  bc
    movec   d0,cacr
    cpusha  bc
    nop
    move.w  (sp)+,sr
    rts
