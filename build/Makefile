prefix = /usr/local
bindir = $(prefix)/bin

#-----------------------------------------------
#srcdir is a path to your top level "buildfiles" file
#-----------------------------------------------
srcdir = ../src
testdir = ../test

SRCS = \
	$(srcdir)/CodeUtils.c \
	$(srcdir)/ConvertFields.c \
	$(srcdir)/ErrorMessages.c \
	$(srcdir)/MacroProxy.c \
	$(srcdir)/OutputEmbededAsm.c \
	$(srcdir)/OutputEmbededC.c \
	$(srcdir)/OutputLod.c \
	$(srcdir)/OutputP56.c \
	$(srcdir)/PipeLineRestriction.c \
	$(srcdir)/StringBuffer.c \
	$(srcdir)/SymbolTable.c \
	$(srcdir)/TokenStream.c \
	$(srcdir)/Value.c \
	$(srcdir)/main.c \
	$(srcdir)/codegen/GenArith.c \
	$(srcdir)/codegen/GenBitOps.c \
	$(srcdir)/codegen/GenMisc.c \
	$(srcdir)/codegen/GenParMoves.c \
	\
	Parser.c \
	Lexer.c \
	$(empty)

#-----------------------------------------------
# compilers
#-----------------------------------------------

#EVAL_CROSS := case `uname -s` in *MiNT*) echo no;; *) echo yes; esac
#CROSS := $(shell $(EVAL_CROSS))

ifeq ($(CROSS),yes)

CROSSPREFIX=m68k-atari-mint-
CPU=-m68020-60

NATIVECC = gcc
EXE_EXT=.ttp

else

NATIVECC = $(CC)
CPU=
EXE_EXT=

endif

#-----------------------------------------------
#target file name
#-----------------------------------------------

TARGETNAME=asm56k$(EXE_EXT)


CC=$(CROSSPREFIX)gcc
CFLAGS=$(CPU) -O2 -Wall -Wstrict-prototypes -Wmissing-prototypes

LDFLAGS=-s
LD=$(CC)

LEX=flex
LEXFLAGS=-Pasm -Cem -8 

PYTHON = /usr/bin/env python

all: $(TARGETNAME)

OBJS = $(SRCS:.c=.o)

LIBS = -lm

CPPFLAGS = -I. -I$(srcdir)

$(OBJS): Parser.h

Parser.h: Parser.c

Parser.c: $(srcdir)/Parser.y
	bison -o $@ --defines=Parser.h $<

Lexer.c: $(srcdir)/Lexer.l
	flex -Pasm -Cem -8 -o $@ $<

$(TARGETNAME): $(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)



check:: $(TARGETNAME)
	@log=log.txt; \
	$(RM) $$log; \
	res=0; \
	for file in $(testdir)/*.asm; do \
		base=`basename $$file .asm`; \
		echo "./$(TARGETNAME) -I$(testdir) -o $$base.lod -p $$base.p56 $$file" >> $$log; \
		./$(TARGETNAME) -I$(testdir) -o $$base.lod -p $$base.p56 $$file >> $$log; \
		status=$$?; \
		if grep -q XFAIL $$file; then if test $$status = 0; then status=1; else status=0; fi; fi; \
		if test $$status = 0; then \
			echo "PASSED: $$file"; \
		else \
			echo "FAILED: $$file"; \
			echo "FAILED: $$file" >> $$log; \
			res=1; \
		fi \
	done; \
	exit $$res

install:: $(TARGETNAME)
	cp $(TARGETNAME) $(DESTDIR)$(bindir)


clean::
	$(RM) $(TARGETNAME) $(OBJS) Parser.c Parser.h Lexer.c *.lod *.p56 log.txt

