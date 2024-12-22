#
# $Header: d:/lc/st\default.mk,v 1.2 1993/11/08 17:48:18 AGK Exp $
#
# Default rules for HiSoft make
#
# Copyright (c) 1992 HiSoft
#

# Host information
ATARI	=
M68000	=
LATTICE	=

# Utility section
ARFLAGS	= -.
AR	= oml
MFLAGS	=
RMFLAGS	= -f
RM	= -rm

.EXPORT: MAKEFLAGS

MAKEFLAGS =

.o.a:
	${AR} ${ARFLAGS} $@ r $<

# DERCS section
DFLAGS	=
DERCS	= dercs

.hrd.c:
	${DERCS} ${DFLAGS} $* $@

.hrd.o:
	${DERCS} ${DFLAGS} $* $*.c
	${CC} -c ${CFLAGS} -o$@ $*.c
	${RM} ${RMFLAGS} $*.c

.def.c .dfn.c .rsd.c:
	wconvert $<
	${DERCS} ${DFLAGS} $* $@
	${RM} ${RMFLAGS} $*.hrd

# RCS section
COFLAGS	=
CO	= co

.c,v.c .y,v.y .l,v.l .s,v.s .h,v.h .i,v.i:
	${CO} ${COFLAGS} $<

# LEX section
LFLAGS	=
LEX	= flex

.l.c:
	${LEX} ${LFLAGS} -t $< >$@

.l,v.c:
	${CO} ${COFLAGS} $<
	${LEX} ${LFLAGS} -t $*.l >$@
	${RM} ${RMFLAGS} $*.l

.l.o:
	${LEX} ${LFLAGS} -t $< >$*.c
	${CC} -c ${CFLAGS} -o$@ $*.c
	${RM} ${RMFLAGS} $*.c

.l,v.o:
	${CO} ${COFLAGS} $<
	${LEX} ${LFLAGS} -t $*.l >$*.c
	${CC} -c ${CFLAGS} -o$@ $*.c
	${RM} ${RMFLAGS} $*.l $*.c

# YACC section
YFLAGS	=
YACC	= bison

.y.c:
	${YACC} ${YFLAGS} $<

.y,v.c:
	${CO} ${COFLAGS} $<
	${YACC} ${YFLAGS} $*.y
	${RM} ${RMFLAGS} $*.y

.y.o:
	${YACC} ${YFLAGS} $<
	${CC} -c ${CFLAGS} -o$@ $*.c
	${RM} ${RMFLAGS} $*.c

.y,v.o:
	${CO} ${COFLAGS} $<
	${YACC} ${YFLAGS} $*.y
	${CC} -c ${CFLAGS} -o$@ $*.c
	${RM} ${RMFLAGS} $*.y $*.c

# Assembler section
ASFLAGS	= -.
AS	= asm

ifeq '${AS}' 'genst2'	# GenST returns 5 if there were warnings
__AS	= -5${AS}
else
__AS	= ${AS}
endif

.s.o:
	${__AS} ${ASFLAGS} -o$@ $<

.s,v.o:
	${CO} ${COFLAGS} $<
	${__AS} ${ASFLAGS} -o$@ $*.s
	${RM} ${RMFLAGS} $*.s

.s.a:
	${__AS} ${ASFLAGS} -o$% $<
	${AR} ${ARFLAGS} $@ r $%
	${RM} ${RMFLAGS} $%

.s,v.a:
	${CO} ${COFLAGS} $<
	${__AS} ${ASFLAGS} -o$% $*.s
	${AR} ${ARFLAGS} $@ r $%
	${RM} ${RMFLAGS} $*.s $%

# C section
CFLAGS	=
CPRFLAGS=
CC	= lcc

.c.i:
	${CC} -c -pr ${CPRFLAGS} ${CFLAGS} -o$@ $<

.c,v.i:
	${CO} ${COFLAGS} $<
	${CC} -c -pr ${CPRFLAGS} ${CFLAGS} -o$@ $*.c
	${RM} ${RMFLAGS} $*.c

.c.o:
	${CC} -c ${CFLAGS} -o$@ $<

.c,v.o:
	${CO} ${COFLAGS} $<
	${CC} -c ${CFLAGS} -o$@ $*.c
	${RM} ${RMFLAGS} $*.c

.c.a:
	${CC} -c ${CFLAGS} -o$% $<
	${AR} ${ARFLAGS} $@ r $%
	${RM} ${RMFLAGS} $%

.c,v.a:
	${CO} ${COFLAGS} $<
	${CC} -c ${CFLAGS} -o$% $*.c
	${AR} ${ARFLAGS} $@ r $%
	${RM} ${RMFLAGS} $*.c $%

LDFLAGS	=
LD	= lcc

.c.ttp .c.prg .c.tos .c.app .c.gtp:
	${CC} ${CFLAGS} ${LDFLAGS} -o$@ $<

.c,v.ttp .c,v.prg .c,v.tos .c,v.app .c,v.gtp:
	${CO} ${COFLAGS} $<
	${CC} ${CFLAGS} ${LDFLAGS} -o$@ $*.c
	${RM} ${RMFLAGS} $*.c

#BASIC section
BFLAGS	=
BC	= hbasic

.bas.ttp .bas.prg .bas.tos .bas.app .bas.gtp:
	${BC} $< -F$@ ${BFLAGS}

.bas,v.ttp .bas,v.prg .bas,v.tos .bas,v.app .bas,v.gtp:
	${CO} ${COFLAGS} $<
	${BC} $*.bas -F$@ ${BFLAGS}
	${RM} ${RMFLAGS} $*.bas

SUFFIXES =	.prg .app .gtp .ttp .tos \
		.o \
		.i .i,v \
		.s .s,v \
		.c .c,v \
		.y .y,v \
		.l .l,v \
		.bas \
		.h .h,v \
		.hrd .dfn .def .rsd

.SUFFIXES: ${SUFFIXES}
