BINROOT	= c:/tools
SRCROOT	= ../..
include $(SRCROOT)/purec/Makefile.pc
.PHONY: all clean install

all:
	$(SRCROOT)/purec/$(PCMAKE) -B thingpal.prj

clean:
	rm -f *.o *.tos *.prg *.lst *.pdb *.pal *.act
	rm -f *.O *.TOS *.PRG *.LST *.PDB *.PAL *.ACT

install: all
ifneq ($(cross),y)
	cp thingpal.tos $(BINROOT)/
endif
