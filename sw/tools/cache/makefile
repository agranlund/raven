BINROOT	= c:/tools
SRCROOT	= ../..
include $(SRCROOT)/purec/Makefile.pc
.PHONY: all clean install

all:
	$(SRCROOT)/purec/$(PCMAKE) -B cacheoff.prj
	$(SRCROOT)/purec/$(PCMAKE) -B cacheon.prj

clean:
	rm -f *.O *.prg *.tos
	
install: all
ifneq ($(cross),y)
	cp cacheoff.prg $(BINROOT)/
	cp cacheon.prg $(BINROOT)/
endif
