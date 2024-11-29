
BINROOT	= c:/tools
SRCROOT	= ../..
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B fpu_off.prj

clean:
	rm -f *.O *.prg *.tos

install: all
	cp fpu_off.prg $(BINROOT)/
