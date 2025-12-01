;-------------------------------------------------------------------------------
; rvsnd : common driver startup code
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

    .text
    .xref init
    .export rvsnd
    .export rvini

;-------------------------------------------------------------------------------
;
; driver entry point, must be first in the text segment
;
; parameters passed on stack
;   0 = return pointer
;   4 = rvsnd_driver_api_t*
;   8 = ini_t* 
;
;-------------------------------------------------------------------------------
rvxdrv_start:
    move.l  4(sp),rvsnd
    move.l  8(sp),rvini
    sub.l   d0,d0
    jsr     init
    rts

rvsnd:  dc.l    0
rvini:  dc.l    0

