* atari.s:	AS68/MADMAC synonyms & Atari equates for Lattice ASM
*************************************************************************
* History:
* 920514 agk	updated for new as68.syn
* 920424 kbad	minor bugfixes
* 920421 kbad	based on lmd's 870721 atari.s & Lattice as68.syn
*************************************************************************

	ifnd	__ATARI_S
__ATARI_S	equ 1

	include	as68.syn

* Magic numbers
* =======================================================================

bootmagic	equ $1234	; magic checksum for boot sector
memmagic	equ $752019f3	; validates 'memvalid'
memmag2		equ $237698aa	; validates 'memval2'
memmag3		equ $5555aaaa	; validates 'memval3'
rammagic	equ $1357bd13	; validates 'ramtop'
resmagic	equ $31415926	; validates 'resvalid'


* ST hardware locations
* =======================================================================

cartbase	equ $fa0000	; start	of cartridge ROM
cartsize	equ $20000	; size of cartridge (128K)

;--- Shifter
memconf		equ $ffff8001	; memory controller
dbaseh		equ $ffff8201	; display base high, low
dbasel		equ $ffff8203
vcounthi	equ $ffff8205	; display counter high, mid, low
vcountmid	equ $ffff8207
vcountlow	equ $ffff8209
syncmode	equ $ffff820a	; video	sync mode
color0		equ $ffff8240	; ST color palette #0
shiftmd		equ $ffff8260	; video	shift mode (resolution)

ttcolor0	equ $ffff8400	; TT background color reg
ttshiftmd	equ $ffff8262	; TT shift mode

;--- DMA chip
diskctl		equ $ffff8604	; disk controller data access
fifo		equ $ffff8606	; DMA mode control
dmahigh		equ $ffff8609	; DMA base high, mid, low
dmamid		equ $ffff860b
dmalow		equ $ffff860d

;--- 1770 registers
cmdreg		equ $80		; 1770/FIFO command register select
trkreg		equ $82		; 1770/FIFO track register select
secreg		equ $84		; 1770/FIFO sector register select
datareg		equ $86		; 1770/FIFO data register select

;--- Sound Chip
giselect	equ $ffff8800	; (W) sound chip register select
giread		equ $ffff8800	; (R) sound chip read-data
giwrite		equ $ffff8802	; (W) sound chip write-data
gitoneaf	equ 0		; channel A fine/coarse tune
gitoneac	equ 1
gitonebf	equ 2		; channel B
gitonebc	equ 3
gitonecf	equ 4		; channel C
gitonecc	equ 5
ginoise		equ 6		; noise generator control
gimixer		equ 7		; I/O control/volume control register
giaamp		equ 8		; channel A, B, C amplitude
gibamp		equ 9
gicamp		equ $a
gifienvlp	equ $b		; envelope period fine, coarse tune
gicrnvlp	equ $c
giporta		equ $e		; GI register# for I/O port A
giportb		equ $f		; Centronics output register

;--- Bits in "giporta":
xrts		equ 8		; RTS output
dtr		equ $10		; DTR output
strobe		equ $20		; Centronics strobe output
gpo		equ $40		; "general purpose" output

;--- 68901 (MFP)
mfp		equ $fffffa00	; mfp base

gpip		equ mfp+$01	; general purpose I/O
aer		equ mfp+$03	; active edge reg
ddr		equ mfp+$05	; data direction reg
iera		equ mfp+$07	; interrupt enable A & B
ierb		equ mfp+$09
ipra		equ mfp+$0b	; interrupt pending A & B
iprb		equ mfp+$0d
isra		equ mfp+$0f	; interrupt inService A & B
isrb		equ mfp+$11
imra		equ mfp+$13	; interrupt mask A & B
imrb		equ mfp+$15
vr		equ mfp+$17	; interrupt vector base
tacr		equ mfp+$19	; timer A control
tbcr		equ mfp+$1b	; timer B control
tcdcr		equ mfp+$1d	; timer C & D control
tadr		equ mfp+$1f	; timer A data
tbdr		equ mfp+$21	; timer B data
tcdr		equ mfp+$23	; timer C data
tddr		equ mfp+$25	; timer D data
scr		equ mfp+$27	; sync char
ucr		equ mfp+$29	; USART control reg
rsr		equ mfp+$2b	; receiver status
tsr		equ mfp+$2d	; transmit status
udr		equ mfp+$2f	; USART data

