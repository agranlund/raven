
BINROOT	= c:/tools

SRCROOT	= ../..
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B cacheoff.prj
	$(PCMAKE) -B cacheon.prj

clean:
	rm -f *.O *.prg *.tos
	
install: all
	cp cacheoff.prg $(BINROOT)/
	cp cacheon.prg $(BINROOT)/

