BINROOT	= c:/tools
SRCROOT	= ../..
include $(SRCROOT)/purec/Makefile.pc
.PHONY: all clean install

all:
	$(SRCROOT)/purec/$(PCMAKE) -B fpu_off.prj

clean:
	rm -f *.O *.prg *.tos

install: all
ifneq ($(cross),y)
	cp fpu_off.prg $(BINROOT)/
endif
