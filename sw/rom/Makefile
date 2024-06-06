#Makefile

EMUTOS_DIR	= ../../../emutos
EMUTOS_IMG	= emutos.img
EMUTOS_ROM	= emutos-raven.rom
OUT_BIN		= rom.bin

.PHONY: tos mon_bin emutos_bin

mon: mon_bin
	cp boot.bin $(OUT_BIN)


tos: mon_bin emutos_bin
	cp boot.bin $(OUT_BIN)
	cat os.bin >> $(OUT_BIN)

mon_bin:
	cd mon && make clean
	cd mon && make
	dd if=/dev/zero ibs=1k count=256 | LC_ALL=C tr "\000" "\377" >boot.bin
	dd if=mon/mon.bin of=boot.bin conv=notrunc

emutos_bin:
	cd $(EMUTOS_DIR) && make raven UNIQUE=uk
	cp $(EMUTOS_DIR)/$(EMUTOS_IMG) tos.bin
	dd if=/dev/zero ibs=1k count=256 | LC_ALL=C tr "\000" "\377" >os.bin
	dd if=tos.bin of=os.bin conv=notrunc

clean:
	cd mon && make clean
	cd $(EMUTOS_DIR) && make clean
	rm -f boot.bin tos.bin os.bin $(OUT_BIN)

