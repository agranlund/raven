
SRCROOT	= ../..
DSTROOT	= $(SRCROOT)/../dist/tools
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B cacheoff.prj
	$(PCMAKE) -B cache_on.prj

clean:
	rm -f *.O *.prg *.tos

dist: clean all
	cp cacheoff.prg $(DSTROOT)/
	cp cache_on.prg $(DSTROOT)/
	
