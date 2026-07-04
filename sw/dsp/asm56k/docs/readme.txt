THIS IS WORK IN PROGRESS...

Why and what ?
==============

This is a rewritten version of a DSP assembler, based on work from
sqward, which in turn based on work from Quinn C. Jensen.

It fixes quite some build problems in newer environments, removes to the
of python so it can easier be natively compiled on Atari, and also
fixes a few bugs i found.

Thorsten Otto, Sep 2018

(original statement from sqward)

This is a new partially rewritten version of DSP assembler I wrote almost 10 years ago for Deesse project.
I've undig dig this code on my harddrive some time ago and upon closer inspecion it turned out to be very 
low quality. I decided to release it and hence the rewrite. It's supposed to be a cross assembler that 
targets primarily Falcon's DSP. There's some support for 56300 series too. 

This is an a beta version so stability may be low. I'll try to fix all the bugs and implement all the 
feature requests I may get. 


Licence
=======

Dunno.. Maybe MIT or BSD ?


Stuff I use in this programm:
=============================

Lexer was base on similar assembler done by Quinn C. Jensen, here the copyright:

/*
 * Copyright (C) 1990-1992 Quinn C. Jensen
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  The author makes no representations
 * about the suitability of this software for any purpose.  It is
 * provided "as is" without express or implied warranty.
 *
 */


 
Building
========

You have 2 options, depending on your platform:

1. Unix:
Go to build folder and type “make”. This should build you a version as long as you have a gcc installed. 

To install type
sudo make install

(Skip sudo for cygwin..)

If you have a gcc cross compiler then you can crosscompile a version that runs on a real atari.
To do so, go to build and type "make CROSS=yes"

2. Windows:

I strongly suggest you should use cygwin to do all your cross development. But if you really need, you can
build it with Visual Studio (I use it only for debuging). There's a sln file included with the code. You'll
need GNU tools for win32 installed (flex, bison and m4).

Testing
=======

Go to “build” folder and type “make check”. It will run some tests and if anything fails you should 
debug it and send me a path :)

Known bugs
==========
include statements inside conditionals don't work as expected, the file will
be included regardless of the result of the conditional expression
(that also causes the test_include_fail.asm test to fail).

Invoking
========

Usage is very simple, basic pattern is like this: 

asm56k [options] input-file

This commandline options are supported:

 input-file           <STRING>        File to process

 -s                   <BOOL>          Output symbols.
 -o --output-file     <BOOL>          Output in LOD format. (default)
 -p --p56-file        <BOOL>          Output in P56 format.
 -e --embed-file      <BOOL>          Output as P56 in 68k source file.
 -D                   <STRING>        Define a symbol: 		[ -Dsymbolname[=val] ]
 -I                   <STRING>        Add include path: 	[ -Ipath ]

example:

 ./asm56k.exe -I../../test/ -o test.lod ../../test/test_data_3.asm


Supported DSPs
==============

This assembler was written with 56301 in mind, but whe Deesse project died there was no need for 56301 support
anymore. Nevertheles, 56301 instructions are still supported. Current focus is to provide a good quality cross
assembler for Falcon development.

There will be a command line switch to set a cpu type. At the moment 56301 instructions are enabled by default.
If there's no interest in 56301 I may consider removing corresponding code.
 
Syntax And Stuff
================

These assembler directives are supported:

---------------------------------------------------------------------------------------------------------------

ORG mem_space - creates new section. It takes memory location as an argument. For instance:
		
		org	x:$0
		org y:10
		org	l:$1000
		org p:$40
---------------------------------------------------------------------------------------------------------------

ALIGN val 		- aligns  offset in current section (akka pc) by the value of "val". 
---------------------------------------------------------------------------------------------------------------

DC 				- can take one or many comma separated value that will be put in your code
---------------------------------------------------------------------------------------------------------------

DS val 			- reserves val number of words
---------------------------------------------------------------------------------------------------------------

DSM val 		- reserves val number of words for a ring buffer. If you want to create a ring buffer with
				  predefined data, then use ALIGN and DC
---------------------------------------------------------------------------------------------------------------

IF val1 rel val2 - evaluates the condition and depending on that it will remove or not the code that
					follows up until ENDC
---------------------------------------------------------------------------------------------------------------

IFNDEF
---------------------------------------------------------------------------------------------------------------

IFDEF

---------------------------------------------------------------------------------------------------------------

ELSE
---------------------------------------------------------------------------------------------------------------
ENDC
---------------------------------------------------------------------------------------------------------------
MACRO
---------------------------------------------------------------------------------------------------------------
ENDM
---------------------------------------------------------------------------------------------------------------
END
---------------------------------------------------------------------------------------------------------------
MSG
---------------------------------------------------------------------------------------------------------------
ERROR
---------------------------------------------------------------------------------------------------------------
FAILE
---------------------------------------------------------------------------------------------------------------
INCLUDE
---------------------------------------------------------------------------------------------------------------


More documentation to come. For the moment have a look at the tests.

Labels
======

If you label starts from the begining of the line then it doens't have to be appended with colon.
If it does not start at the new beginning of the line it has to be appended with colon.
Local labels can be defined with '_' or '.' or '@' prefix. A global label will invalidate all
previously defined local labels, for example:

_local:	move #10,x0		
		rts

		jsr _local			; this will call the function above
		
a_global:
		
		rts

_local:	move #10,x0		
		rts

		jsr _local			; this will call the function above but not the very first one.

These are valid label examples:

label1:		dc 1				
label2		dc 1				; must be at the beginning of the line
			label1:	dc 1		; still valid as long as postfixed with colon!
		

Expressions and numbers
=======================

Assembler supports float and fractional arithmetics. Refer to "test_math.asm" for examples.
More math stuff (like trigonometry) to come in the furure (and if someone requires that).

Assembler tries to be strict with types. Therefore there's a bunch of "casting" functions
that you can use. For example:

constant_float:		equ	0.4f			; defines a float
constant_fract:		equ	0.5				// defines fractional

val_1:				equ	constant_float + float( constant_fract )		// float(x) will convert fixedpoint number to float

start:		move	#  val_1 , x0								; if value will be float or fixed point, assembler will try to store it as fixed point
			move	#  int ( val_1 + 0.1f), y1					; to force conversion to integer you have to use int(x) function 

As you can see int(x) is doing the cast. There's also a float(x) and fract(x).


You can use single quotes in the expressions to turn that into int. Like that:

	move	#'dom',x0
	
This will put a 0x646F6D in X0.

