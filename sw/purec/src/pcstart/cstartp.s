****************
* extended cstart routine:
* - setting up argc and argv
* - setting up environment
* - shrink memory
* - install trap for profiling
* - and start main
*
* parameters are wildcard expanded
*
                GLOBL   exit
                GLOBL   __exit
                GLOBL   _BasPag
                GLOBL	_base
                GLOBL   _app
                GLOBL   errno
                GLOBL   _AtExitVec,_FilSysVec
                GLOBL   _RedirTab
                GLOBL	_stksize
                GLOBL   _StkLim
                GLOBL   _PgmSize
                GLOBL	environ
                GLOBL	_init_environ
                
                GLOBL   __text
                GLOBL   __data
                GLOBL   __bss
                
                XREF    main
                XREF    _fpuinit
                XREF    _StkSize
                XREF    _FreeAll


* Base page structure

                OFFSET  0

TpaStart:       ds.l    1
TpaEnd:         ds.l    1
TextSegStart:   ds.l    1
TextSegSize:    ds.l    1
DataSegStart:   ds.l    1
DataSegSize:    ds.l    1
BssSegStart:    ds.l    1
BssSegSize:     ds.l    1
DtaPtr:         ds.l    1
PntPrcPtr:      ds.l    1
Reserved0:      ds.l    1
EnvStrPtr:      ds.l    1
Reserved1:      ds.b    7
CurDrv:         ds.b    1
Reserved2:      ds.l    18
CmdLine:        ds.b    128
BasePageSize:   ds      0


PROFSYMSIZE		equ		16002
OUTBUFSIZE		equ		8192

                TEXT

Setexc			equ		5

Cconws			equ		$09
Fsetdta         equ     $1a
Fgetdta         equ     $2f
Fcreate			equ		$3c
Fopen			equ		$3d
Fclose			equ		$3e
Fread			equ		$3f
Fwrite			equ		$40
Fseek			equ		$42
Mshrink         equ     $4a
Pterm           equ     $4c
Fsfirst         equ     $4e
Fsnext          equ     $4f

Jdisint			equ		26
Jenabint		equ		27
Supexec			equ		38


__text:
Start:          bra     Start0

                dc.l    _RedirTab
_stksize:       dc.l    _StkSize                * Stack size entry

EmpStr:         dc.b    0,0

				dc.b	'PROF'
				
Start0:         move.l  a0,a3
                move.l  a3,d0
                bne     acc
                move.l  4(a7),a3
                moveq.l #1,d0
                bra     app
acc:            moveq.l #0,d0
app:            move.l  a3,_BasPag
                move.w  d0,_app
                movea.l TextSegSize(a3),a0
                adda.l  DataSegSize(a3),a0
                adda.l  BssSegSize(a3),a0
                adda.w  #BasePageSize,a0
                move.l  a0,_PgmSize

                move.l  a3,d0
                add.l   a0,d0
                and.b   #$FC,d0
                movea.l d0,a7

* check application flag

*        TST.W   _app
*        BEQ     Start8  * No environment and no arguments

                sub.l   _stksize,d0
                add.l	#4,d0
                and.b   #$FC,d0
                movea.l d0,a1
                movea.l a1,a4
                move.l  a1,environ
                move.l  a1,_init_environ
                movea.l EnvStrPtr(a3),a2
                move.l  a2,(a1)+
Start1:         tst.b   (a2)+
                bne     Start1
                move.l  a2,(a1)+
                tst.b   (a2)+
                bne     Start1
                clr.l   -4(a1)
                movea.l a1,a2

                move.l  #EmpStr,(a1)+   ;   argv[0] = ""
                moveq   #1,d3
                lea     CmdLine(a3),a0
                move.b  (a0)+,d1
                ext.w   d1
                clr.b   0(a0,d1)
Start2:         move.b  (a0)+,d1
                beq     Start99
                cmpi.b  #' '+1,d1
                bmi     Start2
                addq.l  #1,d3
                subq.l  #1,a0
                cmpi.b  #'"',d1
                bne     Start4
                addq.l  #1,a0
                move.l  a0,(a1)+
Start3:         move.b  (a0)+,d1
                beq     Start99
                cmp.b   #'"',d1
                bne     Start3
                clr.b   -1(a0)
                bra     Start2
Start4:         move.l  a0,(a1)+
                moveq   #-1,d2
