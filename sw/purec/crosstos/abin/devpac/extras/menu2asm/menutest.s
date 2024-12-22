
* a sample GEM program supplied with Devpac
* Menu test program using the AES, VDI not required

* last changed:14.8.92
* (updated for Devpac 3 includes)

	opt xdebug,hcln			dump long labels for debugging

	include	gemmacro.i

MN_SELECTED	equ	10

* the program proper
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

	appl_init
	move.w	d0,ap_id		store the application id

* set the mouse to an arrow
	graf_mouse	#0		arrow please

	bsr	load_resources

	menu_bar	#menu_start,#1	display it

	clr.w	check_state

* the main loop of the application
* the only interesting events are messages
waitforevent
	evnt_mesag	#messagebuf
	move.l	#messagebuf,a0
	move.w	(a0),d0			message type
	cmp.w	#MN_SELECTED,d0
	beq	handle_menu

* nothing I'm interested in so try again
	bra	waitforevent


* to go away various things have to be tidied up
quit	
	menu_bar	#menu_start,#0
	appl_exit			tell GEM I've finished

* now quit to the desktop
	clr.w	-(a7)			status code
	move.w	#$4c,-(a7)		P_TERM
	trap	#1			and go away

* menu handling after one has been clicked on
handle_menu
	move.w	messagebuf+8,d4		menu item
	bsr.s	do_menu			act on it
* now unlight the menu title
	menu_tnormal	#menu_start,messagebuf+6,#1
	bra	waitforevent		and try again

* given a menu click (d4) work out what it is and act on it
do_menu	cmp.w	#m_about,d4
	beq.s	do_about
	cmp.w	#m_hello,d4
	beq.s	do_hello
	cmp.w	#m_quit,d4
	beq	quit
	cmp.w	#m_check,d4
	beq.s	do_check
	cmp.w	#m_dialog,d4
	beq.s	do_dialog
	rts

do_about
	form_alert	#1,#about_alert
	rts
do_hello
	form_alert	#1,#hello_alert
	rts
do_check
	eor.w	#1,check_state
	menu_icheck	#menu_start,#m_check,check_state
	rts
do_dialog
	form_center	#my_dialog
	movem.w	int_out+2,d3-d6			d3-d6 are dialog co-ords
	form_dial	#0,d3,d4,d5,d6,d3,d4,d5,d6
	objc_draw	#my_dialog,#0,#2,d3,d4,d5,d6
	form_do		#my_dialog,#0
	mulu	#ob_sizeof,d0
	move.l	#my_dialog,a0
	bclr	#0,ob_state+1(a0,d0.w)		un-select button
	form_dial	#3,d3,d4,d5,d6,d3,d4,d5,d6
	rts

*************************
*	RESOURCES	*
*************************
* done the hard way......

ob_flags	equ	8
ob_state	equ	10
ob_sizeof	equ	24

G_BOX		equ	20
G_STRING	equ	28
G_BUTTON	equ	26

NONE		equ	0
SELECTABLE	equ	1
DEFAULT		equ	2
EXIT		equ	4
LASTOB		equ	$20

NORMAL		equ	0
OUTLINED	equ	$10


nextobj	set	1
object	macro	next,head,tail,type,flags,state
	dc.w	\1,\2,\3
	dc.w	G_\4,\5,\6
nextobj	set	nextobj+1
	endm

* doesn't actually load a resource file as we don't have one, but
* instead converts the built-in resources co-ordinates
load_resources
	lea	menu_start(pc),a3
	bsr.s	convert_resource
	lea	my_dialog(pc),a3
	bsr.s	convert_resource
	rts

* given an object tree at a3, fixup its co-ordinates
convert_resource
	moveq	#0,d3
	move.l	a3,a0
.loop	move.l	a0,-(sp)
	rsrc_obfix	a3,d3
	move.l	(sp)+,a0
	btst	#5,ob_flags+1(a0)
	bne.s	.done
	addq.w	#1,d3
	add.w	#ob_sizeof,a0
	bra.s	.loop			until LASTOB
.done	rts

* once a menu is stabilised it is best to Insert the file
menu_start
	include	menutest.mnu

	even
nextobj	set	1
my_dialog
	object	0,nextobj,nextobj+1,BOX,NONE,NORMAL
	dc.l	2<<16+$1181
	dc.w	0,0,35,10
		object	nextobj,-1,-1,STRING,NONE,OUTLINED
		dc.l	titletx
		dc.w	5,2,25,2
		object	0,-1,-1,BUTTON,SELECTABLE!DEFAULT!EXIT!LASTOB,NORMAL
		dc.l	buttontx
		dc.w	12,7,12,2


* menu numbers are best calculated using the RS directive
	rsset	4
* titles first
	rs.b	1		File title
	rs.b	1		Test title
*				other titles
	rs.b	2
* now the items
m_about	rs.b	1			About item
	rs.b	6+1			other items under Desk
		rs.b	1	skip File title
m_hello	rs.b	1
m_quit	rs.b	1
		rs.b	1	skip Test item
m_check	rs.b	1
	rs.b	1
m_dialog rs.b	1


	SECTION	DATA
about_alert	dc.b	'[1][A test program written|'
		dc.b	'with DevpacST Version 2][ Great ]',0
hello_alert	dc.b	'[3][  ][ Hello! ]',0
titletx		dc.b	' A Hand-Made Dialog Box!',0
buttontx	dc.b	'Click me',0

	SECTION	BSS
* global variables

ws_handle	ds.w	1

ap_id		ds.w	1
messagebuf	ds.b	16
check_state	ds.w	1

	ds.l	100			stack space
mystack	ds.w	1			(stacks go backwards)


* if not linking then include the run-times

	IFEQ	__LK
	include	aeslib.s
* VDI not needed!!
	ENDC
