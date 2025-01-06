#
# Raven ROM image makefile
#

MON_DIR			= mon
MON_IMG			= $(MON_DIR)/mon.bin

EMUTOS_DIR		= emutos
EMUTOS_IMG		= $(EMUTOS_DIR)/emutos-raven.rom
EMUTOS_IMG_CON	= $(EMUTOS_DIR)/emucon-raven.rom
EMUTOS_IMG_DBG	= $(EMUTOS_DIR)/emudbg-raven.rom
EMUTOS_IMG_ST	= $(EMUTOS_DIR)/etos192uk.img


ROM_MON			= rom_mon.bin
ROM_EMUTOS		= rom_tos.bin
ROM_EMUTOS_CON	= rom_con.bin
ROM_EMUTOS_DBG	= rom_dbg.bin

ROMDISK_DIR		= romdisk
ROMDISK_IMG		= romdisk.bin

DEPS			= $(MAKEFILE_LIST)

all: tos mon
mon: $(ROM_MON)
tos: $(ROM_EMUTOS)
tos-dbg: $(ROM_EMUTOS_DBG)
tos-con: $(ROM_EMUTOS_CON)
tos-st: $(EMUTOS_IMG_ST)

.PHONY: $(ROM_EMUTOS)
$(ROM_EMUTOS): $(EMUTOS_IMG) $(ROM_MON) $(ROMDISK_IMG) $(DEPS)
	rm -f $@
	cat $(ROM_MON) $(EMUTOS_IMG) $(ROMDISK_IMG) > $@

.PHONY: $(ROM_EMUTOS_CON)
$(ROM_EMUTOS_CON): $(EMUTOS_IMG_CON) $(ROM_MON) $(ROMDISK_IMG) $(DEPS)
	rm -f $@
	cat $(ROM_MON) $(EMUTOS_IMG_CON) $(ROMDISK_IMG) > $@

.PHONY: $(ROM_EMUTOS_DBG)
$(ROM_EMUTOS_DBG): $(EMUTOS_IMG_DBG) $(ROM_MON) $(ROMDISK_IMG) $(DEPS)
	rm -f $@
	cat $(ROM_MON) $(EMUTOS_IMG_DBG) $(ROMDISK_IMG) > $@

.PHONY: $(ROM_MON)
$(ROM_MON): $(MON_IMG) $(DEPS)
	cp $(MON_IMG) $@

.PHONY: $(MON_IMG)
$(MON_IMG): $(DEPS)
	$(MAKE) -C $(MON_DIR)

.PHONY: $(EMUTOS_IMG)
$(EMUTOS_IMG): $(DEPS)
	$(MAKE) -C $(EMUTOS_DIR) raven COUNTRY=us

.PHONY: $(EMUTOS_IMG_CON)
$(EMUTOS_IMG_CON): $(DEPS)
	$(MAKE) -C $(EMUTOS_DIR) raven-con COUNTRY=us

.PHONY: $(EMUTOS_IMG_DBG)
$(EMUTOS_IMG_DBG): $(DEPS)
	$(MAKE) -C $(EMUTOS_DIR) raven-dbg COUNTRY=us

.PHONY: $(EMUTOS_IMG_ST)
$(EMUTOS_IMG_ST): $(DEPS)
	$(MAKE) -C $(EMUTOS_DIR) 192 UNIQUE=uk DEF="-DINITINFO_DURATION=0 -DSHOW_STARTUP_MESSAGE=0"

.PHONY: $(ROMDISK_IMG)
$(ROMDISK_IMG): $(wildcard $(ROMDISK_DIR)/files/*) $(ROMDISK_DIR)/romdisk.sh $(DEPS)
	rm -f $@
	$(ROMDISK_DIR)/romdisk.sh --diskfile $@ $(ROMDISK_DIR)/files/*

clean:
	$(MAKE) -C $(MON_DIR) clean
	$(MAKE) -C $(EMUTOS_DIR) clean
	rm -f $(ROM_MON) $(ROM_EMUTOS) $(ROM_EMUTOS_DBG) $(ROM_EMUTOS_CON) $(EMUTOS_IMG_ST) $(ROMDISK_IMG)
