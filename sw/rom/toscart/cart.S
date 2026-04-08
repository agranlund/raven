//; ------------------------------------------------------------
//; Raven TOS extension cartridge
//; ------------------------------------------------------------

#define FLG_INIT0   (1<<24)
#define FLG_INIT1   (1<<25)
#define FLG_INIT2   (1<<26)
#define FLG_INIT3   (1<<27)
#define FLG_INIT4   (1<<28)
#define FLG_ACC     (1<<29)
#define FLG_TOS     (1<<30)
#define FLG_TTP     (1<<31)

//; ------------------------------------------------------------
//;
//; cartridge header
//;
//; ------------------------------------------------------------
    .section    header
    .text

.global start
start:

//; application cartridge magic
    dc.l    0xABCDEF42
    dc.l    hdr_init0               //; ca_next
    dc.l    0                       //; ca_init
    dc.l    0                       //; ca_run
    dc.w    0                       //; ca_time
    dc.w    0                       //; ca_date
    dc.l    0                       //; ca_size
    dc.l    0                       //; ca_name
    dc.l    0

//; raven special at cart offset 0x20
.global hdr_raven
hdr_raven:
    dc.l    0x5241564E              //; RAVN
    dc.l    VERSION                 //; yyyymmdd
    dc.l    run_initd
    dc.l    run_init0
    dc.l    run_init1
    dc.l    run_init2
    dc.l    run_init3
    dc.l    run_init4
    dc.l    run_inita
    dc.l    0
    dc.l    0
    dc.l    0
    dc.l    0
    dc.l    0
    dc.l    0
    dc.l    0

//; program headers at offset 0x60
.global hdr_init0
hdr_init0:
    dc.l    hdr_init1               //; ca_next
    dc.l    run_init0+FLG_INIT0     //; ca_init
    dc.l    0                       //; ca_run
    dc.w    0                       //; ca_time
    dc.w    0                       //; ca_date
    dc.l    0                       //; ca_size
    dc.l    0                       //; ca_name
    dc.l    0
    dc.l    0
.global hdr_init1
hdr_init1:
    dc.l    hdr_init2               //; ca_next
    dc.l    run_init1+FLG_INIT1     //; ca_init
    dc.l    0                       //; ca_run
    dc.w    0                       //; ca_time
    dc.w    0                       //; ca_date
    dc.l    0                       //; ca_size
    dc.l    0                       //; ca_name
    dc.l    0
    dc.l    0
.global hdr_init2
hdr_init2:
    dc.l    hdr_init3               //; ca_next
    dc.l    run_init2+FLG_INIT2     //; ca_init
    dc.l    0                       //; ca_run
    dc.w    0                       //; ca_time
    dc.w    0                       //; ca_date
    dc.l    0                       //; ca_size
    dc.l    0                       //; ca_name
    dc.l    0
    dc.l    0
.global hdr_init3
hdr_init3:
    dc.l    hdr_init4               //; ca_next
    dc.l    run_init3+FLG_INIT3     //; ca_init
    dc.l    0                       //; ca_run
    dc.w    0                       //; ca_time
    dc.w    0                       //; ca_date
    dc.l    0                       //; ca_size
    dc.l    0                       //; ca_name
    dc.l    0
    dc.l    0
.global hdr_init4
hdr_init4:
    dc.l    0                       //; ca_next
    dc.l    run_init4+FLG_INIT4     //; ca_init
    dc.l    0                       //; ca_run
    dc.w    0                       //; ca_time
    dc.w    0                       //; ca_date
    dc.l    0                       //; ca_size
    dc.l    0                       //; ca_name
    dc.l    0
    dc.l    0
.global hdr_last
hdr_last:
    dc.l    0                       //; ca_next
    dc.l    0                       //; ca_init
    dc.l    0                       //; ca_run
    dc.w    0                       //; ca_time
    dc.w    0                       //; ca_date
    dc.l    0                       //; ca_size
    dc.l    0                       //; ca_name
    dc.l    0
    dc.l    0


//; ------------------------------------------------------------
//; Initd, diagnostic cart
//; ------------------------------------------------------------
.global run_initd
run_initd:
    //; no atari hardware inited
    //; a6 = return pointer
    //; sp = garbage
    jmp     (a6)

//; ------------------------------------------------------------
//; Init2, before video init
//; ------------------------------------------------------------
.global run_init2
run_init2:
    rts

//; ------------------------------------------------------------
//; Init0, after hardware init
//; ------------------------------------------------------------
.global run_init0
run_init0:
    rts

