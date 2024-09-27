#
# Raven ROM image makefile
#

MON_DIR			= mon
MON_IMG			= $(MON_DIR)/mon.bin

EMUTOS_DIR		= emutos
EMUTOS_IMG		= $(EMUTOS_DIR)/emutos-raven.rom

OUT_BIN_TOS		= rom_tos.bin
OUT_BIN_MON		= rom_mon.bin

DEPS			= $(MAKEFILE_LIST)

.PHONY: all tos mon clean

all: tos mon
tos: $(OUT_BIN_TOS)
mon: $(OUT_BIN_MON)

.PHONY: $(OUT_BIN_TOS)
$(OUT_BIN_TOS): $(EMUTOS_IMG) $(OUT_BIN_MON) $(DEPS)
	rm -f $@
	cat $(OUT_BIN_MON) $(EMUTOS_IMG) > $@

.PHONY: $(OUT_BIN_MON)
$(OUT_BIN_MON): $(MON_IMG) $(DEPS)
	cp $(MON_IMG) $@

$(MON_IMG): $(DEPS)
	$(MAKE) -C $(MON_DIR)

$(EMUTOS_IMG): $(DEPS)
	$(MAKE) -C $(EMUTOS_DIR) raven COUNTRY=us

clean:
	$(MAKE) -C $(MON_DIR) clean
	$(MAKE) -C $(EMUTOS_DIR) clean
	rm -f $(OUT_BIN_MON) $(OUT_BIN_TOS)

