CROSS_PREFIX = m68k-atari-mint-

CC = $(CROSS_PREFIX)gcc
AS = $(CROSS_PREFIX)gcc
LD = $(CC)

WARNINGS = -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes
# Use -DDISABLE_NATFEATS=1 to disable NATFEATS at compile time.
CFLAGS = -Os -fomit-frame-pointer $(WARNINGS) # -DDISABLE_NATFEATS=1
LDFLAGS = -s

ifeq ($(CROSS_PREFIX),)
EXE_EXT=
else
EXE_EXT=.ttp
endif

all: pcmake$(EXE_EXT)


OBJS = \
	pcmake.o \
	prj.o \
	exec.o \
	asopts.o \
	ccopts.o \
	ldopts.o \
	utils.o \
	list.o \
	getopt.o \
	$(empty)

pcmake$(EXE_EXT): $(OBJS)
	$(LD) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

$(OBJS): pcmake.h memdebug.h list.h getopt.h warnings.h

clean::
	rm -f *.o *.pdb pcmake *.ttp
