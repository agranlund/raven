
BINROOT	= c:/tools

SRCROOT	= ../..
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B thingpal.prj

clean:
	rm -f *.o *.tos *.prg *.lst *.pdb *.pal *.act
	rm -f *.O *.TOS *.PRG *.LST *.PDB *.PAL *.ACT

install: all
	cp thingpal.tos $(BINROOT)/
