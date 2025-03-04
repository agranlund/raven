;-------------------------------------------------------------------------------
; flash
; (c) 2025 Anders Granlund
;
; ROM flasher
;
;-------------------------------------------------------------------------------
;
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
;
;-------------------------------------------------------------------------------
	TEXT

    .EXPORT setsr
    .EXPORT restart

setsr:
    move.w  sr,d1
    and.w   #0xF0FF,d1
    and.w   #0x0F00,d0
    or.w    d0,d1
    move.w  sr,d0
    move.w  d1,sr
    rts

cacheoff:
    move.w  sr,d1               ; disable interrupts
    or.w    #0x0700,sr
    movec.l cacr,d0
    btst.l  #31,d0              ; data cache enabled?
    beq.b   .1
    nop
    cpusha  dc                  ; push data cache
.1: nop
    btst.l  #15,d0              ; instruction cache enabled?
    beq.b   .2
    nop
    cpusha  ic                  ; push instruction cache
.2: nop
    moveq.l #0,d0
    movec.l d0,cacr             ; disable caches
    nop
    move.w  d1,sr               ; restore interrupts
    rts

restart:
    move.w  #0x2700,sr          ; disable interrupts
    bsr     cacheoff            ; flush and disable caches
    move.l  0x40000004,-(sp)    ; get initial pc
    rts                         ; jump

