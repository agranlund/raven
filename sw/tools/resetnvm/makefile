
BINROOT	= c:/tools

SRCROOT	= ../..
DSTROOT	= $(SRCROOT)/../dist/tools
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B resetnvm.prj

clean:
	rm -f *.O *.prg *.tos

dist: all
	cp resetnvm.prg $(DSTROOT)/

install: all
	cp resetnvm.prg $(BINROOT)/
