DEST_DIR = /usr/local/bin/

ifeq ($(CROSS),yes)
CROSSPREFIX=m68k-atari-mint-
CPU=-m68020-60
EXE_EXT=.ttp
else
CROSSPREFIX=
CPU=
EXE_EXT=
endif

CC=$(CROSSPREFIX)gcc
CFLAGS = $(CPU) -Os -Iinclude

TARGETS=\
	cldinfo$(EXE_EXT) \
	cldlod$(EXE_EXT) \
	cofdmp$(EXE_EXT) \
	dsp56k-strip$(EXE_EXT) \
	lod2p56$(EXE_EXT) \
	srec$(EXE_EXT) \
	tiohist$(EXE_EXT) \
	#

all: $(TARGETS)

.PHONY: check
check:

.PHONY: clean
clean:
	$(RM) $(TARGETS)

install: $(TARGETS)
	cp $(TARGETS) $(DEST_DIR)

cldinfo$(EXE_EXT) : cldinfo.c
	$(CC) $(CFLAGS) -o $@ $<

cldlod$(EXE_EXT) : cldlod.c
	$(CC) $(CFLAGS) -o $@ $<

cofdmp$(EXE_EXT) : cofdmp.c
	$(CC) $(CFLAGS) -o $@ $<

dsp56k-strip$(EXE_EXT) : dsp56k-strip.c
	$(CC) $(CFLAGS) -o $@ $<

lod2p56$(EXE_EXT) : lod2p56.c
	$(CC) $(CFLAGS) -o $@ $<

srec$(EXE_EXT) : srec.c
	$(CC) $(CFLAGS) -o $@ $<

tiohist$(EXE_EXT) : tiohist.c
	$(CC) $(CFLAGS) -o $@ $<