//; ------------------------------------------------------------
//; Init1, after hardware init (interrupts on)
//; ------------------------------------------------------------
.global run_init1
run_init1:
    rts

//; ------------------------------------------------------------
//; Init3, before disk boot
//; ------------------------------------------------------------
.global run_init3
run_init3:
    //; install reset resident code.
    //; runs after disk boot and just before auto programs
    bsr rr_nuke
    bsr rr_install
    rts

//; ------------------------------------------------------------
//; Init4, not used by tos
//; ------------------------------------------------------------
.global run_init4
run_init4:
    rts

//; ------------------------------------------------------------
//; Inita, before auto folder programs
//; ------------------------------------------------------------


    .align 2
empty_string:       .asciz ""
    .align 2
rvbios_fname:       .asciz "RVBIOS.BIN"
    .align 2
rvbios_fname_old:   .asciz "C:\\AUTO\\RVBIOS.PRG"

    .align 4
.global run_inita
run_inita:
    movem.l d0-d7/a0-a6,-(sp)
    //; remove old rvbios.prg from auto folder
    move.l  #rvbios_fname_old,-(sp)
    move.w  #65,-(sp)
    trap    #1                      // Fdelete()
    addq.l  #6,sp
    //; try rvbios.bin on disk
    move.l  #0,-(sp)                //; env
    move.l  #empty_string,-(sp)     //; cmdline
    move.l  #rvbios_fname,-(sp)     //; filename
    move.w  #0,-(sp)                //; PE_LOADGO
    move.w  #75,-(sp)
    trap    #1                      //; Pexec()
    lea     16(sp),sp
    cmp.l   #0,d0
    beq.s   3f
    //; run rvbios.bin in rom
2:  movea.l #prg_rvbios,a0
    bsr     util_exec
3:  movem.l (sp)+,d0-d7/a0-a6
    rts


//; ------------------------------------------------------------
//;
//; utility functions
//;
//; ------------------------------------------------------------

.global util_flushcache
util_flushcache:
    nop
    cpusha bc
    nop
    rts

.global util_exec
util_exec:
    //; parse header
    movem.l d0-d7/a0-a6,-(sp)
    move.l  a0,a6                   //; a6 = prg buffer
    move.l  0x02(a6),d3             //; d3 = text size
    move.l  0x06(a6),d4             //; d4 = data size
    move.l  0x0a(a6),d5             //; d5 = bss size
    move.l  0x0e(a6),d6             //; d6 = sym size
    move.l  0x16(a6),d0             //; d0 = prg flags
    //; create basepage
    move.l  #0,-(sp)                //; env
    move.l  #empty_string,-(sp)     //; cmdline
    move.l  d0,-(sp)                //; prgflags
    move.w  #7,-(sp)                //; PE_BASEPAGEFLAGS
    move.w  #0x4b,-(sp)             //; Pexec()
    trap    #1
    lea     16(sp),sp
    tst.l   d0                      //; success?
    ble     8f
    move.l  d0,a1                   //; a1 = PD *pd
    //; copy program
    move.l  a1,a2
    add.l   #256,a2                 //; a2 = dst = pd + 256 (basepage)
    move.l  a6,a3
    add.l   #0x1c,a3                //; a3 = src = prg + 0x1c (header)
    move.l  d3,d0
    add.l   d4,d0                   //; d0 = text+data size
    bra.b   2f
1:  move.b  (a3)+,(a2)+
    subq.l  #1,d0
2:  bne     1b
    //; clear bss
    move.l  d5,d0
    bra.b   4f
3:  clr.b   (a2)+
    subq.l  #1,d0
4:  bne     3b
    //; setup basepage
    move.l  a1,d0
    add.l   #256,d0             //; d0 = pd + 256 = text base
    move.l  d0,0x08(a1)         //; pd->p_tbase
    move.l  d3,0x0c(a1)         //; pd->p_tlen
    add.l   d3,d0
    move.l  d0,0x10(a1)         //; pd->p_dbase
    move.l  d4,0x14(a1)         //; pd->p_dlen
    add.l   d4,d0
    move.l  d0,0x18(a1)         //; pd->p_bbase
    move.l  d5,0x1c(a1)         //; pd->p_blen    
    //; relocate
    tst.w   0x1a(a6)            //; relocation info?
    bne     7f
    move.l  a6,a3               //; a3 = relocation data
    add.l   #0x1c,a3            //; skip header
    add.l   d3,a3               //; skip text
    add.l   d4,a3               //; skip data
    add.l   d6,a3               //; skip symbol table
    move.l  0x08(a1),d1         //; d1 = dst text start
    move.l  d1,a2               //; a2 = dst pointer
    move.l  (a3)+,d0            //; first offset is a longword
    beq     7f
    add.l   d0,a2               //; adjust dst pointer
    add.l   d1,(a2)             //; relocate
