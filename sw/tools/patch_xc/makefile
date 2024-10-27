
SRCROOT	= ../..
DSTROOT	= $(SRCROOT)/../dist/tools
PCMAKE	= $(SRCROOT)/purec/pcmake.ttp

all:
	$(PCMAKE) -B patch_xc.prj

clean:
	rm -f *.o *.O *.prg *.PRG

dist: clean all
	cp patch_xc.prg $(DSTROOT)/