Start5:         addq.w  #1,d2
Start6:         move.b  (a0)+,d1
                beq     Start7
                cmp.b   #'?',d1
                beq     Start5
                cmp.b   #'*',d1
                beq     Start5
                cmp.b   #' '+1,d1
                bpl     Start6
Start7:         clr.b   -1(a0)
                tst.w   d2
                beq     Start2
                subq.l  #4,a1
                movea.l (a1),a5
                subq.l  #1,d3
                movem.l a0-a2,-(a7)
                movea.l a5,a0
                cmpi.b  #':',1(a5)
                bne     Start8
                addq.l  #2,a0
Start8:         movea.l a0,a6
Start9:         move.b  (a0)+,d1
                beq     Start10
                cmp.b   #'\',d1
                bne     Start9
                bra     Start8
Start10:        move.w  #Fgetdta,-(a7)
                trap    #1
                addq.l  #2,a7
                move.l  d0,olddta
                pea     dta
                move.w  #Fsetdta,-(a7)
                trap    #1
                addq.l  #4,a7
                move.w  #7,(a7)
                move.l  a5,-(a7)
                move.w  #Fsfirst,-(a7)
                trap    #1
                addq.l  #8,a7
                bra     Start16
Start11:        lea     dta+30,a0
                moveq   #0,d0
Start12:        addq.w  #1,d0
                tst.b   (a0)+
                bne     Start12
                movem.l (a7)+,d1-d2/d4
                move.l  a7,d5
                sub.w   d0,d5
                and.b   #$FE,d5
                movea.l d5,a1
                movea.l d5,a7
                move.l  a6,d0
                sub.l   a5,d0
                tst.w   d0
                beq     Start14
                sub.w   d0,d5
                and.b   #$FE,d5
                movea.l d5,a7
                movea.l d5,a1
                movea.l a5,a0
Start13:        move.b  (a0)+,(a1)+
                cmpa.l  a0,a6
                bne     Start13
Start14:        lea     dta+30,a0
Start15:        move.b  (a0)+,(a1)+
                bne     Start15
                movea.l d2,a1
                move.l  a7,(a1)+
                addq.l  #1,d3
                move.l  a1,d2
                movem.l d1-d2/d4,-(a7)
                move.w  #Fsnext,-(a7)
                trap    #1
                addq.l  #2,a7
Start16:        tst.w   d0
                beq     Start11
                move.l  olddta,-(a7)
                move.w  #Fsetdta,-(a7)
                trap    #1
                addq.l  #6,a7
                movem.l (a7)+,a0-a2
                bra     Start2
Start99:        clr.l   (a1)+


                move.l  a1,_StkLim
                move.l  _PgmSize,-(a7)
                move.l  a3,-(a7)
                move.w  #0,-(a7)
                move.w  #Mshrink,-(a7)
                trap    #1
                lea     12(a7),a7

;               jsr     _fpuinit

				movem.l	d0-d6/a0-a6,-(a7)
				clr.w	-(a7)
				pea		_profile_sname
				move	#Fopen,-(a7)
				trap	#1
				addq.l	#8,a7
				lea		_profile_errm2(pc),a0
				tst.w	d0
				bmi		_profile_abort
				move.w	d0,_profile_hd
				pea		_profile_syms
				pea		PROFSYMSIZE
				move.w	d0,-(a7)
				move.w	#Fread,-(a7)
				trap	#1
				lea		12(a7),a7
				lea		_profile_errm3(pc),a0
				tst.l	d0
				bmi		_profile_abort
				lea		_profile_errm4(pc),a0
				sub.l	#2,d0
				ble		_profile_abort
				move.l	d0,_profile_fsize
				lsr.l	#4,d0
				cmp.w	_profile_syms,d0
				bne		_profile_abort
				move.w	_profile_hd,-(a7)
				move.w	#Fclose,-(a7)
				trap	#1
				addq.l	#4,a7
				lea		_profile_syms+2,a0
				move.l	_profile_fsize,d0
