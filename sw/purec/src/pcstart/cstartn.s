****************
* extended cstart routine:
* - setting up argc and argv
* - setting up environment
* - shrink memory
* - and start main
*
* parameters are NOT wildcard expanded
*
                GLOBL   exit
                GLOBL   __exit
                GLOBL   _BasPag
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
				XREF	virus_self_test
				XREF	p_stacksize


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



                TEXT

Fsetdta         equ     $1a
Fgetdta         equ     $2f
Mshrink         equ     $4a
Pterm           equ     $4c
Fsfirst         equ     $4e
Fsnext          equ     $4f


__text:
Start:          bra     Start0

                dc.l    _RedirTab
_stksize:       dc.l    _StkSize                * Stack size entry

EmpStr:         dc.b    0,0

Start0:         move.l  a0,a3
                move.l  a3,d0
                bne     acc
                move.l  4(a7),a3
                moveq.l #1,d0
                bra     app
acc:            clr.w   d0
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
Start6:         move.b  (a0)+,d1
                beq     Start7
                cmp.b   #' '+1,d1
                bpl     Start6
Start7:         clr.b   -1(a0)
                bra     Start2
Start99:        clr.l   (a1)+


                move.l  a1,_StkLim
                move.l	a7,_StkTop
                move.l  _PgmSize,-(a7)
                move.l  a3,-(a7)
                move.w  #0,-(a7)
                move.w  #Mshrink,-(a7)
                trap    #1
                lea     12(a7),a7

;				jsr		virus_self_test
;               jsr     _fpuinit

                move.l  d3,d0
                movea.l a2,a0
                movea.l a4,a1
                jsr     main


exit:           move.w  d0,-(a7)
                move.l  _AtExitVec,d0
                beq     __exit
                movea.l d0,a0
                jsr     (a0)

__exit:
				ifne	0
				move.l	_StkLim,a0
				move.l	_StkTop,d0
stkloop:		cmp.l	a0,d0
				bcs		stkend
				tst.l	(a0)+
				beq		stkloop
stkend:			sub.l	a0,d0
				addq.l	#4,d0
				jsr		p_stacksize
				endif
				
                move.l  _FilSysVec,d0
                beq     Exit1
                movea.l d0,a0
                jsr     (a0)
Exit1:          jsr     _FreeAll

                move.w  #Pterm,-(a7)
                trap    #1


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
_StkTop:        ds.l    1
_PgmSize:       ds.l    1
_RedirTab:      ds.b    24
environ:        ds.l    1
_init_environ:  ds.l    1
olddta:         ds.l    1
dta:            ds.b    44
