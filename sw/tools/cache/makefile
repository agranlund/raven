
BINROOT	= c:/tools

SRCROOT	= ../..
DSTROOT	= $(SRCROOT)/../dist/tools
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B cacheoff.prj
	$(PCMAKE) -B cache_on.prj

clean:
	rm -f *.O *.prg *.tos

dist: all
	cp cacheoff.prg $(DSTROOT)/
	cp cache_on.prg $(DSTROOT)/
	
install: all
	cp cacheoff.prg $(BINROOT)/
	cp cache_on.prg $(BINROOT)/