;--- TTMFP 68901
ttmfp		equ $fffffa80

ttgpip		equ ttmfp+$01	; general purpose I/O
ttaer		equ ttmfp+$03	; active edge reg
ttddr		equ ttmfp+$05	; data direction reg
ttiera		equ ttmfp+$07	; interrupt enable A & B
ttierb		equ ttmfp+$09
ttipra		equ ttmfp+$0b	; interrupt pending A & B
ttiprb		equ ttmfp+$0d
ttisra		equ ttmfp+$0f	; interrupt inService A & B
ttisrb		equ ttmfp+$11
ttimra		equ ttmfp+$13	; interrupt mask A & B
ttimrb		equ ttmfp+$15
ttvr		equ ttmfp+$17	; interrupt vector base
tttacr		equ ttmfp+$19	; timer A control
tttbcr		equ ttmfp+$1b	; timer B control
tttcdcr		equ ttmfp+$1d	; timer C & D control
tttadr		equ ttmfp+$1f	; timer A data
tttbdr		equ ttmfp+$21	; timer B data
tttcdr		equ ttmfp+$23	; timer C data
tttddr		equ ttmfp+$25	; timer D data
ttscr		equ ttmfp+$27	; sync char
ttucr		equ ttmfp+$29	; USART control reg
ttrsr		equ ttmfp+$2b	; receiver status
tttsr		equ ttmfp+$2d	; transmit status
ttudr		equ ttmfp+$2f	; USART data

;--- 6850s
keyctl		equ $fffffc00	; keyboard ACIA control
keybd		equ $fffffc02	; keyboard data
midictl		equ $fffffc04	; MIDI ACIA control
midi		equ $fffffc06	; MIDI data


* BIOS variables
* =======================================================================

;--- "extended" trap vectors:
etv_timer	equ $400	; vector for timer interrupt chain
etv_critic	equ $404	; vector for critical error chain
etv_term	equ $408	; vector for process terminate
etv_xtra	equ $40c	; 5 reserved vectors

memvalid	equ $420	; indicates system state on RESET
memcntlr	equ $424	; mem controller config nibble
resvalid	equ $426	; validates 'resvector'
resvector	equ $42a	; [RESET] bailout vector
phystop		equ $42e	; physical top of RAM
_membot		equ $432	; bottom of available memory;
_memtop		equ $436	; top of available memory;
memval2		equ $43a	; validates 'memcntlr' and 'memconf'
flock		equ $43e	; floppy disk/FIFO lock variable
seekrate	equ $440	; default floppy seek rate
_timr_ms	equ $442	; system timer calibration (in ms)
_fverify	equ $444	; nonzero: verify on floppy write
_bootdev	equ $446	; default boot device
palmode		equ $448	; nonzero :: PAL mode
defshiftmd	equ $44a	; default video rez (first byte)
sshiftmd	equ $44c	; shadow for 'shiftmd' register
_v_bas_ad	equ $44e	; pointer to base of screen memory
vblsem		equ $452	; semaphore to enforce mutex in	vbl
nvbls		equ $454	; number of deferred vectors
_vblqueue	equ $456	; pointer to vector of deferred	vfuncs
colorptr	equ $45a	; pointer to palette setup (or NULL)
screenpt	equ $45e	; pointer to screen base setup (|NULL)
_vbclock	equ $462	; count	of unblocked vblanks
_frclock	equ $466	; count	of every vblank

hdv_init	equ $46a	; hard disk initialization
swv_vec		equ $46e	; video change-resolution bailout
hdv_bpb		equ $472	; disk "get BPB"
hdv_rw		equ $476	; disk read/write
hdv_boot	equ $47a	; disk "get boot sector"
hdv_mediach	equ $47e	; disk media change detect

_cmdload	equ $482	; nonzero: load COMMAND.COM from boot
conterm		equ $484	; console/vt52 bitSwitches (%%0..%%2)

themd		equ $48e	; memory descriptor (MD)
_____md		equ $49e	; (more MD)
savptr		equ $4a2	; pointer to register save area

_nflops		equ $4a6	; number of disks attached (0, 1+)
con_state	equ $4a8	; state of conout() parser
save_row	equ $4ac	; saved row# for cursor X-Y addressing
sav_context	equ $4ae	; pointer to saved processor context
_bufl		equ $4b2	; two buffer-list headers
_hz_200		equ $4ba	; 200hz raw system timer tick
_drvbits	equ $4c2	; bit vector of "live" block devices
_dskbufp	equ $4c6	; pointer to common disk buffer
_autopath	equ $4ca	; pointer to autoexec path (or NULL)
_vbl_list	equ $4ce	; initial _vblqueue (to $4ee)

