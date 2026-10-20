//;-------------------------------------------------------------------------------
//; Fat32 Filesystem driver for TOS
//; (c)2026 Anders Granlund
//;-------------------------------------------------------------------------------
//; This file is free software  you can redistribute it and/or modify
//; it under the terms of the GNU General Public License as published by
//; the Free Software Foundation  either version 2, or (at your option)
//; any later version.
//;
//; This file is distributed in the hope that it will be useful,
//; but WITHOUT ANY WARRANTY  without even the implied warranty of
//; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//; GNU General Public License for more details.
//;
//; You should have received a copy of the GNU General Public License
//; along with this program  if not, write to the Free Software
//; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//;-------------------------------------------------------------------------------
	.extern cpu

	.global cpu_SetSR
	.global cpu_FlushCache
	.global InstallTraps
	.global DispatchGemdos
	.global DispatchBios


//;-------------------------------------------------------------------------------
//; int16_t cpu_SetSR(int16_t sr)
//;-------------------------------------------------------------------------------
cpu_SetSR:
    move.l  4(sp),d1
    and.w   #0x0f00,d1
    move.w  sr,d0
    and.w   #0x0f00,d0
    move.w  d0,-(sp)
    move.w  sr,d0
    and.w   #0xf0ff,d0
    or.w    d1,d0
    move.w  d0,sr
    move.w  (sp)+,d0
    rts

//;-------------------------------------------------------------------------------
//; void cpu_FlushCache(uint8_t cpu)
//;-------------------------------------------------------------------------------
cpu_FlushCache:
	move.l	4(sp),d0
    cmp.b   #20,d0
    beq.b   cpu_FlushCache020
    cmp.b   #30,d0
    beq.b   cpu_FlushCache030
    cmp.b   #40,d0
    beq.b   cpu_FlushCache040
    cmp.b   #60,d0
    beq.b   cpu_FlushCache060
    rts

cpu_FlushCache020:
cpu_FlushCache030:
    nop
    dc.l   	0x4e7a0002  //; movec cacr,d0
    or.w    #0x0808,d0  //; instruction + data
    nop
    dc.l   	0x4e7b0002  //; movec d0,cacr
    nop
    rts

cpu_FlushCache040:
cpu_FlushCache060:
    nop
    dc.w	0xf4f8        //; cpusha bc
    nop
    rts



//;-------------------------------------------------------------------------------
//; Gemdos trap - 68020+
//;-------------------------------------------------------------------------------
	.align 4
gdos20_hdr:
	dc.b 	0, 0, 0, 0, 'X','B','R','A', 'F','T','3','2'
gdos20_old:
	dc.l 	0x00000084
gdos20_new:
	bset	#0,gdos20_hdr		//; re-entering?
	bne.b	2f
	move	usp,a0
	btst	#5,(sp)				//; already super?
	beq.b	1f
	lea		8(sp),a0			//; long stackframe
1:  move.l  a0,-(sp)
	jsr		DispatchGemdos
	move.b	#0,gdos20_hdr
	addq.l	#4,sp
	tst		d1
	beq.b	2f
	rte
2:	movea.l	gdos20_old(pc),a0
	jmp		(a0)


//;-------------------------------------------------------------------------------
//; Bios trap - 68020+
//;-------------------------------------------------------------------------------
	.align 4
bios20_hdr:
	dc.b 	0, 0, 0, 0, 'X','B','R','A', 'F','T','3','2'
bios20_old:
	dc.l 	0x000000b4
bios20_new:
	bset	#0,bios20_hdr		//; re-entering?
	bne.b	2f
	move	usp,a0
	btst	#5,(sp)				//; already super?
	beq.b	1f
	lea		8(sp),a0			//; long stackframe
1:  move.l  a0,-(sp)
	jsr		DispatchBios
	move.b	#0,bios20_hdr
	addq.l	#4,sp
	tst		d1
	beq.b	2f
	rte
2:	movea.l	bios20_old(pc),a0
	jmp		(a0)


//;-------------------------------------------------------------------------------
//; Gemdos trap - 68000
//;-------------------------------------------------------------------------------
	.align 2
gdos00_hdr:
	dc.b 	0, 0, 0, 0, 'X','B','R','A', 'F','T','3','2'
gdos00_old:
	dc.l 	0x00000084
gdos00_new:
	tas		gdos00_hdr			//; re-entering?
	bne.b	2f
	move	usp,a0
	btst	#5,(sp)				//; already super?
	beq.b	1f
	lea		6(sp),a0			//; short stackframe
1:  move.l  a0,-(sp)
	jsr		DispatchGemdos
	move.b	#0,gdos00_hdr
	addq.l	#4,sp
	tst		d1
	beq.b	2f
	rte
2:	movea.l	gdos00_old(pc),a0
	jmp		(a0)

//;-------------------------------------------------------------------------------
//; Bios trap - 68000
//;-------------------------------------------------------------------------------
	.align 2
bios00_hdr:
	dc.b 	0, 0, 0, 0, 'X','B','R','A', 'F','T','3','2'
bios00_old:
	dc.l 	0x000000b4
bios00_new:
	tas		bios00_hdr			//; re-entering?
	bne.b	2f
	move	usp,a0
	btst	#5,(sp)				//; already super?
	beq.b	1f
	lea		6(sp),a0			//; short stackframe
1:  move.l  a0,-(sp)
	jsr		DispatchBios
	move.b	#0,bios00_hdr
	addq.l	#4,sp
	tst		d1
	beq.b	2f
	rte
2:	movea.l	bios00_old(pc),a0
	jmp		(a0)


//;-------------------------------------------------------------------------------
//; void InstallTraps(void)
//;-------------------------------------------------------------------------------
InstallTraps:
	move.w	sr,-(sp)
	or.w	#0x0700,sr
	//; todo: correct way is to check cpu cookie.
	//; this here has an edge case where 68010 ends up in the 68020+ path.
	//; it works for now since no 020+ specific instructions are used anyway
	//;
	//; also there are no 68010 Ataris, and the number of people who desoldered
	//; the 68000 and replaced with a 68010 must be considered close to zero anyway.
	//; so in practice, this is fine as is.
	tst.w	0x59e.w
	bne.b	1f
	//; 68000
	move.l	0x84.w,gdos00_old
	move.l	0xb4.w,bios00_old
	move.l	#gdos00_new,0x84.w
	move.l	#bios00_new,0xb4.w
1:	//; 68020+
	move.l	0x84.w,gdos20_old
	move.l	0xb4.w,bios20_old
	move.l	#gdos20_new,0x84.w
	move.l	#bios20_new,0xb4.w
2:	move.w	(sp)+,sr
	rts
