BINROOT	= c:/auto
SRCROOT	= ..
include $(SRCROOT)/purec/Makefile.pc
.PHONY: all clean install

all:
	$(SRCROOT)/purec/$(PCMAKE) rvnova.prj
#	$(SRCROOT)/purec/$(PRGFLG) 0xff 0x31 rvnova.prg

clean:
	rm -f *.prg *.lst *.pdb *.o setup/*.o
	rm -f *.PRG *.LST *.PDB *.O setup/*.O

install: all
ifneq ($(cross),y)
	cp rvnova.prg $(BINROOT)/
endif
