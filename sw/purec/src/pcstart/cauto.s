*******
* cstart module for use within AUTO-folder programs & ACC's:
* - no argc, argv setup
* - no environment
* - just shrink memory and start main
*

                XREF    main
				GLOBL	_app
				GLOBL	_BasPag
                GLOBL	_base
                GLOBL	__base
                GLOBL   errno
                GLOBL   _PgmSize
				
                GLOBL   __text
                GLOBL   __data
                GLOBL   __bss
                
Mshrink         equ     $4a
Pterm           equ     $4c

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

******** Tc startup code


__text:
TcStart:        move.l  a0,a3
                move.l  a3,d0
                bne     acc
                move.l  4(a7),a3
                moveq.l #1,d0
                bra     app
acc:            clr.w   d0
app:            MOVE.L  A3,_BasPag
                MOVE.W  D0,_app
                MOVE.L  TextSegSize(A3),A0
                ADD.L   DataSegSize(A3),A0
                ADD.L   BssSegSize(A3),A0
                ADD.W   #BasePageSize,A0
                move.l  a0,_PgmSize
                
* Setup longword aligned application stack

                MOVE.L  A3,D0
                ADD.L   A0,D0
                AND.B   #$FC,D0
                MOVE.L  D0,A7
                
* Free not required memory

                MOVE.L  a0,-(A7)
                MOVE.L  A3,-(A7)
                MOVE.W  #0,-(A7)
                MOVE.W  #Mshrink,-(A7)
                TRAP    #1
                LEA.L   12(A7),A7

                JSR     main

******** exit ***********************************************************
*
* Terminate program
*
* Entry parameters:
*   <D0.W> = Termination status : Integer
* Return parameters:
*   Never returns

exit:           MOVE.W  D0,-(A7)
                MOVE.W  #Pterm,-(A7)
                TRAP    #1

                DATA
                
__data:
errno:          dc.l    0

				BSS
__bss:
_base:
__base:
_BasPag:		ds.l	1
_app:			ds.w	1
_PgmSize:       ds.l    1                       * Program size
