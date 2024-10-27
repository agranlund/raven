
SRCROOT	= ../..
DSTROOT	= $(SRCROOT)/../dist/tools
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B resetnvm.prj

clean:
	rm -f *.O *.prg *.tos

dist: clean all
	cp resetnvm.prg $(DSTROOT)/

