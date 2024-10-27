				GLOBL	xbios
				GLOBL	Supexec
				GLOBL	Ssbrk
				GLOBL	Settime
				GLOBL	Setscreen
				GLOBL	VsetScreen
				GLOBL	Setprt
				GLOBL	Setpalette
				GLOBL	Setcolor
				GLOBL	Scrdmp
				GLOBL	Rsconf
				GLOBL	Random
				GLOBL	Puntaes
				GLOBL	Prtblk
				GLOBL	Protobt
				GLOBL	Physbase
				GLOBL	Ongibit
				GLOBL	Offgibit
				GLOBL	Midiws
				GLOBL	Mfpint
				GLOBL	Logbase
				GLOBL	Keytbl
				GLOBL	Kbrate
				GLOBL	Kbdvbase
				GLOBL	Jenabint
				GLOBL	Jdisint
				GLOBL	Iorec
				GLOBL	Initmouse
				GLOBL	Ikbdws
				GLOBL	Giaccess
				GLOBL	Gettime
				GLOBL	Getrez
				GLOBL	Flopwr
				GLOBL	Flopver
				GLOBL	Floprd
				GLOBL	Floprate
				GLOBL	Flopfmt
				GLOBL	Vsync
				GLOBL	Dosound
				GLOBL	Cursconf
				GLOBL	Blitmode
				GLOBL	Bconmap
				GLOBL	Bioskeys
				
				GLOBL	EsetSmear
				GLOBL	EsetShift
				GLOBL	EsetPalette
				GLOBL	EsetGray
				GLOBL	EsetColor
				GLOBL	EsetBank
				GLOBL	EgetShift
				GLOBL	EgetPalette
				GLOBL	DMAwrite
				GLOBL	DMAread
				
				GLOBL	Waketime
				
				GLOBL	Metainit
				GLOBL	Metaopen
				GLOBL	Metaclose
				GLOBL	Metaread
				GLOBL	Metawrite
				GLOBL	Metastatus
				GLOBL	Metaioctl
				GLOBL	Metastartaudio
				GLOBL	Metastopaudio
				GLOBL	Metasetsongtime
				GLOBL	Metagettoc
				GLOBL	Metadiscinfo
				
				GLOBL	Dsp_DoBlock
				GLOBL	Dsp_BlkHandShake
				GLOBL	Dsp_BlkUnpacked
				GLOBL	Dsp_InStream
				GLOBL	Dsp_OutStream
				GLOBL	Dsp_IOStream
				GLOBL	Dsp_RemoveInterrupts
				GLOBL	Dsp_GetWordSize
				GLOBL	Dsp_Lock
				GLOBL	Dsp_Unlock
				GLOBL	Dsp_Available
				GLOBL	Dsp_Reserve
				GLOBL	Dsp_LoadProg
				GLOBL	Dsp_ExecProg
				GLOBL	Dsp_ExecBoot
				GLOBL	Dsp_LodToBinary
				GLOBL	Dsp_TriggerHC
				GLOBL	Dsp_RequestUniqueAbility
				GLOBL	Dsp_GetProgAbility
				GLOBL	Dsp_FlushSubroutines
				GLOBL	Dsp_LoadSubroutine
				GLOBL	Dsp_InqSubrAbility
				GLOBL	Dsp_RunSubroutine
				GLOBL	Dsp_Hf0
				GLOBL	Dsp_Hf1
				GLOBL	Dsp_Hf2
				GLOBL	Dsp_Hf3
				GLOBL	Dsp_BlkWords
				GLOBL	Dsp_BlkBytes
				GLOBL	Dsp_HStat
				GLOBL	Dsp_SetVectors
				
				GLOBL	Vsetmode
				GLOBL	VsetMode
				GLOBL	VsetSync
				GLOBL	VgetSize
				GLOBL	VsetRGB
				GLOBL	VgetRGB
				GLOBL	ValidMode
				GLOBL	VsetMask
				
				GLOBL	Locksnd
				GLOBL	Unlocksnd
				GLOBL	Soundcmd
				GLOBL	NSoundcmd
				GLOBL	Setbuffer
				GLOBL	Setmode
				GLOBL	Settracks
				GLOBL	Setmontracks
				GLOBL	Setinterrupt
				GLOBL	Buffoper
				GLOBL	Dsptristate
				GLOBL	Gpio
				GLOBL	Devconnect
				GLOBL	Sndstatus
				GLOBL	Buffptr
				
				GLOBL   CacheCtrl
				GLOBL   WdgCtrl
				GLOBL   ExtRsConf
				GLOBL   NVMaccess
				
				MACRO	CALLXBIOS
				trap	#14
				ENDM
				
				MODULE	xbios
				move.l	save_ptr,a0
				move.l	(a7)+,-(a0)
				move.l	a2,-(a0)
				move.l	a0,save_ptr
				move.w	d0,-(a7)
				CALLXBIOS
				addq.l	#2,a7
				move.l	save_ptr,a0
				move.l	(a0)+,a2
				move.l	(a0)+,-(a7)
				move.l	a0,save_ptr
				rts

				BSS
