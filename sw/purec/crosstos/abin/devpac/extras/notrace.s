
*	A TSR (terminate-and-stay-resident) demo program that also has
*	the useful feature of pointing the default trace exception vector
*	to an RTE, as described in the MonST Chapter.
*
*	This demo shows how to write a Terminate and Stay resident
*	program. It does *not* try to be super-smart whereby the code
*	is copied to the basepage to save memory, to preserve clarity.

* 14.8.92 updated to reflect Devpac 3 include files

	include gemdos.i
	include	bios.i	
	
start	bra.s	real_start

TSR_start
*-------	here starts the code to be TSRed
	bclr	#7,(sp)			works on 68000/10/20/30!
	rte
*-------	here ends the code to be TSRed
	even
keep_length	equ	*-start

real_start
* print a message via GEMDOS - this should be before the vector patching
* in case Ctrl-C is pressed during the printing
	pea	hellotx(pc)
	move.w	#c_conws,-(sp)		print string
	trap	#1
	addq.l	#6,sp
*-------	any initialisation for the TSR goes here
	move.l	#TSR_start,-(sp)
	move.w	#9,-(sp)		Trace vector number
	move.w	#setexc,-(sp)		setexc
	trap	#13
	addq.l	#8,sp			lose old one

*-------	end initialisation

	clr.w	-(sp)
	move.l	#$100+keep_length,-(sp)		$100 for basepage
	move.w	#p_termres,-(sp)
	trap	#1		that's the end

hellotx	dc.b	'NOTRACE installed ',189,' HiSoft 1988',13,10,0
