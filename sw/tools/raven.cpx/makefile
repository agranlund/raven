
BINROOT	= c:/cpx

SRCROOT	= ../..
DSTROOT	= $(SRCROOT)/../dist/cpx
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B ct60conf.prj

install:
	cpxlink.ttp raven.hd

clean:
	rm -f *.o *.O *.prg *.PRG *.pdb *.PDB *.cp

dist: install
	cp $(BINROOT)/raven.cpx $(DSTROOT)/

