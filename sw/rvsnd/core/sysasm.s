;-------------------------------------------------------------------------------
; rvsnd
; (c)2025 Anders Granlund
;-------------------------------------------------------------------------------
; This file is free software  you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation  either version 2, or (at your option)
; any later version.
;
; This file is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY  without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program  if not, write to the Free Software
; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;-------------------------------------------------------------------------------
    
; todo: make compatible with <68020 cpus
; todo: get rid of unnecessary register save/restores
    
    .text
    
    .export sys_InstallBios
    .export sys_InstallMidiBios
    .export sys_InstallSoundBios

    .export cur_bconstat3
    .export cur_bconin3
    .export cur_bcostat3
    .export cur_bconout3

    .export tos_bconstat3
    .export tos_bconin3
    .export tos_bcostat3
    .export tos_bconout3

    .xref xbios_midi_bconstat
    .xref xbios_midi_bconin
    .xref xbios_midi_bcostat
    .xref xbios_midi_bconout

    .xref xb_locksnd
    .xref xb_unlocksnd
    .xref xb_soundcmd
    .xref xb_setbuffer
    .xref xb_setmode
    .xref xb_settracks
    .xref xb_setmontracks
    .xref xb_setinterrupt
    .xref xb_buffoper
    .xref xb_dsptristate
    .xref xb_gpio
    .xref xb_devconnect
    .xref xb_sndstatus
    .xref xb_buffptr
    .xref xb_rvsnd

    .export sys_ei
    .export sys_di
    .export sys_getpid
    .export sys_icache_clear20
    .export sys_icache_clear40
    .export sys_icache_enable20
    .export sys_icache_enable40
    .export sys_icache_restore20
    .export sys_icache_restore40

;----------------------------------------------------------
; utility functions
;----------------------------------------------------------

 ; uint16_t sys_di(void)
 sys_di:
    move.w  sr,d0
    or.w    #$0700,sr
    and.w   #$0700,d0
    rts

 ; void sys_ei(uint16_t)
sys_ei:
    move.w  sr,d1
    and.w   #$0700,d0
    and.w   #$f8ff,d1
    or.w    d0,d1
    move.w  d1,sr
    rts

 ; uint16_t sys_set_ipl(uint16_t)
sys_set_ipl:
    move.w  sr,d1       ; d1 = new ipl
    and.w   #$0700,d0
    and.w   #$f8ff,d1
    or.w    d0,d1
    move.w  sr,d0       ; d0 = old ipl
    and.w   #$0700,d0
    move.w  d1,sr       ; set ipl
    rts                 ; return old

sys_getpid:
    move.w  #268,-(sp)   ; Offset 0
    trap    #1           ; GEMDOS
    addq.l  #2,sp        ; Correct stack
    rts

sys_icache_clear20:
    move.w  sr,d1
    or.w    #$2700,sr
    movec   cacr,d0
    or.l    #$00000008,d0   ; clear instr cache
    nop
    movec   d0,cacr
    nop
    move.w  d1,sr
    rts
sys_icache_enable20:
    move.w  sr,-(sp)
    or.w    #$2700,sr
    movec   cacr,d0
    move.l  d0,d1
    or.l    #$00000009,d1   ; clear + enable instr cache
    nop
    movec   d1,cacr
    nop
    move.w  (sp)+,sr
    rts
sys_icache_restore20:
    move.w  sr,-(sp)
    or.w    #$2700,sr
    movec   cacr,d1
    and.l   #$fffffffe,d1   ; mask instr cache
    and.l   #$00000001,d0
    or.l    #$00000008,d0   ; clear instr cache
    or.l    d0,d1
    nop
    movec   d1,cacr
    nop
    move.w  (sp)+,sr
    rts

sys_icache_clear40:
    move.w  sr,d1
    or.w    #$2700,sr
    nop
    cinva   ic              ; clear instr cache
    nop
    move.w  d1,sr
    rts
sys_icache_enable40:
    move.w  sr,-(sp)
    or.w    #$2700,sr
    movec   cacr,d0
    move.l  d0,d1
    or.l    #$00008000,d1   ; enable instr cache
    nop
    cinva   ic              ; clear instr cache
    nop
    movec   d1,cacr         ; apply
    nop
    move.w  (sp)+,sr
    rts
sys_icache_restore40:
    move.w  sr,-(sp)
    or.w    #$2700,sr
    movec   cacr,d1
    and.l   #$ffff7fff,d1    ; mask instr cache
    and.l   #$00008000,d0
    or.l    d0,d1
    nop
    movec   d1,cacr
    nop
    cinva   ic
    nop
    move.w  (sp)+,sr
    rts

;----------------------------------------------------------
; int32_t bconstat3()
; return -1 = character waiting, 0 = nothing available
;----------------------------------------------------------
tos_bconstat3:
    dc.l 0x00000000
new_bconstat3:
    movem.l d2/a2,-(sp)
    move.l  cur_bconstat3,a0
    jsr     (a0)
    movem.l (sp)+,d2/a2
    rts
cur_bconstat3:
    dc.l 0x00000000

;----------------------------------------------------------
; int32_t bconin3()
;----------------------------------------------------------
tos_bconin3:
    dc.l    0x00000000
new_bconin3:
    movem.l d2/a2,-(sp)
    move.l  cur_bconstat3,a0
.1: jsr     (a0)
    beq.b   .1
    move.l  cur_bconin3,a0
    jsr     (a0)
    movem.l (sp)+,d2/a2
    rts
cur_bconin3:
    dc.l    0x00000000

;----------------------------------------------------------
; int32_t bcostat3()
; return -1 = ok to send, 0 = not ready
;----------------------------------------------------------
tos_bcostat3:
    dc.l 0x00000000