_dumpflg	equ $4ee	; screen-dump flag
_prtabt		equ $4f0	; printer abort flag
_sysbase	equ $4f2	; -> base of OS
_shell_p	equ $4f6	; -> global shell info
end_os		equ $4fa	; -> end of OS memory usage
exec_os		equ $4fe	; -> address of shell to exec on startup
scr_dump	equ $502	; -> screen dump code

;--- character-output vectors for _prtblk():
prv_lsto	equ $506	; -> _lstostat()
prv_lst		equ $50a	; -> _lstout()
prv_auxo	equ $50e	; -> _auxostat()
prv_aux		equ $512	; -> _auxout()

pun_ptr		equ $516	; -> hard disk driver phys unit table
memval3		equ $51a	; memory validation

_longframe	equ $59e	; if nonzero, we're on >68000
_p_cookie	equ $5a0	; -> cookie jar
_ramtop		equ $5a4	; top of ram starting at $01000000
_ramvalid	equ $5a8	; _ramtop valid if rammagic ($1357bd13)
bell_hook	equ $5ac	; handler for bell
kcl_hook	equ $5b0	; handler for keyclick


* "Line A" equates
* =======================================================================

VPLANES		equ 0
VWRAP		equ 2
CONTRL		equ 4
INTIN		equ 8
PTSIN		equ 12
INTOUT		equ 16
PTSOUT		equ 20
COLBIT0		equ 24
COLBIT1		equ 26
COLBIT2		equ 28
COLBIT3		equ 30
LSTLIN		equ 32
LNMASK		equ 34
WMODE		equ 36
X1		equ 38
Y1		equ 40
X2		equ 42
Y2		equ 44
PATPTR		equ 46
PATMSK		equ 50
MFILL		equ 52
CLIP		equ 54
XMINCL		equ 56
YMINCL		equ 58
XMAXCL		equ 60
YMAXCL		equ 62
XDDA		equ 64
DDAINC		equ 66
SCALDIR		equ 68
MONO		equ 70
SRCX		equ 72
SRCY		equ 74
DSTX		equ 76
DSTY		equ 78
DELX		equ 80
DELY		equ 82
FBASE		equ 84
FWIDTH		equ 88
STYLE		equ 90
LITEMSK		equ 92
SKEWMSK		equ 94
WEIGHT		equ 96
ROFF		equ 98
LOFF		equ 100
SCALE		equ 102
CHUP		equ 104
TEXTFG		equ 106
SCRTCHP		equ 108
SCRPT2		equ 112
TEXTBG		equ 114
COPYTRAN	equ 116
SEEDABORT	equ 118


* "Line A" macros
* =======================================================================

A_init	macro
	dc.w	$a000
	endm
A_putpix macro
	dc.w	$a001
	endm
A_getpix macro
	dc.w	$a002
	endm
A_abline macro
	dc.w	$a003
	endm
A_habline macro
	dc.w	$a004
	endm
A_rectfill macro
	dc.w	$a005
	endm
A_polyfill macro
	dc.w	$a006
	endm
A_bitblt macro
	dc.w	$a007
	endm
A_textblt macro
	dc.w	$a008
	endm
A_showcur macro
	dc.w	$a009
	endm
A_hidecur macro
	dc.w	$a00a
	endm
A_chgcur macro
	dc.w	$a00b
	endm
A_drsprite macro
	dc.w	$a00c
	endm
A_unsprite macro
	dc.w	$a00d
	endm
A_copyrstr macro
	dc.w	$a00e
	endm
A_seedfill macro
	dc.w	$a00f
	endm


* OS macros
* =======================================================================

;--- trap to BIOS
 .macro
 Bios trpno,clean
	move.w	#trpno,-(sp)
	trap	#13
    .ifgt clean-8
	lea	clean(sp),sp
    .else
	addq	#clean,sp
    .endif
 .endm

;--- trap to XBIOS
 .macro
 Xbios trpno,clean
	move.w	#trpno,-(sp)
	trap	#14
    .ifgt clean-8
	lea	clean(sp),sp
    .else
	addq	#clean,sp
    .endif
 .endm

