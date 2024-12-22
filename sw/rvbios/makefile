BINROOT	= c:/auto
SRCROOT	= ..
include $(SRCROOT)/purec/Makefile.pc
.PHONY: all clean install

all:
	$(SRCROOT)/purec/$(PCMAKE) rvbios.prj
#	$(SRCROOT)/purec/$(PRGFLG) 0xff 0x31 rvbios.prg

clean:
	rm -f *.prg *.lst *.pdb *.o setup/*.o
	rm -f *.PRG *.LST *.PDB *.O setup/*.O

install: all
ifneq ($(cross),y)
	cp rvbios.prg $(BINROOT)/rvbios.prg
endif
