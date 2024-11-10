#
# Raven ROM image makefile
#

MON_DIR			= mon
MON_IMG			= $(MON_DIR)/mon.bin

EMUTOS_DIR		= emutos
EMUTOS_IMG		= $(EMUTOS_DIR)/emutos-raven.rom
EMUCON_IMG		= $(EMUTOS_DIR)/emucon-raven.rom
EMUDBG_IMG		= $(EMUTOS_DIR)/emudbg-raven.rom

OUT_BIN_TOS		= rom_tos.bin
OUT_BIN_CON		= rom_con.bin
OUT_BIN_MON		= rom_mon.bin
OUT_BIN_DBG		= rom_dbg.bin

DEPS			= $(MAKEFILE_LIST)

.PHONY: dist all tos tos-dbg tos-con mon clean

all: tos mon
mon: $(OUT_BIN_MON)
tos: $(OUT_BIN_TOS)
tos-dbg: $(OUT_BIN_DBG)
tos-con: $(OUT_BIN_CON)

dist:
	make clean
	make tos-dbg
	make clean
	make tos
	make mon

.PHONY: $(OUT_BIN_TOS)
$(OUT_BIN_TOS): $(EMUTOS_IMG) $(OUT_BIN_MON) $(DEPS)
	rm -f $@
	cat $(OUT_BIN_MON) $(EMUTOS_IMG) > $@

.PHONY: $(OUT_BIN_CON)
$(OUT_BIN_CON): $(EMUCON_IMG) $(OUT_BIN_MON) $(DEPS)
	rm -f $@
	cat $(OUT_BIN_MON) $(EMUCON_IMG) > $@

.PHONY: $(OUT_BIN_DBG)
$(OUT_BIN_DBG): $(EMUDBG_IMG) $(OUT_BIN_MON) $(DEPS)
	rm -f $@
	cat $(OUT_BIN_MON) $(EMUDBG_IMG) > $@

.PHONY: $(OUT_BIN_MON)
$(OUT_BIN_MON): $(MON_IMG) $(DEPS)
	cp $(MON_IMG) $@

$(MON_IMG): $(DEPS)
	$(MAKE) -C $(MON_DIR)

$(EMUTOS_IMG): $(DEPS)
	$(MAKE) -C $(EMUTOS_DIR) raven COUNTRY=us

$(EMUCON_IMG): $(DEPS)
	$(MAKE) -C $(EMUTOS_DIR) raven-con COUNTRY=us

$(EMUDBG_IMG): $(DEPS)
	$(MAKE) -C $(EMUTOS_DIR) raven-dbg COUNTRY=us

clean:
	$(MAKE) -C $(MON_DIR) clean
	$(MAKE) -C $(EMUTOS_DIR) clean
	rm -f $(OUT_BIN_MON) $(OUT_BIN_TOS)

