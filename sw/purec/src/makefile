
PCROOT = ..
include $(PCROOT)/Makefile.pc

PUREC		= crosstos/abin/pc

.PHONY: purec pcmake libs clean distclean
all: purec pcmake libs

purec:
ifeq ($(cross),y)
	make -C crosstos
	cp crosstos/dist/pc/m68k-atari-tos-pc-pcc ../
	cp crosstos/dist/pc/m68k-atari-tos-pc-pasm ../
	cp crosstos/dist/pc/m68k-atari-tos-pc-plink ../
else
	cp -f $(PUREC)/pcc.ttp ../
	cp -f $(PUREC)/pasm.ttp ../
	cp -f $(PUREC)/plink.ttp ../
endif

pcmake:
ifeq ($(cross),y)
	make -C pcmake CROSS_PREFIX=
	cp pcmake/pcmake ../
endif

libs:
	mkdir -p ../lib
	cp -f -r $(PUREC)/lib/pcstdlib.lib ../lib/
	cp -f -r $(PUREC)/lib/pcfltlib.lib ../lib/
	make -C pctoslib cross=$(cross)
	make -C pcgemlib cross=$(cross)
	make -C pclnalib cross=$(cross)
	make -C pcsyslib cross=$(cross)
	make -C pcstart cross=$(cross)

clean:
ifeq ($(cross),y)
	make -C crosstos clean
	make -C pcmake clean
endif
	make -C pctoslib clean
	make -C pcgemlib clean
	make -C pclnalib clean
	make -C pcsyslib clean
	make -C pcstart clean

distclean: clean
	rm -f ../pc.prg
	rm -f ../pcc.ttp
	rm -f ../pasm.ttp
	rm -f ../plink.ttp
	rm -f ../m68k-atari-tos-pc-pcc
	rm -f ../m68k-atari-tos-pc-pasm
	rm -f ../m68k-atari-tos-pc-plink
	rm -f ../pcmake
	rm -f ../lib/*
