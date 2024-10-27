
SRCROOT	= ../..
DSTROOT	= $(SRCROOT)/../dist/tools
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B thingpal.prj

clean:
	rm -f *.o *.tos *.prg *.lst *.pdb *.pal *.act
	rm -f *.O *.TOS *.PRG *.LST *.PDB *.PAL *.ACT


dist: clean all
	cp thingpal.tos $(DSTROOT)/

