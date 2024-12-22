
BINROOT	= c:/auto

SRCROOT	= ..
PCROOT = $(SRCROOT)/purec
include $(PCROOT)/Makefile.pc

all:
	$(PCROOT)/$(PCMAKE) rvbios.prj
#	$(PCROOT)/$(PRGFLG) 0xff 0x31 rvbios.prg

clean:
	rm -f *.prg *.lst *.pdb *.o setup/*.o
	rm -f *.PRG *.LST *.PDB *.O setup/*.O

install: all
ifneq ($(cross),y)
	cp rvbios.prg $(BINROOT)/rvbios.prg
endif
