BINROOT	= c:/tools
SRCROOT	= ../..
include $(SRCROOT)/purec/Makefile.pc
.PHONY: all clean install

all:
	$(SRCROOT)/purec/$(PCMAKE) -B resetnvm.prj

clean:
	rm -f *.O *.prg *.tos

install: all
ifneq ($(cross),y)
	cp resetnvm.prg $(BINROOT)/
endif