save_a2_pc:		ds.l	2*8
save_end:
				DATA
save_ptr:		dc.l	save_end
				TEXT
				ENDMOD

; STE/TT
				MODULE	Bconmap				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#44,-(a7)
				CALLXBIOS
				addq.l	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Bioskeys				
				pea     (a2)
				move.w	#$18,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Blitmode				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#$40,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cursconf				
				pea     (a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$15,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	DMAread				
				pea     (a2)
				move.w	d2,-(a7)
				pea     (a0)
				move.w	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#42,-(a7)
				CALLXBIOS
				lea.l	14(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	DMAwrite				
				pea     (a2)
				move.w	d2,-(a7)
				pea     (a0)
				move.w	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#43,-(a7)
				CALLXBIOS
				lea.l	14(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Dosound				
				pea     (a2)
				pea     (a0)
				move.w	#$20,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				move.l	d0,a0
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	EgetPalette				
				pea     (a2)
				pea     (a0)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#85,-(a7)
				CALLXBIOS
				lea.l	10(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	EgetShift				
				pea     (a2)
				move.w	#81,-(a7)
				CALLXBIOS
				addq.l	#2,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	EsetBank				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#82,-(a7)
				CALLXBIOS
				addq.l	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	EsetColor				
				pea     (a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#83,-(a7)
				CALLXBIOS
				addq.l	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	EsetGray				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#86,-(a7)
				CALLXBIOS
				addq.l	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	EsetPalette				
				pea     (a2)
				pea     (a0)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#84,-(a7)
				CALLXBIOS
				lea.l	10(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	EsetShift				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#80,-(a7)
				CALLXBIOS
				addq.l	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	EsetSmear				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#87,-(a7)
				CALLXBIOS
				addq.l	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Flopfmt				
				pea     (a2)
				move.w	16(a7),-(a7) ; virgin
				move.l	14(a7),-(a7) ; magic
				move.w	16(a7),-(a7) ; interlv
				move.w	16(a7),-(a7) ; sideno
				move.w	d2,-(a7) ; trackno
				move.w	d1,-(a7) ; spt
				move.w	d0,-(a7) ; devno
				pea     (a1) ; filler
				pea     (a0) ; buf
				move.w	#$A,-(a7)
				CALLXBIOS
				lea		26(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Floprate				
				pea     (a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#41,-(a7)
				CALLXBIOS
				addq.l	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Floprd				
				pea     (a2)
				move.l	8(a7),-(a7) ; sideno + count
				move.w	d2,-(a7) ; trackno
				move.w	d1,-(a7) ; sectno
				move.w	d0,-(a7) ; dev
				pea     (a1) ; filler
				pea     (a0) ; buf
				move.w	#8,-(a7)
				CALLXBIOS
				lea		20(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Flopver				
				pea     (a2)
				move.l	8(a7),-(a7)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				pea     (a1)
				pea     (a0)
				move.w	#$13,-(a7)
				CALLXBIOS
				lea		20(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Flopwr				
				pea     (a2)
				move.l	8(a7),-(a7)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				pea     (a1)
				pea     (a0)
				move.w	#9,-(a7)
				CALLXBIOS
				lea		20(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Getrez				
				pea     (a2)
				move.w	#4,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Gettime				
				pea     (a2)
				move.w	#$17,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Giaccess				
				pea     (a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$1C,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Ikbdws				
				pea     (a2)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$19,-(a7)
				CALLXBIOS
				addq.w	#8,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Initmouse
				pea     (a2)
				pea     (a1)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#0,-(a7)
				CALLXBIOS
				lea		$C(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Iorec				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#$E,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				movea.l	d0,a0
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Jdisint				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#$1A,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Jenabint				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#$1B,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Kbdvbase				
				pea     (a2)
				move.w	#$22,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				movea.l	d0,a0
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Kbrate				
				pea     (a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$23,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Keytbl				
				pea     (a2)
				move.l	8(a7),-(a7)
				pea     (a1)
				pea     (a0)
				move.w	#$10,-(a7)
				CALLXBIOS
				lea		$E(a7),a7
				movea.l	d0,a0
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Logbase				
				pea     (a2)
				move.w	#3,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				movea.l	d0,a0
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Mfpint				
				pea     (a2)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$D,-(a7)
				CALLXBIOS
				addq.w	#8,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Midiws				
				pea     (a2)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$C,-(a7)
				CALLXBIOS
				addq.w	#8,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Offgibit				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#$1D,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Ongibit				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#$1E,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Physbase				
				pea     (a2)
				move.w	#2,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				movea.l	d0,a0
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Protobt				
				pea     (a2)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#$12,-(a7)
				CALLXBIOS
				lea		14(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Prtblk				
				pea     (a2)
				pea     (a0)
				move.w	#$24,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Puntaes				
				pea     (a2)
				move.w	#$27,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Random				
				pea     (a2)
				move.w	#$11,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Rsconf				
				pea     (a2)
				move.w	12(a7),-(a7)
				move.w	12(a7),-(a7)
				move.w	12(a7),-(a7)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#15,-(a7)
				CALLXBIOS
				lea		14(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Scrdmp				
				pea     (a2)
				move.w	#$14,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Setcolor				
				pea     (a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#7,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Setpalette				
				pea     (a2)
				pea     (a0)
				move.w	#6,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Setprt				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#$21,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Setscreen				
VsetScreen:
				pea     (a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				pea     (a1)
				pea     (a0)
				move.w	#5,-(a7)
				CALLXBIOS
				lea		14(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Settime				
				pea     (a2)
				move.l	d0,-(a7)
				move.w	#$16,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Ssbrk				
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#1,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				movea.l	d0,a0
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Vsync				
				pea     (a2)
				move.w	#$25,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Supexec				
				pea     (a2)
				pea     (a0)
				move.w	#$26,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Xbtimer				
				pea     (a2)
				pea     (a0)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$1F,-(a7)
				CALLXBIOS
				lea		12(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

; ST-Book
				MODULE	Waketime
				pea     (a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$2F,-(a7)
				CALLXBIOS
				lea		6(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD
				

; Falcon

				MODULE	Dsp_DoBlock
				pea		(a2)
				move.l	d1,-(a7)
				pea		(a1)
				move.l	d0,-(a7)
				pea		(a0)
				move.w	#96,-(a7)
				CALLXBIOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_BlkHandShake
				pea		(a2)
				move.l	d1,-(a7)
				pea		(a1)
				move.l	d0,-(a7)
				pea		(a0)
				move.w	#97,-(a7)
				CALLXBIOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_BlkUnpacked
				pea		(a2)
				move.l	d1,-(a7)
				pea		(a1)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#98,-(a7)
				CALLXBIOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_InStream
				pea		(a2)
				pea     (a1)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#99,-(a7)
				CALLXBIOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_OutStream
				pea		(a2)
				pea     (a1)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#100,-(a7)
				CALLXBIOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_IOStream
				pea		(a2)
				move.l	8(a7),-(a7)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				pea     (a1)
				pea     (a0)
				move.w	#101,-(a7)
				CALLXBIOS
				lea		26(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_RemoveInterrupts
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#102,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_GetWordSize
				pea		(a2)
				move.w	#103,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_Lock
				pea		(a2)
				move.w	#104,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_Unlock
				pea		(a2)
				move.w	#105,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_Available
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.w	#106,-(a7)
				CALLXBIOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_Reserve
				pea		(a2)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#107,-(a7)
				CALLXBIOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_LoadProg
				pea		(a2)
				pea     (a1)
				move.w	d0,-(a7)
				pea     (a0)
				move.w	#108,-(a7)
				CALLXBIOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_ExecProg
				pea		(a2)
				move.w	d1,-(a7)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#109,-(a7)
				CALLXBIOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_ExecBoot
				pea		(a2)
				move.w	d1,-(a7)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#110,-(a7)
				CALLXBIOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_LodToBinary
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.w	#111,-(a7)
				CALLXBIOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_TriggerHC
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#112,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_RequestUniqueAbility
				pea		(a2)
				move.w	#113,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_GetProgAbility
				pea		(a2)
				move.w	#114,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_FlushSubroutines
				pea		(a2)
				move.w	#115,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_LoadSubroutine
				pea		(a2)
				move.w	d1,-(a7)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#116,-(a7)
				CALLXBIOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_InqSubrAbility
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#117,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_RunSubroutine
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#118,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_Hf0
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#119,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_Hf1
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#120,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_Hf2
				pea		(a2)
				move.w	#121,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_Hf3
				pea		(a2)
				move.w	#122,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_BlkWords
				pea		(a2)
				move.l	d1,-(a7)
				pea     (a1)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#123,-(a7)
				CALLXBIOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_BlkBytes
				pea		(a2)
				move.l	d1,-(a7)
				pea     (a1)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#124,-(a7)
				CALLXBIOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_HStat
				pea		(a2)
				move.w	#125,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_SetVectors
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.w	#126,-(a7)
				CALLXBIOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsp_MultBlocks
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#127,-(a7)
				CALLXBIOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Vsetmode
VsetMode:
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#88,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				GLOBL	VgetMonitor
				GLOBL	Vgetmonitor
				GLOBL	mon_type
				GLOBL	Montype
				MODULE	Montype
VgetMonitor:
Vgetmonitor:
mon_type:
				pea		(a2)
				move.w	#89,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	VsetSync
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#90,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	VgetSize
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#91,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	VsetRGB
				pea		(a2)
				pea		(a0)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#93,-(a7)
				CALLXBIOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	VgetRGB
				pea		(a2)
				pea		(a0)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#94,-(a7)
				CALLXBIOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	ValidMode
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#95,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	VsetMask
				pea		(a2)
				move.w	d2,-(a7)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#150,-(a7)
				CALLXBIOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Locksnd
				pea		(a2)
				move.w	#128,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Unlocksnd
				pea		(a2)
				move.w	#129,-(a7)
				CALLXBIOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Soundcmd
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#130,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	NSoundcmd
				pea		(a2)
				move.l	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#130,-(a7)
				CALLXBIOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Setbuffer
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#131,-(a7)
				CALLXBIOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Setmode
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#132,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Settracks
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#133,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Setmontracks
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#134,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Setinterrupt
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#135,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Buffoper
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#136,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsptristate
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#137,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Gpio
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#138,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Devconnect
				pea		(a2)
				move.l	8(a7),-(a7)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#139,-(a7)
				CALLXBIOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Sndstatus
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#140,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Buffptr
				pea		(a2)
				pea		(a0)
				move.w	#141,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
; METADOS

				MODULE	Metainit
				pea		(a2)
				pea		(a0)
				move.w	#$30,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metaopen
				pea		(a2)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$31,-(a7)
				CALLXBIOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metaclose
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$32,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metaread
				pea		(a2)
				move.w	d2,-(a7)
				move.l	d1,-(a7)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$33,-(a7)
				CALLXBIOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metawrite
				pea		(a2)
				move.w	d2,-(a7)
				move.l	d1,-(a7)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$34,-(a7)
				CALLXBIOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metastatus
				pea		(a2)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$36,-(a7)
				CALLXBIOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metaioctl
				pea		(a2)
				pea		(a0)
				move.w	d2,-(a7)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$37,-(a7)
				CALLXBIOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metastartaudio
				pea		(a2)
				pea		(a0)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$3b,-(a7)
				CALLXBIOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metastopaudio
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$3c,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metasetsongtime
				pea		(a2)
				move.l	8(a7),-(a7)
				move.l	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$3d,-(a7)
				CALLXBIOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metagettoc
				pea		(a2)
				pea		(a0)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$3e,-(a7)
				CALLXBIOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Metadiscinfo
				pea		(a2)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$3f,-(a7)
				CALLXBIOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	CacheCtrl
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#160,-(a7)
				CALLXBIOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	WdgCtrl
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#161,-(a7)
				CALLXBIOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	ExtRsConf
				pea		(a2)
				move.l	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#162,-(a7)
				CALLXBIOS
				lea     10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	NVMaccess
				pea		(a2)
				pea     (a0)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#46,-(a7)
				CALLXBIOS
				lea     12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
