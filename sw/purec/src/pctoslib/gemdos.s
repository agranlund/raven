				GLOBL	gemdos
				GLOBL	Tsettime
				GLOBL	Tsetdate
				GLOBL	Tgettime
				GLOBL	Tgetdate
				GLOBL	Sversion
				GLOBL	Super
				GLOBL	Ptermres
				GLOBL	Pterm0
				GLOBL	Pterm
				GLOBL	Pexec
				GLOBL	Mshrink
				GLOBL	Mfree
				GLOBL	Malloc
				GLOBL	Fwrite
				GLOBL	Fshrink
				GLOBL	Fsnext
				GLOBL	Fsfirst
				GLOBL	Fsetdta
				GLOBL	Fseek
				GLOBL	Frename
				GLOBL	Fread
				GLOBL	Fopen
				GLOBL	Fgetdta
				GLOBL	Fforce
				GLOBL	Fdup
				GLOBL	Fdelete
				GLOBL	Fdatime
				GLOBL	Fcreate
				GLOBL	Fclose
				GLOBL	Fattrib
				GLOBL	Dsetpath
				GLOBL	Dsetdrv
				GLOBL	Dgetpath
				GLOBL	Dgetdrv
				GLOBL	Dfree
				GLOBL	Ddelete
				GLOBL	Dcreate
				GLOBL	Crawio
				GLOBL	Crawcin
				GLOBL	Cprnout
				GLOBL	Cprnos
				GLOBL	Cnecin
				GLOBL	Cconws
				GLOBL	Cconrs
				GLOBL	Cconout
				GLOBL	Cconos
				GLOBL	Cconis
				GLOBL	Cconin
				GLOBL	Cauxout
				GLOBL	Cauxos
				GLOBL	Cauxis
				GLOBL	Cauxin
				
				GLOBL	Mxalloc
				GLOBL	Maddalt
				
				GLOBL	Nversion
				GLOBL	Unlock
				GLOBL	Lock
				GLOBL	Flock
				GLOBL	F_lock
				GLOBL	Funlock
				GLOBL	Frunlock
				GLOBL	Frlock
				GLOBL	Fflush

				GLOBL	Nenable		/* Pamsnet */
				GLOBL	Ndisable	/* Pamsnet */
				GLOBL	Nremote		/* Pamsnet */
				GLOBL	Nmsg		/* Pamsnet */
				GLOBL	Nrecord		/* Pamsnet */
				GLOBL	Nreset		/* Pamsnet */
				GLOBL	Nprinter	/* Pamsnet */
				GLOBL	Nlocked		/* Pamsnet */
				GLOBL	Nunlock		/* Pamsnet */
				GLOBL	Nlock		/* Pamsnet */
				GLOBL	Nlogged		/* Pamsnet */
				GLOBL	Nnodeid		/* Pamsnet */
				GLOBL	Nactive		/* Pamsnet */
				
				GLOBL	Sconfig		/* KAOS 1.2 */
				GLOBL	Mgrow		/* KAOS 1.2 */
				GLOBL	Mblavail	/* KAOS 1.2 */
				
				GLOBL	Slbopen		/* Magic 5.20 */
				GLOBL	Slbclose	/* Magic 5.20 */
				
				MACRO	CALLDOS
				trap	#1
				ENDM
				
				MODULE	gemdos
				move.l	save_ptr,a0			
				move.l	(a7)+,-(a0)
				move.l	a2,-(a0)
				move.l	a0,save_ptr
				move.w	d0,-(a7)
				CALLDOS
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

				MODULE	Pterm0
				pea		(a2)
				move.w	#$00,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cconin
				pea		(a2)
				move.w	#$01,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cconout
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$02,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cauxin
				pea		(a2)
				move.w	#$03,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cauxout
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$04,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cprnout
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$05,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Crawio
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$06,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Crawcin
				pea		(a2)
				move.w	#$07,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cnecin
				pea		(a2)
				move.w	#$08,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cconws
				pea		(a2)
				pea		(a0)
				move.w	#$09,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cconrs				
				pea		(a2)
				pea		(a0)
				move.w	#$0A,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cconis
				pea		(a2)
				move.w	#$0B,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Dsetdrv
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$0E,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cconos
				pea		(a2)
				move.w	#$10,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cprnos
				pea		(a2)
				move.w	#$11,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cauxis
				pea		(a2)
				move.w	#$12,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Cauxos
				pea		(a2)
				move.w	#$13,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Dgetdrv
				pea		(a2)
				move.w	#$19,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fsetdta
				pea		(a2)
				pea		(a0)
				move.w	#$1A,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

