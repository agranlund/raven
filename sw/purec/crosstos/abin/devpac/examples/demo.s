* a simple test program for Devpac
* prints a simple message, waits for a key, then quits
* two deliberate mistakes

c_conws	equ	9
c_conin	equ	1

	opt	xdebug			long labels for debugging
	opt	hcln			and compressed line info
	opt	noeven			disable odd address checking
	
* firstly print the string

	move.l	#string,-(sp)
	move.w	#c_conws,-(sp)
	trap	#1
	addq.l	#6,a7			restore stack

* now wait for a key
	
	mov.w	c_conin,-(sp)
	trap	#1
	addq.l	#2,a7

* and quit
	clr.w	-(sp)
	trap	#1			quick exit

string	dc.b	"A simple GEMDOS program",13,10
	dc.b	"Press any key to Quit...",0
