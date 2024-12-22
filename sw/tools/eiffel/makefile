BINROOT	= c:/tools/eiffel
SRCROOT	= ../..
include $(SRCROOT)/purec/Makefile.pc
.PHONY: all clean install

all:
	$(SRCROOT)/purec/$(PCMAKE) eiffelcf.prj

clean:
	rm -f *.app *.lst *.pdb *.o
	rm -f *.APP *.LST *.PDB *.O

install: all
ifneq ($(cross),y)
	cp eiffelcf.app $(BINROOT)/
	cp eiffel_g.rsc $(BINROOT)/	
endif
