
BINROOT	= c:/tools
SRCROOT	= ../..
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B resetnvm.prj

clean:
	rm -f *.O *.prg *.tos

install: all
	cp resetnvm.prg $(BINROOT)/