Start98:		move.l	8(a0,d0.l),d1
				add.l	d1,12(a0,d0.l)
				sub.l	#16,d0
				bpl		Start98
				
				clr.w	-(a7)
				pea		_profile_name(pc)
				move.w	#Fcreate,-(a7)
				trap	#1
				addq.l	#8,a7
				lea		_profile_errm5(pc),a0
				tst.w	d0
				bmi		_profile_abort
				move.w	d0,_profile_hd
				lea		_profile_buf+4,a0
				move.l	a0,_profile_optr
				lea		OUTBUFSIZE-4(a0),a0
				move.l	a0,_profile_eptr
				
				pea		_profile(pc)
				move.w	#32,-(a7)		; trap #0
				move.w	#Setexc,-(a7)
				trap	#13
				addq.l	#8,a7
				move.l	d0,_profile_trp
				pea		_profile_nclk(pc)
				move.w	#$4d,-(a7)
				move.w	#Setexc,-(a7)
				trap	#13
				addq.l	#8,a7
				move.l	d0,_profile_vclk
				pea		_profile_eni(pc)
				move.w	#Supexec,-(a7)
				trap	#14
				addq.l	#6,a7
				move.w	#13,-(a7)
				move.w	#Jenabint,-(a7)
				trap	#14
				addq.l	#4,a7
				
				clr.l	_profile_cnt
				
				movem.l	(a7)+,d0-d6/a0-a6
				
                move.l  d3,d0
                movea.l a2,a0
                movea.l a4,a1
                
                clr.l	_profile_clk
                clr.l	_profile_lastclk
                jsr     main
				

exit:           move.w  d0,-(a7)
                move.l  _AtExitVec,d0
                beq     __exit
                movea.l d0,a0
                jsr     (a0)
__exit:         move.l  _FilSysVec,d0
                beq     Exit1
                movea.l d0,a0
                jsr     (a0)
Exit1:          jsr     _FreeAll

                move.l	_profile_trp,-(a7)
                move.w	#32,-(a7)
                move.w	#Setexc,-(a7)
                trap	#13
                addq.l	#8,a7
                move.w	#13,-(a7)
                move.w	#Jdisint,-(a7)
                trap	#14
                addq.l	#4,a7
                pea		_profile_disi(pc)
                move.w	#Supexec,-(a7)
                trap	#14
                addq.l	#6,a7
                move.l	_profile_vclk,-(a7)
                move.w	#$4d,-(a7)
                move.w	#Setexc,-(a7)
                trap	#13
                addq.l	#8,a7
                
				move.l	_profile_optr,a0
				bsr		_profile_out0
				move.l	a0,_profile_optr
				clr.w	-(a7)
				move.w	_profile_hd,-(a7)
				clr.l	-(a7)
				move.w	#Fseek,-(a7)
				trap	#1
				lea		10(a7),a7
				move.l	_profile_optr,a0
				move.l	_profile_cnt,(a0)+
				bsr		_profile_out0
				
				move.w	_profile_hd,-(a7)
				move.w	#Fclose,-(a7)
				trap	#1
				addq.l	#4,a7
				
_profile_exit:	move.w  #Pterm,-(a7)
                trap    #1

regoff			equ		28
cputype			equ		$059e

_profile_abort: pea		(a0)
				move.w	#Cconws,-(a7)
				trap	#1
				addq.l	#6,a7
				move.w	#-1,-(a7)
				bra		_profile_exit
				
_profile_err:	pea		_profile_errm1(pc)
_profile_err0:	move.w	#Cconws,-(a7)
				trap	#1
				addq.l	#6,a7
				move.w	#-1,d0
				bra		exit
_profile_errm1:	dc.b	13,10,"error writing profile, aborting", 13, 10, 0
_profile_errm2:	dc.b	"symbol file not found",13,10,0
_profile_errm3:	dc.b	"error reading symbol file",13,10,0
_profile_errm4: dc.b	"corrupt symbol file",13,10,0
_profile_errm5:	dc.b	"can't create profile",13,10,0

				align
				
_profile_eni:	move.b	#4,$fffffa19
				move.b	#48,$fffffa1f
				rts
_profile_disi:	clr.b	$fffffa19
				rts

_profile_nclk:	add.l	#1,_profile_clk
				bclr	#5,$fffffa0f
				rte
				
_profile:		clr.b	$fffffa19
				move.l	_profile_clk,_profile_time
				btst	#5,(a7)
				bne		_profile2
				movem.l	d0-d3/a0-a2,-(a7)
				move.l	regoff+2(a7),a1
				move.l	a1,d0
				move.l	_BasPag,a0
				sub.l	TextSegStart(a0),d0
				move.l	usp,a0
				move.b	#$01,_profile_flag
				move.w	(a1),d1
				cmp.w	#$4e75,d1
				beq		_profile1
				addq.l	#2,a1
				move.l	a1,regoff+2(a7)
				move.l	a6,-(a0)
				move.l	a0,a6
				add.w	d1,a0
				move.b	#$41,_profile_flag
				bra		_profile_log
