

BINROOT	= c:/tools/eiffel

SRCROOT	= ../..
DSTROOT	= $(SRCROOT)/../dist/eiffel
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) eiffelcf.prj

clean:
	rm -f *.app *.lst *.pdb *.o
	rm -f *.APP *.LST *.PDB *.O

install: all
	cp eiffelcf.app $(BINROOT)/
	cp eiffel_g.rsc $(BINROOT)/	

dist: all
	cp eiffelcf.app $(DSTROOT)/
	cp eiffel_g.rsc $(DSTROOT)/



