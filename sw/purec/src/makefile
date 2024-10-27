
INSTALL_BIN	= /usr/bin
INSTALL_INC	= /usr/include/purec
INSTALL_LIB	= /usr/lib/purec

all:
	mkdir -p ../lib
ifneq ($(PUREC),)
	cp -f $(PUREC)/pcc.ttp ../
	cp -f $(PUREC)/pasm.ttp ../
	cp -f $(PUREC)/plink.ttp ../
	cp -f -r $(PUREC)/lib/pcstdlib.lib ../lib/
	cp -f -r $(PUREC)/lib/pcfltlib.lib ../lib/
endif
	make -C pctoslib
	make -C pcgemlib
	make -C pclnalib
	make -C pcstart

	
clean:
	make -C pctoslib clean
	make -C pcgemlib clean
	make -C pclnalib clean
	make -C pcstart clean

distclean: clean
	rm -f ../pc.prg ../pcc.ttp ../pasm.ttp ../plink.ttp
	rm -f ../lib/*


install: all
	mkdir -p $(INSTALL_BIN)
	
	mkdir -p $(INSTALL_INC)
	mkdir -p $(INSTALL_LIB)
	
	cp -f ../pcc.ttp $(INSTALL_BIN)/pcc
	cp -f ../pasm.ttp $(INSTALL_BIN)/pasm
	cp -f ../plink.ttp $(INSTALL_BIN)/plink
	
	cp -f -r ../lib/* $(INSTALL_LIB)/
	cp -f -r ../include/* $(INSTALL_INC)/
	