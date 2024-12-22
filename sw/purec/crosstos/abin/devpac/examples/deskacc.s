
* a sample desk accessory supplied with Devpac

* Source code Copyright (C) 1988,1992 HiSoft. All rights reserved.
* No part of this source may be reproduced, transmitted,
* transcribed, or stored in a retrieval system, or translated
* in any form or by any means without the prior written
* permission of HiSoft.

* HiSoft makes no representations or warranties with respect
* to the contents hereof and specifically disclaims any
* implied warranties or merchantability or fitness for any
* particular purpose.

* feel free to use any or all of the object code

* If you cannot assemble this exactly as supplied, tell us.
* If the object doesn't run after you have made any changes,
* please do not tell us, as you're on your own once you
* start messing with it!

* last changed:14.8.92
* updated for Devpac 3

* this is a conditional so that a program may be assembled as
* an accessory or as stand-alone to debug

RUNNER	equ	0		1 for .PRG, 0 for .ACC

* NOTE: with this particular program assembled stand-alone there is no way
* to exit from the program!

	IFEQ	RUNNER
	OUTPUT	.ACC
	ELSEIF
	opt x+				dump long labels for debugging
	OUTPUT	.PRG
	ENDC

	include	gemmacro.i

* the program proper
	IFEQ	RUNNER
start	move.l	#mystack,a7		must have a stack!
	ELSEIF
start	move.l	4(a7),a3		base page
	move.l	#mystack,a7
	move.l	$c(a3),d0		text len
	add.l	$14(a3),d0		data len
	add.l	$1c(a3),d0		BSS len
	add.l	#$100,d0		basepage
	move.l	d0,-(sp)
	move.l	a3,-(sp)
	clr.w	-(sp)
	move.w	#$4a,-(sp)
	trap	#1			shrink memory
	lea	12(sp),sp
	ENDC

	appl_init
	move.w	d0,ap_id		store the application id

	IFEQ	RUNNER
* start by installing me in the Desk menu
	menu_register	ap_id,#mymenu
	ELSEIF
* set the mouse to an arrow
	graf_mouse	#0
	bra	open_acc		then pretend an Open
	ENDC

* the main loop of the application
* the only interesting events are messages
waitforevent
	evnt_mesag	#messagebuf
	move.l	#messagebuf,a0
	move.w	(a0),d0			message type
	cmp.w	#40,d0
	beq	open_acc
* check others here
	bra.s	waitforevent

* here when I have to Open
open_acc
	form_alert	#1,#myalert
	bra	waitforevent

	SECTION	DATA

* all C strings must end in a null
mymenu	dc.b	'  HiSoft Demo',0

myalert dc.b	'[1][This is a Desk Accessory|'
	dc.b	'written with Devpac 3][ OK ]',0

* global constants
	SECTION	BSS

ap_id		ds.w 1
messagebuf	ds.b 16

	ds.l	100			stack space
mystack	ds.w	1			(stacks go backwards)


* if not linking then include the run-times

	IFEQ	__LK
	include	aeslib.s
	ENDC
