				GLOBL	Syield
				GLOBL	Fpipe
				GLOBL	Ffchown
				GLOBL	Ffchmod
				GLOBL	Fsync
				GLOBL	Fcntl
				GLOBL	Finstat
				GLOBL	Foutstat
				GLOBL	Fgetchar
				GLOBL	Fputchar
				GLOBL	Pwait
				GLOBL	Pnice
				GLOBL	Pgetpid
				GLOBL	Pgetppid
				GLOBL	Pgetpgrp
				GLOBL	Psetpgrp
				GLOBL	Pgetuid
				GLOBL	Psetuid
				GLOBL	Pkill
				GLOBL	Psignal
				GLOBL	Pvfork
				GLOBL	Pgetgid
				GLOBL	Psetgid
				GLOBL	Psigblock
				GLOBL	Psigsetmask
				GLOBL	Pusrval
				GLOBL	Pdomain
				GLOBL	Psigreturn
				GLOBL	Pfork
				GLOBL	Pwait3
				GLOBL	Fselect
				GLOBL	Prusage
				GLOBL	Psetlimit
				GLOBL	Talarm
				GLOBL	Pause
				GLOBL	Sysconf
				GLOBL	Psigpending
				GLOBL	Dpathconf
				GLOBL	Pmsg
				GLOBL	Fmidipipe
				GLOBL	Prenice
				GLOBL	Dopendir
				GLOBL	Dreaddir
				GLOBL	Drewinddir
				GLOBL	Dclosedir
				GLOBL	Fxattr
				GLOBL	Flink
				GLOBL	Fsymlink
				GLOBL	Freadlink
				GLOBL	Dcntl
				GLOBL	Fchown
				GLOBL	Fchmod
				GLOBL	Pumask
				GLOBL	Psemaphore
				GLOBL	Dlock
				GLOBL	Psigpause
				GLOBL	Psigaction
				GLOBL	Pgeteuid
				GLOBL	Pgetegid
				GLOBL	Pwaitpid
				GLOBL	Dgetcwd
				GLOBL	Salert
				GLOBL	Tmalarm
				GLOBL	Psigintr
				GLOBL	Suptime
				GLOBL	Ptrace
				GLOBL	Mvalidate
				GLOBL	Dxreaddir
				GLOBL	Pseteuid
				GLOBL	Psetegid
				GLOBL	Pgetauid
				GLOBL	Psetauid
				GLOBL	Pgetgroups
				GLOBL	Psetgroups
				GLOBL	Tsetitimer
				GLOBL	Scookie
				GLOBL	Psetreuid
				GLOBL	Psetregid
				GLOBL	Sync
				GLOBL	Shutdown
				GLOBL	Dreadlabel
				GLOBL	Dwritelabel
				GLOBL	Srealloc
				GLOBL	Ssystem
				GLOBL	Tgettimeofday
				GLOBL	Tsettimeofday
				GLOBL	Tadjtime
				GLOBL	Pgetpriority
				GLOBL	Psetpriority
				GLOBL	Dchroot
				GLOBL	Fstat64
				GLOBL	Fseek64
				GLOBL	Dsetkey
				GLOBL	Fpoll
				GLOBL	Fwritev
				GLOBL	Freadv
				GLOBL	Ffstat64
				GLOBL	Psysctl
				GLOBL	Semulation

				GLOBL	Fsocket
				GLOBL	Fsocketpair
				GLOBL	Faccept
				GLOBL	Fconnect
				GLOBL	Fbind
				GLOBL	Flisten
				GLOBL	Frecvmsg
				GLOBL	Fsendmsg
				GLOBL	Frecvfrom
				GLOBL	Fsendto
				GLOBL	Fsetsockopt
				GLOBL	Fgetsockopt
				GLOBL	Fgetpeername
				GLOBL	Fgetsockname
				GLOBL	Fshutdown

				GLOBL	Pshmget
				GLOBL	Pshmctl
				GLOBL	Pshmat
				GLOBL	Pshmdt
				GLOBL	Psemget
				GLOBL	Psemctl
				GLOBL	Psemop
				GLOBL	Psemconfig
				GLOBL	Pmsgget
				GLOBL	Pmsgctl
				GLOBL	Pmsgsnd
				GLOBL	Pmsgrcv

				GLOBL	Maccess
				GLOBL	Fchown16
				GLOBL	Fchdir
				GLOBL	Ffdopendir
				GLOBL	Fdirfd

				
				MACRO	CALLDOS
				trap	#1
				ENDM
				
				
				MODULE	Srealloc
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$15,-(a7)
				CALLDOS
				add.w	#6,a7
				move.l	d0,a0
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Syield
				pea		(a2)
				move.w	#$ff,-(a7)
				CALLDOS
				add.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fpipe
				pea		(a2)
				pea		(a0)
				move.w	#$100,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Ffchown
				pea		(a2)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$101,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Ffchmod
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$102,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fsync
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$103,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fcntl
				pea		(a2)
				move.w	d2,-(a7)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$104,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Finstat
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$105,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Foutstat
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$106,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fgetchar
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$107,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fputchar
				pea		(a2)
				move.w	d2,-(a7)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$108,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pwait
				pea		(a2)
				move.w	#$109,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pnice
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$10a,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pgetpid
				pea		(a2)
				move.w	#$10b,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pgetppid
				pea		(a2)
				move.w	#$10c,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE Pgetpgrp
				pea		(a2)
				move.w	#$10d,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE Psetpgrp
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$10e,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pgetuid
				pea		(a2)
				move.w	#$10f,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psetuid
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$110,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pkill
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$111,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psignal
				pea		(a2)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$112,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	d0,a0
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pvfork
				move.l	(a7)+,a1
				move.w	#$113,-(a7)
				CALLDOS
				addq.w	#2,a7
				jmp		(a1)
				ENDMOD
				
				MODULE	Pgetgid
				pea		(a2)
				move.w	#$114,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psetgid
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$115,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psigblock
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$116,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psigsetmask
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$117,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pusrval
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$118,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pdomain
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$119,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psigreturn
				pea		(a2)
				move.w	#$11a,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pfork
				pea		(a2)
				move.w	#$11b,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pwait3
				pea		(a2)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$11c,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fselect
				pea		(a2)
				move.l	8(a7),-(a7)
				pea		(a1)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$11d,-(a7)
				CALLDOS
				lea		16(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Prusage
				pea		(a2)
				pea		(a0)
				move.w	#$11e,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psetlimit
				pea		(a2)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$11f,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Talarm
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$120,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pause
				pea		(a2)
				move.w	#$121,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Sysconf
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$122,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psigpending
				pea		(a2)
				move.w	#$123,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dpathconf
				pea		(a2)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$124,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pmsg
				pea		(a2)
				pea		(a0)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$125,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fmidipipe
				pea		(a2)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$126,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Prenice
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$127,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dopendir
				pea		(a2)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$128,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dreaddir
				pea		(a2)
				pea		(a0)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$129,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Drewinddir
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$12a,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dclosedir
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$12b,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fxattr
				pea		(a2)
				pea		(a1)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$12c,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Flink
				pea		(a2)
				pea		(a1)
				pea		(a0)
				move.w	#$12d,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fsymlink
				pea		(a2)
				pea		(a1)
				pea		(a0)
				move.w	#$12e,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Freadlink
				pea		(a2)
				pea		(a1)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$12f,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dcntl
				pea		(a2)
				move.l	d1,-(a7)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$130,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fchown
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$131,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fchmod
				pea		(a2)
				move.w	d0,-(a7)
				pea		(a0)
				move.w	#$132,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pumask
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$133,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psemaphore
				pea		(a2)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$134,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dlock
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$135,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psigpause
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$136,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psigaction
				pea		(a2)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$137,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pgeteuid
				pea		(a2)
				move.w	#$138,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pgetegid
				pea		(a2)
				move.w	#$139,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pwaitpid
				pea		(a2)
				pea     (a0)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$13a,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dgetcwd
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				pea     (a0)
				move.w	#$13b,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Salert
				pea		(a2)
				pea     (a0)
				move.w	#$13c,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Tmalarm
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$13d,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psigintr
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$13e,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Suptime
				pea		(a2)
				pea		(a1)
				pea		(a0)
				move.w	#$13f,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Ptrace
				pea		(a2)
				move.l	d2,-(a7)
				pea		(a0)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$140,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Mvalidate
				pea		(a2)
				pea		(a1)
				move.l	d1,-(a7)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$141,-(a7)
				CALLDOS
				lea		16(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dxreaddir
				pea		(a2)
				move.l	8(a7),-(a7)
				pea		(a1)
				pea		(a0)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$142,-(a7)
				CALLDOS
				lea		20(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pseteuid
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$143,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psetegid
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$144,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pgetauid
				pea		(a2)
				move.w	#$145,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psetauid
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$146,-(a7)
				CALLDOS
				addq.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pgetgroups
				pea		(a2)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$147,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psetgroups
				pea		(a2)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$148,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Tsetitimer
				pea		(a2)
				move.l	12(a7),-(a7)
				move.l	12(a7),-(a7)
				pea		(a1)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$149,-(a7)
				CALLDOS
				lea		20(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				; obsolete, same function number as Dchroot
				MODULE	Scookie
				pea		(a2)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$14a,-(a7)
				CALLDOS
				addq.w	#8,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dchroot
				pea		(a2)
				pea		(a0)
				move.w	#$14a,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fstat64
				pea		(a2)
				pea		(a1)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#$14b,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fseek64
				pea		(a2)
				pea		(a0) ; newpos
				move.w	12(a7),-(a7); how
				move.w	d2,-(a7) ; handle
				move.l	d1,-(a7) ; low
				move.l	d0,-(a7) ; high
				move.w	#$14c,-(a7)
				CALLDOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dsetkey
				pea		(a2)
				move.w	d2,-(a7) ; cipher
				pea		(a1) ; key
				move.l	d1,-(a7) ; minor
				move.l	d0,-(a7) ; major
				move.w	#$14d,-(a7)
				CALLDOS
				lea		16(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psetreuid
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$14e,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psetregid
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$14f,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Sync
				pea		(a2)
				move.w	#$150,-(a7)
				CALLDOS
				addq.w	#2,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Shutdown
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$151,-(a7)
				CALLDOS
				addq.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dreadlabel
				pea		(a2)
				move.w	d0,-(a7)
				pea     (a1)
				pea     (a0)
				move.w	#$152,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Dwritelabel
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.w	#$153,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Ssystem
				pea		(a2)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$154,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Tgettimeofday
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.w	#$155,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Tsettimeofday
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.w	#$156,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Tadjtime
				pea		(a2)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$157,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Pgetpriority
				pea		(a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$158,-(a7)
				CALLDOS
				lea		6(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psetpriority
				pea		(a2)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$159,-(a7)
				CALLDOS
				lea		8(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fpoll
				pea		(a2)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#$15a,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Fwritev
				pea		(a2)
				move.l	d1,-(a7)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$15b,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Freadv
				pea		(a2)
				move.l	d1,-(a7)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$15c,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Ffstat64
				pea		(a2)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$15d,-(a7)
				CALLDOS
				lea		8(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Psysctl
				pea		(a2)
				move.l	d1,-(a7) ; newlen
				move.l	16(a7),-(a7) ; new
				move.l	16(a7),-(a7) ; oldlen
				pea     (a1) ; old
				move.l	d0,-(a7) ; namelen
				pea     (a0) ; name
				move.w	#$15e,-(a7)
				CALLDOS
				lea		26(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
				MODULE	Semulation
				pea		(a2)
				move.l	28(a7),-(a7)
				move.l	28(a7),-(a7)
				move.l	28(a7),-(a7)
				move.l	28(a7),-(a7)
				move.l	28(a7),-(a7)
				move.l	28(a7),-(a7)
				move.l	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$15f,-(a7)
				CALLDOS
				lea		34(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
/* long Fsocket(long domain, long type, long protocol); */
				MODULE	Fsocket
				pea		(a2)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$160,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
/* long Fsocketpair(long domain, long type, long protocol, short fds[2]); */
				MODULE	Fsocketpair
				pea		(a2)
				pea     (a0)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$161,-(a7)
				CALLDOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
/* long Faccept(short fd, struct sockaddr *name, unsigned long *anamelen); */
				MODULE	Faccept
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$162,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
/* long Fconnect(short fd, const struct sockaddr *name, unsigned long anamelen); */
				MODULE	Fconnect
				pea		(a2)
				move.l	d1,-(a7)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$163,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD
				
/* long Fbind(short fd, const struct sockaddr *name, unsigned long namelen); */
				MODULE	Fbind
				pea		(a2)
				move.l	d1,-(a7)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$164,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

/* long Flisten(short fd, long backlog); */
				MODULE	Flisten
				pea		(a2)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$165,-(a7)
				CALLDOS
				lea		8(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

/* long Frecvmsg(short fd, struct msghdr *msg, long flags); */
				MODULE	Frecvmsg
				pea		(a2)
				move.l	d1,-(a7)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$166,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

/* long Fsendmsg(short fd, const struct msghdr *msg, long flags); */
				MODULE	Fsendmsg
				pea		(a2)
				move.l	d1,-(a7)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$167,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

/* long Frecvfrom(short fd, void *buf, unsigned long len, long flags, struct sockaddr *from, unsigned long *fromlenaddr); */
				MODULE	Frecvfrom
				pea		(a2)
				move.l	8(a7),-(a7)
				pea     (a1)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$168,-(a7)
				CALLDOS
				lea		24(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

/* long Fsendto(short fd, const void *buf, unsigned long len, long flags, const struct sockaddr *to, unsigned long tolen); */
				MODULE	Fsendto
				pea		(a2)
				move.l	8(a7),-(a7)
				pea     (a1)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$169,-(a7)
				CALLDOS
				lea		24(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

/* long Fsetsockopt(short fd, long level, long name, const void *val, unsigned long valsize); */
				MODULE	Fsetsockopt
				pea		(a2)
				move.l	8(a7),-(a7)
				pea     (a0)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$16a,-(a7)
				CALLDOS
				lea		20(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

/* long Fgetsockopt(short fd, long level, long name, void *val, unsigned long *avalsize); */
				MODULE	Fgetsockopt
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$16b,-(a7)
				CALLDOS
				lea		20(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

/* long Fgetpeername(short fd, struct sockaddr *asa, unsigned long *alen); */
				MODULE	Fgetpeername
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$16c,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

/* long Fgetsockname(short fd, struct sockaddr *asa, unsigned long *alen); */
				MODULE	Fgetsockname
				pea		(a2)
				pea     (a1)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#$16d,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

/* long Fshutdown(short fd, long how); */
				MODULE	Fshutdown
				pea		(a2)
				move.l	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#$16e,-(a7)
				CALLDOS
				lea		8(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Pshmget
				pea		(a2)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$170,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Pshmctl
				pea		(a2)
				pea     (a0)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$171,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Pshmat
				pea		(a2)
				move.l	d1,-(a7)
				pea     (a0)
				move.l	d0,-(a7)
				move.w	#$172,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Pshmdt
				pea		(a2)
				pea     (a0)
				move.w	#$173,-(a7)
				CALLDOS
				lea		6(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Psemget
				pea		(a2)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$174,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Psemctl
				pea		(a2)
				pea     (a0)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$175,-(a7)
				CALLDOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Psemop
				pea		(a2)
				move.l	d1,-(a7)
				pea     (a0)
				move.l	d0,-(a7)
				move.w	#$176,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Psemconfig
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$177,-(a7)
				CALLDOS
				lea		6(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Pmsgget
				pea		(a2)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$178,-(a7)
				CALLDOS
				lea		10(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Pmsgctl
				pea		(a2)
				pea     (a0)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				move.w	#$179,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Pmsgsnd
				pea		(a2)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				pea     (a0)
				move.l	d0,-(a7)
				move.w	#$17a,-(a7)
				CALLDOS
				lea		18(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Pmsgrcv
				pea		(a2)
				move.l	8(a7),-(a7)
				move.l	d2,-(a7)
				move.l	d1,-(a7)
				pea     (a0)
				move.l	d0,-(a7)
				move.w	#$17b,-(a7)
				CALLDOS
				lea		22(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Maccess
				pea		(a2)
				move.l	d1,-(a7)
				move.l	d0,-(a7)
				pea     (a0)
				move.w	#$17d,-(a7)
				CALLDOS
				lea		14(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fchown16
				pea		(a2)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				pea     (a0)
				move.w	#$180,-(a7)
				CALLDOS
				lea		12(a7),a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fchdir
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$181,-(a7)
				CALLDOS
				add.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Ffdopendir
				pea		(a2)
				move.w	d0,-(a7)
				move.w	#$182,-(a7)
				CALLDOS
				add.w	#4,a7
				move.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Fdirfd
				pea		(a2)
				move.l	d0,-(a7)
				move.w	#$183,-(a7)
				CALLDOS
				add.w	#6,a7
				move.l	(a7)+,a2
				rts
				ENDMOD
