

BINROOT	= c:/auto

SRCROOT	= ..
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp
PRGFLG	= $(SRCROOT)/purec/prgflags.prg

all:
	$(PCMAKE) rvnova.prj
#	$(PRGFLG) 0xff 0x31 rvnova.prg

clean:
	rm -f *.prg *.lst *.pdb *.o setup/*.o
	rm -f *.PRG *.LST *.PDB *.O setup/*.O

install: all
	cp rvnova.prg $(BINROOT)/