new_bcostat3:
    movem.l d2/a2,-(sp)
    move.l  cur_bcostat3,a0
    jsr     (a0)
    movem.l (sp)+,d2/a2
    rts
cur_bcostat3:
    dc.l 0x00000000

;----------------------------------------------------------
; void bconout3(int16_t dev, int16_t chr)
; returns aaaaaabb where a is timestamp and b is data
; dev = 4(a7)
; chr = 6(a7)
;----------------------------------------------------------
tos_bconout3:
    dc.l 0x00000000
new_bconout3:
    move.l  4(sp),d0
    movem.l d0/d2/a2,-(sp)
    move.l  cur_bcostat3,a0
.1: jsr     (a0)
    beq.b   .1
    move.l  cur_bconout3,a0
    jsr     (a0)
    movem.l (sp)+,d0/d2/a2
    rts
cur_bconout3:
    dc.l 0x00000000

;----------------------------------------------------------
; Xbios, opcode $0C
; void Midiws(int16_t cnt, uint8_t* ptr)
;   cnt.w = 0(a0)
;   ptr.l = 2(a0)
;----------------------------------------------------------
xb_midiws:
    movem.l d2-d4/a2-a5,-(sp)
    move.w  0(a0),d3            ; d3 = cnt
    movea.l 2(a0),a3            ; a3 = ptr
    move.l  #$00030000,d4       ; d4 = data
    move.l  cur_bcostat3,a4     ; a4 = bcostat
    move.l  cur_bconout3,a5     ; a5 = bconout
    bra.b   .2
.1: jsr     (a4)                ; wait for bcostat() != 0
    tst.b   d0
    beq.b   .1
    move.b  (a3)+,d4            ; d4 = *ptr++
    move.l  d4,-(sp)
    jsr     (a5)                ; bconout(3, d4)
    addq.l  #4,sp
.2: dbf.w   d3,.1
    movem.l (sp)+,d2-d4/a2-a5
    rte

;----------------------------------------------------------
; Xbios, opcode $8F
; int32_t rvsnd(int16_t opcode, ...)
;----------------------------------------------------------
;xb_rvsnd:
;    addq.l  #4,a0       ; a0 = args
;    move.w  (a0)+,d0    ; d0 = op
;    move.l  #666,d0
;    addq.l  #4,sp
;    rte


;----------------------------------------------------------
; Xbios, default handler, from function call
;----------------------------------------------------------
xb_fcall_old:
    move.l  old_xbios,(sp)      ; replace return address
    rts                         ; and jump


;----------------------------------------------------------
;
; Trap14 xbios handler
;   d0-d2/a0-a2 are allowed to be destroyed
;
;----------------------------------------------------------
	dc.b "XBRA"
	dc.b "RSND"
old_xbios:
	dc.l 0x00B8
new_xbios:
	move	usp,a0
	btst	#5,(sp)			; already super?
	beq.b	.1
    lea     8(sp),a0        ; assume longframes
.1: move.w  (a0)+,d0
    cmp.w   #12,d0          ; midiws
    beq.s   xb_midiws
    sub.w   #128,d0         ; sound
    bmi.b   .3
    cmp.w   #16,d0
    bge.b   .3
    jsr     ([xbfunc_sound,d0*4])
    rte
.3: jmp     ([old_xbios])

xbfunc_sound:
    ds.l 16 ; trap14, opcode 128-143

;----------------------------------------------------------
; install xbios handler
;----------------------------------------------------------
sys_InstallBios:
    ; disable interrupts
    move.w  sr,-(sp)
    move.w  #0x2700,sr

    ; xbios trap14 handler
    move.l  0x00b8.w,d0
    move.l  d0,old_xbios
    move.l  #new_xbios, 0x00b8.w

    ; midi gemdos hooks
    move.l  0x52a.w, tos_bconstat3
    move.l  0x52a.w, cur_bconstat3
    move.l  #new_bconstat3, 0x52a.w
    move.l  0x54a.w, tos_bconin3
    move.l  0x54a.w, cur_bconin3
    move.l  #new_bconin3,   0x54a.w
    move.l  0x56e.w, tos_bcostat3
    move.l  0x56e.w, cur_bcostat3
    move.l  #new_bcostat3,  0x56e.w
    move.l  0x58a.w, tos_bconout3
    move.l  0x58a.w, cur_bconout3
    move.l  #new_bconout3,  0x58a.w

    ; sound xbios
    move.l  #xbfunc_sound,a0
    move.l  #xb_locksnd,(a0)+       ; $80
    move.l  #xb_unlocksnd,(a0)+     ; $81
    move.l  #xb_soundcmd,(a0)+      ; $82
    move.l  #xb_setbuffer,(a0)+     ; $83
    move.l  #xb_setmode,(a0)+       ; $84
    move.l  #xb_settracks,(a0)+     ; $85
    move.l  #xb_setmontracks,(a0)+  ; $86
    move.l  #xb_setinterrupt,(a0)+  ; $87
    move.l  #xb_buffoper,(a0)+      ; $88
    move.l  #xb_dsptristate,(a0)+   ; $89
    move.l  #xb_gpio,(a0)+          ; $8A
    move.l  #xb_devconnect,(a0)+    ; $8B
    move.l  #xb_sndstatus,(a0)+     ; $8C
    move.l  #xb_buffptr,(a0)+       ; $8D
    move.l  #xb_fcall_old,(a0)+     ; $8E (unused)

    ; rvsnd xbios
    move.l  #xb_fcall_old,(a0)+     ; $8F (unused)
    ;move.l  #xb_rvsnd,(a0)+         ; $8F (rvsnd)

    ; restore interrupts
    move.w  (sp)+,sr
    rts