;--- trap to GEMDOS
 .macro
 Gemdos trpno,clean
	move.w	#trpno,-(sp)
	trap	#1
    .ifgt clean-8
	lea	clean(sp),sp
    .else
	addq	#clean,sp
    .endif
 .endm

;--- define & use a string
;    String "foobar"		(push onto stack)
;    String "foobar",a0		move address of string somewhere
 .macro
 String str,loc
	csect data,1
string\@: dc.b "str",0
	csect text,0
    .ifgt \#-1
	move.l	#string\@,loc
    .else
	pea	string\@
    .endif
 .endm


* BIOS traps
* =======================================================================

;--- character device numbers
PRT	equ 0	; printer
AUX	equ 1	; RS-232
CON	equ 2	; console (vt-52 emulator)
MIDI	equ 3	; MIDI port
IKBD	equ 4	; ikbd (out only)
RAWCON	equ 5	; console (raw characters)
;---
 .macro
 Getmbp ptr
	move.l	ptr,-(sp)
	Bios	0,4
 .endm
;---
 .macro
 Bconstat dev
	move.w	dev,-(sp)
	Bios	1,4
 .endm
;---
 .macro
 Bconin dev
	move.w	dev,-(sp)
	Bios	2,4
 .endm
;---
 .macro
 Bconout dev,char
	move.w	char,-(sp)
	move.w	dev,-(sp)
	Bios	3,6
 .endm
;---
 .macro
 Rwabs rwflag,buf,count,recno,dev
	move.w	dev,-(sp)
	move.w	recno,-(sp)
	move.w	count,-(sp)
	move.l	buf,-(sp)
	move.w	rwflag,-(sp)
	Bios	4,14
 .endm
;---
 .macro
 Setexc vecnum,vec
	move.l	vec,-(sp)
	move.w	vecnum,-(sp)
	Bios	5,8
 .endm
;---
 .macro
 Tickcal
	Bios	6,2
 .endm
;---
 .macro
 Getbpb devno
	move.w	devno,-(sp)
	Bios	7,4
 .endm
;---
 .macro
 Bcostat devno
	move.w	devno,-(sp)
	Bios	8,4
 .endm
;---
 .macro
 Mediach devno
	move.w	devno,-(sp)
	Bios	9,4
 .endm
;---
 .macro
 Drvmap
	Bios	10,2
 .endm
;---
 .macro
 Kbshift mode
	move.w	mode,-(sp)
	Bios	11,4
 .endm


* Extended BIOS traps
* =======================================================================

 .macro
 Initmous type,param,vec
	move.l	vec,-(sp)
	move.l	param,-(sp)
	move.w	type,-(sp)
	Xbios	0,12
 .endm
;---
 .macro
 Physbase
	Xbios	2,2
 .endm
;---
 .macro
 Logbase
	Xbios	3,2
 .endm
;---
 .macro
 Getrez
	Xbios	4,2
 .endm
;---
 .macro
 Setscreen logloc,physloc,rez
	move.w	rez,-(sp)
	move.l	physloc,-(sp)
	move.l	logloc,-(sp)
	Xbios	5,12
 .endm
;---
 .macro
 Setpalette ptr
	move.l	ptr,-(sp)
	Xbios	6,6
 .endm
;---
 .macro
 Setcolor colnum,color
	move.w	color,-(sp)
	move.w	colnum,-(sp)
	Xbios	7,6
 .endm
;---
 .macro
 Floprd buf,devno,sectno,trackno,sideno,count
	move.w	count,-(sp)
	move.w	sideno,-(sp)
	move.w	trackno,-(sp)
	move.w	sectno,-(sp)
	move.w	devno,-(sp)
	clr.l	-(sp)	; (filler)
	move.l	buf,-(sp)
	Xbios	8,20
 .endm
;---
 .macro
 Flopwr buf,devno,sectno,trackno,sideno,count
	move.w	count,-(sp)
	move.w	sideno,-(sp)
	move.w	trackno,-(sp)
	move.w	sectno,-(sp)
	move.w	devno,-(sp)
	clr.l	-(sp)	; (filler)
	move.l	buf,-(sp)
	Xbios	9,20
 .endm