_profile1:		move.l	a6,a0
				move.l	(a0)+,a6
_profile_log:	move.l	a0,-(a7)
				
				lea		_profile_syms+10,a0
				clr.l	d1
				move.l	_profile_fsize,d2
_profile0:		cmp.l	d2,d1
				bge		_profile9
				move.l	d1,d3
				add.l	d2,d3
				lsr.l	#1,d3
				and.w	#$fff0,d3
				cmp.l	0(a0,d3.l),d0
				bcc		_profile5
				move.l	d3,d2
				bra		_profile0
_profile5:		cmp.l	4(a0,d3.l),d0
				bcs		_profile6
				add.l	#16,d3
				move.l	d3,d1
				bra		_profile0
_profile6:		lsr.l	#4,d3
				move.w	d3,d0
				move.l	_profile_optr,a0
				bsr		_profile_out
				lsr.w	#8,d0
				bsr		_profile_out
				move.l	_profile_time,d1
				sub.l	_profile_lastclk,d1
				move.b	_profile_flag,d0
				cmp.l	#$100,d1
				bcs		_profile7
				add.b	#1,d0
				cmp.l	#$10000,d1
				bcs		_profile7
				add.b	#1,d0
				cmp.l	#$1000000,d1
				bcs		_profile7
				add.b	#1,d0
_profile7:		bsr		_profile_out
				and.w	#$3f,d0
				exg		d0,d1
_profile8:		bsr		_profile_out
				lsr.l	#8,d0
				subq.w	#1,d1
				bne		_profile8
				move.l	a0,_profile_optr
				add.l	#1,_profile_cnt
				
_profile9:		move.l	(a7)+,a0
				move.l	a0,usp
				movem.l	(a7)+,d0-d3/a0-a2
				move.b	#4,$fffffa19
				move.l	_profile_clk,_profile_lastclk
				rte
_profile2:		move.l	a0,-(a7)
				move.l	6(a7),a0
				cmp.w	#$4e75,(a0)
				beq		_profile3
				move.w	#$4e56,-2(a0)
				bra		_profile4
_profile3:		move.w	#$4e5e,-2(a0)
_profile4:		move.l	(a7)+,a0
				rte

_profile_out:	move.b	d0,(a0)+
				cmp.l	_profile_eptr,a0
				bne		_profile_out1
_profile_out0:	movem.l	d0-d2/a1-a2,-(a7)
				lea		_profile_buf,a1
				pea		(a1)
				move.l	a0,d0
				sub.l	a1,d0
				move.l	d0,-(a7)
				move.w	_profile_hd,-(a7)
				move.w	#Fwrite,-(a7)
				trap	#1
				lea		12(a7),a7
				tst.l	d0
				bmi		_profile_err
				lea		_profile_buf,a0
				movem.l	(a7)+,d0-d2/a1-a2
_profile_out1:	rts

_profile_name:	dc.b	"mon.out",0
_profile_sname:	dc.b	"mon.sym",0
				align

                DATA
                
__data:
errno:          dc.w    0
_AtExitVec:     dc.l    0
_FilSysVec:     dc.l    0


                BSS

__bss:
_base:
_BasPag:        ds.l    1
_app:           ds.w    1
_StkLim:        ds.l    1
_PgmSize:       ds.l    1
_RedirTab:      ds.b    24
environ:        ds.l    1
_init_environ:  ds.l    1
olddta:         ds.l    1
_profile_trp:	ds.l	1	; old TRAP 0
_profile_vclk:	ds.l	1	; old clock vector
_profile_clk:	ds.l	1	; clock tick
_profile_lastclk:ds.l	1	; last clock tick
_profile_hd:	ds.w	1	; file handle
_profile_fsize:	ds.l	1	; symbol size
_profile_optr:	ds.l	1	; output ptr
_profile_eptr:	ds.l	1	; end of output buffer
_profile_flag:	ds.w	1	; flag & size
_profile_cnt:	ds.l	1	; trap counter
_profile_time:	ds.l	1	; actual time


dta:            ds.b    44
_profile_syms:	ds.b	PROFSYMSIZE	; symbol buffer
_profile_buf:	ds.b	OUTBUFSIZE	; output buffer
