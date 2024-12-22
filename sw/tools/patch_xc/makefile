BINROOT	= c:/tools
SRCROOT	= ../..
include $(SRCROOT)/purec/Makefile.pc
.PHONY: all clean install

all:
	$(SRCROOT)/purec/$(PCMAKE) -B patch_xc.prj

clean:
	rm -f *.o *.O *.prg *.PRG

install: all
ifneq ($(cross),y)
	cp patch_xc.prg $(BINROOT)/
endif