;---
 .macro
 Flopfmt buf,devno,spt,trackno,sideno,interlv,magic,virgin
	move.w	virgin,-(sp)
	move.l	magic,-(sp)
	move.w	interlv,-(sp)
	move.w	sideno,-(sp)
	move.w	trackno,-(sp)
	move.w	spt,-(sp)
	move.w	devno,-(sp)
	clr.l	-(sp)	; (filler)
	move.l	buf,-(sp)
	Xbios	$a,26
 .endm
;---
 .macro
 Midiws cnt1,ptr
	move.l	ptr,-(sp)
	move.w	cnt1,-(sp)
	Xbios	$c,8
 .endm
;---
 .macro
 Random
	Xbios	$11,2
 .endm
;---
 .macro
 Protobt buf,serialno,disktype,execflag
	move.w	execflag,-(sp)
	move.w	disktype,-(sp)
	move.l	serialno,-(sp)
	move.l	buf,-(sp)
	Xbios	$12,14
 .endm
;---
 .macro
 Flopver buf, devno, sectno, trackno, sideno, count
	move.w	count,-(sp)
	move.w	sideno,-(sp)
	move.w	trackno,-(sp)
	move.w	sectno,-(sp)
	move.w	devno,-(sp)
	clr.l	-(sp)	; (filler)
	move.l	buf,-(sp)
	Xbios	$13,20
 .endm
;---
CURS_HIDE	equ 0
CURS_SHOW	equ 1
CURS_BLINK	equ 2
CURS_NOBLINK	equ 3
CURS_SETRATE	equ 4
CURS_GETRATE	equ 5

 .macro
 Cursconf func,op
	move.w	op,-(sp)
	move.w	func,-(sp)
	Xbios	$15,6
 .endm
;---
 .macro
 Settime datetime
	move.l	datetime,-(sp)
	Xbios	$16,6
 .endm
;---
 .macro
 Gettime
	Xbios	$17,2
 .endm
;---
 .macro
 Kbdvbase
	Xbios	$22,2
 .endm
;---
 .macro
 Vsync
	Xbios	$24,2
 .endm
;---
 .macro
 Supexec where
	move.l	where,-(sp)
	Xbios	$26,6
 .endm


* GEMDOS traps
* =======================================================================

 .macro
 Dsetdrv drive
	move.w	drive,-(sp)
	Gemdos	$e,4
 .endm
;---
 .macro
 Pterm0
	clr.w	(sp)
	trap	#1
	illegal
 .endm
;---
 .macro
 Cconin
	Gemdos	1,2
 .endm
;---
 .macro
 Cconout char
	move.w #char,-(sp)
	Gemdos 2,4
 .endm
;---
 .macro
 Cconws str
	String	str
	Gemdos	$9,6
 .endm
;---
 .macro
 Pterm code
    .ifgt \#
	move.w	code,-(sp)
    .else
	clr.w	-(sp)
    .endif
	move.w	#$4c,-(sp)
	trap	#1
	illegal
 .endm
;---
 .macro
 Super
	clr.l	-(sp)
	move.w	#$20,-(sp)
	trap	#1
	addq	#6,sp
	move.l	d0,-(sp)
	move.w	#$20,-(sp)
 .endm
;---
 .macro
 User
	trap	#1
	addq	#6,sp
 .endm
;---
 .macro
 Puts string
	move.l	string,-(sp)
	Gemdos	9,6
 .endm
;---
 .macro
 Tgetdate
	Gemdos	$2a,2
 .endm
;---
 .macro
 Tsetdate date
	move.w	date,-(sp)
	Gemdos	$2b,4
 .endm
;---
 .macro
 Tgettime
	Gemdos	$2c,2
 .endm
;---
 .macro
 Tsettime time
	move.w	time,-(sp)
	Gemdos	$2d,4
 .endm
;---
 .macro
 Fgetdta
	Gemdos	$2f,0
 .endm
;---
 .macro
 Sversion
	Gemdos	$30,2
 .endm
;---
 .macro
 Dfree buf,drive
	move.w	drive,-(sp)
	move.l	buf,-(sp)
	Gemdos	$36,8
 .endm
;---
FO_READ		equ 0
FO_WRITE	equ 1
FO_RW		equ 2

 .macro
 Fopen file,mode
	move.w	mode,-(sp)
	move.l	file,-(sp)
	Gemdos	$3d,8
 .endm
;---
 .macro
 Fcreate file,mode
	move.w	mode,-(sp)
	move.l	file,-(sp)
	Gemdos	$3c,8
 .endm
;---
 .macro
 Fread handle,count,buf
	move.l	buf,-(sp)
	move.l	count,-(sp)
	move.w	handle,-(sp)
	Gemdos	$3f,12
 .endm
;---
 .macro
 Fwrite handle,count,buf
	move.l	buf,-(sp)
	move.l	count,-(sp)
	move.w	handle,-(sp)
	Gemdos	$40,12
 .endm
;---
 .macro
 Fclose handle
	move.w	handle,-(sp)
	Gemdos	$3e,4
 .endm
;---
 .macro
 Fdelete file
	move.l	file,-(sp)
	Gemdos	$41,6
 .endm
;---
 .macro
 Fseek offset,handle,mode
	move.w	mode,-(sp)
	move.w	handle,-(sp)
	move.l	offset,-(sp)
	Gemdos	$42,10
 .endm
;---
 .macro
 Fsetdta addr
	move.l	addr,-(sp)
	Gemdos	$1a,6
 .endm
;---
 .macro
 Ptermres count,retcode
    .ifgt \#-1
	move.w	retcode,-(sp)
    .else
	clr.w	-(sp)
    .endif
	move.l	count,-(sp)
	move.w	#$31,-(sp)
	trap	#1
	illegal
 .endm
;---
 .macro
 Mshrink block,newsiz
	move.l	newsiz,-(sp)
	move.l	block,-(sp)
	clr.w	-(sp)
	Gemdos	$4a,12
 .endm
;---
 .macro
 Malloc amount
	move.l	amount,-(sp)
	Gemdos	$48,6
 .endm
;---
 .macro
 Mfree addr
	move.l	addr,-(sp)
	Gemdos	$49,6
 .endm
;---
 .macro
 Pexec mode,p1,p2,p3
	move.l	p3,-(sp)
	move.l	p2,-(sp)
	move.l	p1,-(sp)
	move.w	mode,-(sp)
	Gemdos	$4b,16
 .endm
;---
 .macro
 Fsfirst fspec,attr
	move.w	attr,-(sp)
	move.l	fspec,-(sp)
	Gemdos	$4e,8
 .endm
;---
 .macro
 Fsnext
	Gemdos	$4f,2
 .endm


* Structures
* =======================================================================

;--- Basepage Format
		.ABS 0
Plowtpa:	ds.l 1		; -> base of TPA
Phitpa:		ds.l 1		; -> top of TPA +1
Ptbase:		ds.l 1		; -> text segment
Ptlen:		ds.l 1		; text segment size
Pdbase:		ds.l 1		; -> data segment
Pdlen:		ds.l 1		; data segment size
Pbbase:		ds.l 1		; -> BSS segment
Pblen:		ds.l 1		; BSS segment size
Pxdta:		ds.l 1		; -> current DTA
Pparent:	ds.l 1		; -> parent basepage
Prsv1:		ds.l 1
Penv:		ds.l 1		; -> environment
Prsv2:		ds.l 20
Pcmdlin:	ds.b 128	; command line
sizeof_BP:

;--- Searchfirst/Searchnext DTA structure
		.ABS 0
		ds.b 21
DTA_attr:	ds.b 1
DTA_time:	ds.w 1
DTA_date:	ds.w 1
DTA_size:	ds.l 1
DTA_name:	ds.b 14
sizeof_DTA:

;--- OS header pointed to by _sysbase
		.ABS 0
OS_entry:	ds.w 1		; $00	BRA to reset handler
OS_version:	ds.w 1		; $02	TOS version number
OS_reseth:	ds.l 1		; $04	-> system reset handler
OS_beg:		ds.l 1		; $08	-> base of OS
OS_end:		ds.l 1		; $0c	-> end of BIOS/GEMDOS/VDI memory usage
OS_rsv1:	ds.l 1		; $10	reserved
OS_magic:	ds.l 1		; $14	-> GEM MPB (or NULL)
OS_date:	ds.l 1		; $18	date of system build ($DDMMYYYY)
OS_conf:	ds.w 1		; $1c	OS configuration bits
OS_dosdate:	ds.w 1		; $1e	DOS-format date the system was built

**** the following are available in TOS >= 1.2
OS_root:	ds.l 1		; $20	-> base of OS pool
OS_kbshift:	ds.l 1		; $24	-> keyboard shift state variable
OS_run:		ds.l 1		; $28	-> GEMDOS PID of current process
OS_rsv2:	ds.l 1		; $2c	reserved
sizeof_OS:

	.TEXT
	endc