; no push/pop a2 here,
; because we are switching stacks
; luckily, the Super() function is handled as a special
; case in TOS and does not change a2
				MODULE	Super
				pea		(a0)
				move.w	#$20,-(a7)
				CALLDOS
				addq.w	#6,a7
				rts
				ENDMOD

				MODULE	Tgetdate
				pea		(a2)
				move.w	#$2A,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Tsetdate
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$2B,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Tgettime
				pea		(a2)
				move.w	#$2C,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Tsettime
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$2D,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fgetdta
				pea		(a2)
				move.w	#$2F,-(a7)
				CALLDOS
				addq.w	#2,a7
				movea.l	d0,a0
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Sversion
				pea		(a2)
				move.w	#$30,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Ptermres
				pea		(a2)
				move.w	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$31,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Dfree
				pea		(a2)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$36,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Dcreate
				pea		(a2)
				pea		(a0)
				move.w	#$39,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Ddelete
				pea		(a2)
				pea		(a0)
				move.w	#$3A,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Dsetpath
				pea		(a2)
				pea		(a0)
				move.w	#$3B,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fcreate
				pea		(a2)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$3C,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fopen
				pea		(a2)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$3D,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fclose
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$3E,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fread
				pea		(a2)				
				pea		(a0)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$3F,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fwrite
				pea		(a2)				
				pea		(a0)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$40,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fshrink
				pea		(a2)
				moveq	#-1,d1
				move.l	d1,-(a7)
				moveq	#0,d1
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$40,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fdelete
				pea		(a2)
				pea		(a0)
				move.w	#$41,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fseek
				pea		(a2)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$42,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fattrib
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$43,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fdup				
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$45,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fforce
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$46,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Dgetpath
				pea		(a2)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$47,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Malloc
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$48,-(a7)
				CALLDOS
				addq.w	#6,a7
				movea.l	d0,a0
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Mfree
				pea		(a2)
				pea		(a0)
				move.w	#$49,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Mshrink
				pea		(a2)				
				move.l	d0,-(a7)
				pea		(a0)
				clr.w	-(a7)
				move.w	#$4A,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Pexec
				pea		(a2)
				move.l	8(a7),-(a7)
				pea		(a1)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$4B,-(a7)
				CALLDOS
				lea		16(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Pterm
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$4C,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fsfirst
				pea		(a2)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$4E,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fsnext
				pea		(a2)
				move.w	#$4F,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Frename
				pea		(a2)
				pea		(a1)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$56,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fdatime
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$57,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD


; TOS 030

				MODULE	Maddalt
				pea		(a2)
				move.l	d0,-(a7)
				pea		(a0)
				move.w	#$14,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	d0,a0
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Mxalloc
				pea		(a2)
				move.w	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$44,-(a7)
				CALLDOS
				addq.l	#8,a7
				move.l	d0,a0
				move.l	(a7)+,a2
				rts
				ENDMOD

; KAOS 1.2
				MODULE	Sconfig
				pea		(a2)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$33,-(a7)
				CALLDOS
				addq.l	#8,a7
				move.l	d0,a0
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Mgrow
				pea		(a2)
				move.l	d0,-(a7)
				pea		(a0)
				clr.w	-(a7)
				move.w	#$4a,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	d0,a0
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Mblavail
				pea		(a2)
				pea		$ffffffff.w
				pea		(a0)
				clr.w	-(a7)
				move.w	#$4a,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	d0,a0
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Slbopen
				pea		(a2)
				move.l	12(a7),-(a7)
				move.l	12(a7),-(a7)
				move.l	d0,-(a7)
				pea		(a1)
				pea		(a0)
				move.w	#$16,-(a7)
				CALLDOS
				lea		22(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
								
				MODULE	Slbclose
				pea		(a2)
				pea		(a0)
				move.w	#$17,-(a7)
				CALLDOS
				addq.l	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
								
; Network

				MODULE	Flock
				pea		(a2)
				move.l	8(a7),-(a7)
				move.l	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$5c,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Nversion
				pea		(a2)
				move.w	#$60,-(a7)
				CALLDOS
				addq.l	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Frlock
				pea		(a2)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$62,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Frunlock
				pea		(a2)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$63,-(a7)
				CALLDOS
				addq.l	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	F_lock
				pea		(a2)				
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$64,-(a7)
				CALLDOS
				addq.l	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Funlock
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$65,-(a7)
				CALLDOS
				addq.l	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fflush
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$66,-(a7)
				CALLDOS
				addq.l	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Unlock
				pea		(a2)
				pea		(a0)
				move.w	#$7b,-(a7)
				CALLDOS
				addq.l	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Lock
				pea		(a2)
				pea		(a0)
				move.w	#$7c,-(a7)
				CALLDOS
				addq.l	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

; Pamsnet

				MODULE	Nenable
				pea		(a2)
				move.w	#$73,-(a7)
				CALLDOS
				addq.l	#2,a7
				move.l	(a7)+,a2
				rts

				MODULE	Ndisable
				pea		(a2)
				move.w	#$74,-(a7)
				CALLDOS
				addq.l	#2,a7
				move.l	(a7)+,a2
				rts

				MODULE	Nremote
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$75,-(a7)
				CALLDOS
				addq.l	#4,a7
				move.l	(a7)+,a2
				rts

				MODULE	Nmsg
				pea		(a2)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				pea		(a1)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$76,-(a7)
				CALLDOS
				lea		16(a7),a7
				move.l	(a7)+,a2
				rts

				MODULE	Nrecord
				pea		(a2)
				move.l	8(a7),-(a7)
				move.l	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$77,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts

				MODULE	Nreset
				pea		(a2)
				move.w	#$78,-(a7)
				CALLDOS
				addq.l	#2,a7
				move.l	(a7)+,a2
				rts

				MODULE	Nprinter
				pea		(a2)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$79,-(a7)
				CALLDOS
				addq.l	#8,a7
				move.l	(a7)+,a2
				rts

				MODULE	Nlocked
				pea		(a2)
				move.w	#$7a,-(a7)
				CALLDOS
				addq.l	#2,a7
				move.l	(a7)+,a2
				rts

				MODULE	Nunlock
				pea		(a2)
				pea		(a0)
				move.w	#$7b,-(a7)
				CALLDOS
				addq.l	#6,a7
				move.l	(a7)+,a2
				rts

				MODULE	Nlock
				pea		(a2)
				pea		(a0)
				move.w	#$7c,-(a7)
				CALLDOS
				addq.l	#6,a7
				move.l	(a7)+,a2
				rts

				MODULE	Nlogged
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$7d,-(a7)
				CALLDOS
				addq.l	#4,a7
				move.l	(a7)+,a2
				rts

				MODULE	Nnodeid
				pea		(a2)
				move.w	#$7e,-(a7)
				CALLDOS
				addq.l	#2,a7
				move.l	(a7)+,a2
				rts

				MODULE	Nactive
				pea		(a2)
				move.w	#$7f,-(a7)
				CALLDOS
				addq.l	#2,a7
				move.l	(a7)+,a2
				rts