5:  moveq.l #0,d0
    move.b  (a3)+,d0            //; get offset
    beq     7f                  //; 0 = done
    cmp.b   #1,d0               //; 1 = skip 254 bytes
    beq     6f
    add.l   d0,a2               //; adjust dst pointer
    add.l   d1,(a2)             //; relocate
    bra     5b
6:  add.l   #254,a2             //; adjust dst pointer
    bra     5b
7:  bsr     util_flushcache     //; flush caches
    //; run program
    move.l  #0,-(sp)            //; unused
    move.l  a1,-(sp)            //; basepage
    move.l  #0,-(sp)            //; unused
    move.w  #6,-(sp)            //; PE_GOTHENFREE
    move.w  #0x4b,-(sp)         //; Pexec()
    trap    #1
    lea     16(sp),sp
8:  movem.l (sp)+,d0-d7/a0-a6
    rts


//; ------------------------------------------------------------
//;
//; reset-resident, runs right before auto folder programs
//;
//; ------------------------------------------------------------
#define RR_MAGIC    0x12123456
#define RR_CHKSUM   0x5678
#define RR_LOMEM    0x400
#define RR_SIZE     0x200

//;     0 4     magic
//;     4 4     ptr to 0
//;     8 502   code
//;   510 2     checksum_fix

.global rr_location
rr_location:
    move.l  0x42e.w,a0          //; phystop
    sub.l   #RR_SIZE,a0
    rts

rr_generate_checksum:
    movem.l d0-d1/a0,-(sp)
    clr.l   d0
    move.w  #254,d1
1:  add.w   (a0)+,d0
    dbra    d1,1b
    move.w  #RR_CHKSUM,d1
    sub.w   d0,d1
    move.w  d1,(a0)
    movem.l (sp)+,d0-d1/a0
    rts

.global rr_install
rr_install:
    movem.l d0-d1/a0-a1,-(sp)
    bsr     rr_location             //; a0.l = dest
    move.l  a0,a1                   //; a1.l = dest
    clr.l   d0
    move.w  #127,d1
1:  move.l  d0,(a1)+
    dbra    d1,1b

    move.l  a0,a1                   //; prepare
    move.l  #RR_MAGIC,(a1)+
    move.l  a0,(a1)+
    move.w  #0x4EF9,(a1)+           //; jmp
    move.l  #run_inita,(a1)+        //; run_inita

    bsr     rr_generate_checksum
    bsr     util_flushcache
    movem.l (sp)+,d0-d1/a0-a1
    rts

.global rr_uninstall
rr_uninstall:
    move.l  a0,-(sp)
    bsr     rr_location
    cmp.l   #RR_MAGIC,(a0)
    bne.b   1f
    cmp.l   4(a0),a0
    bne.b   1f
    clr.l   (a0)
1:  move.l  (sp)+,a0
    rts

.global rr_nuke
rr_nuke:
    movem.l d0-d2/a0-a2,-(sp)
    move.l  0x42e.w,a0          //; a0 = phystop
1:  move.l  #RR_MAGIC,d0
    move.l  #RR_LOMEM,a1
    move.l  #RR_SIZE,a2
2:  sub.w   a2,a0
    cmp.l   a1,a0               //; end of memory?
    beq.b   4f
    cmp.l   (a0),d0             //; verify magic
    bne.b   2b
    cmp.l   4(a0),a0            //; verify ptr
    bne.b   2b
    clr.w   d0
    move.l  a0,a1               //; verify checksum
    move.w  #255,d1
3:  add.w   (a1)+,d0
    dbra    d1,3b
    cmp.w   #RR_CHKSUM,d0
    bne.b   1b
    clr.l   (a0)                //; clear magic
    bra.b   1b
4:  bsr     util_flushcache
    movem.l (sp)+,d0-d2/a0-a2
    rts



//; ------------------------------------------------------------
//;
//; programs
//;
//; ------------------------------------------------------------

    .align 4
.global prg_rvbios
prg_rvbios:
    .incbin "rvbios.sys"
    .align 4
prg_rvbios_end:

