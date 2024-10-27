****************
* "normal" cstart routine:
* - setting up argc and argv
* - setting up environment
* - shrink memory
* - and start main
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
                
                GLOBL   __text
                GLOBL   __data
                GLOBL   __bss
                
                XREF    main
                XREF    _fpuinit
                XREF    _StkSize
                XREF    _FreeAll

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

__text:
TcStart:        BRA     TcStart0
                dc.l    _RedirTab               * Redirection array pointer
_stksize:       dc.l    _StkSize                * Stack size entry
EmpStr:         dc.b    0,0
TcStart0:       move.l  a0,a3
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
                sub.l   _stksize,d0
                add.l	#256,d0
                move.l  d0,_StkLim
                
* Free not required memory

                MOVE.L  a0,-(A7)
                MOVE.L  A3,-(A7)
                MOVE.W  #0,-(A7)
                MOVE.W  #Mshrink,-(A7)
                TRAP    #1
                LEA.L   12(A7),A7

* Test if fpu 68881 is present
                
                jsr     _fpuinit
                
                
******* Execute main program *******************************************
*
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

* Execute all registered atexit procedures

                MOVE.L  _AtExitVec,D0
                BEQ     __exit
                MOVE.L  D0,A0
                JSR     (A0)

* Deinitialize file system

__exit:         MOVE.L  _FilSysVec,D0
                BEQ     Exit1
                MOVE.L  D0,A0
                JSR     (A0)

* Deallocate all heap blocks

Exit1:          JSR     _FreeAll

* Program termination with return code

                MOVE.W  #Pterm,-(A7)
                TRAP    #1



                BSS

__bss:
_base:
_BasPag:        ds.l    1                       * Pointer to Basepage
_app:           ds.w    1                       * Application/Accessory flag
_StkLim:        ds.l    1                       * Stack limit
_PgmSize:       ds.l    1                       * Program size
_RedirTab:      ds.l    6                       * Redirection address table

                DATA
                
__data:
errno:          dc.w    0                       * Global error variable
_AtExitVec:     dc.l    0                       * Vector for atexit
_FilSysVec:     dc.l    0                       * Vector for file system deinitialization
