
BINROOT	= c:/tools

SRCROOT	= ../..
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B patch_xc.prj

clean:
	rm -f *.o *.O *.prg *.PRG

install: all
	cp patch_xc.prg $(BINROOT)/

