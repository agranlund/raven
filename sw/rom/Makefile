#Makefile

EMUTOS_DIR		= emutos
EMUTOS_IMG		= emutos.img
EMUTOS_ROM		= emutos-raven.rom
OUT_BIN_TOS		= rom_tos.bin
OUT_BIN_MON		= rom_mon.bin

.PHONY: tos mon_bin emutos_bin


tos: target="tos"
tos: mon_bin emutos_bin
	cp boot.bin $(OUT_BIN_TOS)
	cat os.bin >> $(OUT_BIN_TOS)

mon: mon_bin
	cp boot.bin $(OUT_BIN_MON)

mon_bin:
	$(MAKE) -C mon clean
	$(MAKE) -C mon target=$(target)
	dd if=/dev/zero ibs=1k count=256 | LC_ALL=C tr "\000" "\377" >boot.bin
	dd if=mon/mon.bin of=boot.bin conv=notrunc

emutos_bin:
	$(MAKE) -C $(EMUTOS_DIR) raven UNIQUE=us
	cp $(EMUTOS_DIR)/$(EMUTOS_IMG) tos.bin
	dd if=/dev/zero ibs=1k count=256 | LC_ALL=C tr "\000" "\377" >os.bin
	dd if=tos.bin of=os.bin conv=notrunc

clean:
	$(MAKE) -C mon clean
	$(MAKE) -C $(EMUTOS_DIR) clean
	rm -f boot.bin tos.bin os.bin

