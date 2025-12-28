#
# Raven ROM image makefile
#

MON_DIR			= mon
MON_IMG			= $(MON_DIR)/mon.bin

EMUTOS_DIR		= emutos
EMUTOS_RAW		= $(EMUTOS_DIR)/emutos.img
EMUTOS_IMG		= $(EMUTOS_DIR)/emutos-raven.rom
EMUTOS_IMG_CON	= $(EMUTOS_DIR)/emucon-raven.rom
EMUTOS_IMG_DBG	= $(EMUTOS_DIR)/emudbg-raven.rom
EMUTOS_IMG_ST	= $(EMUTOS_DIR)/etos192uk.img

PRG_EMUCON		= emucon2.tos
ROM_MON			= rom_mon.bin
ROM_EMUTOS		= rom_tos.bin
ROM_EMUTOS_CON	= rom_con.bin
ROM_EMUTOS_DBG	= rom_dbg.bin

ROMDISK_DIR		= romdisk
ROMDISK_IMG		= romdisk.bin

MKROM			= ./mkrom

DEPS			= $(MAKEFILE_LIST)

all: mkrom tos mon emucon
mon: $(ROM_MON)
tos: $(ROM_EMUTOS)
tos-dbg: $(ROM_EMUTOS_DBG)
tos-con: $(ROM_EMUTOS_CON)
tos-st: $(EMUTOS_IMG_ST)
emucon: $(PRG_EMUCON)

.PHONY: $(MKROM)
$(MKROM): mkrom.c $(DEPS)
	gcc -o $(MKROM) mkrom.c

.PHONY: $(PRG_EMUCON)
$(PRG_EMUCON):
	$(MAKE) -C $(EMUTOS_DIR)/cli clean
	$(MAKE) -C $(EMUTOS_DIR)/cli
	cp $(EMUTOS_DIR)/cli/emucon2.tos $(PRG_EMUCON)

.PHONY: $(ROM_EMUTOS)
$(ROM_EMUTOS): $(MKROM) $(EMUTOS_IMG) $(MON_IMG) $(ROMDISK_IMG) $(DEPS)
	rm -f $@
	$(MKROM) $@ "BOOT" $(MON_IMG) 
	$(MKROM) $@ "ETOS" $(EMUTOS_RAW)
	$(MKROM) $@ "RDSK" $(ROMDISK_IMG)

.PHONY: $(ROM_EMUTOS_CON)
$(ROM_EMUTOS_CON): $(MKROM) $(EMUTOS_IMG_CON) $(MON_IMG) $(ROMDISK_IMG) $(DEPS)
	rm -f $@
	$(MKROM) $@ "BOOT" $(MON_IMG) 
	$(MKROM) $@ "ETOS" $(EMUTOS_RAW)
	$(MKROM) $@ "RDSK" $(ROMDISK_IMG)

.PHONY: $(ROM_EMUTOS_DBG)
$(ROM_EMUTOS_DBG): $(MKROM) $(EMUTOS_IMG_DBG) $(MON_IMG) $(ROMDISK_IMG) $(DEPS)
	rm -f $@
	$(MKROM) $@ "BOOT" $(MON_IMG) 
	$(MKROM) $@ "ETOS" $(EMUTOS_RAW)
	$(MKROM) $@ "RDSK" $(ROMDISK_IMG)

.PHONY: $(ROM_MON)
$(ROM_MON): $(MKROM) $(MON_IMG) $(DEPS)
	rm -f $@
	$(MKROM) $@ "BOOT" $(MON_IMG)




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
	make -C ../tools/mon
	cp ../tools/mon/mon.tos $(ROMDISK_DIR)/files/
	cp ../tools/mon/readme.txt $(ROMDISK_DIR)/files/docs/mon.txt
	make -C ../tools/ymodem
	cp ../tools/ymodem/ymodem.tos $(ROMDISK_DIR)/files/
	cp ../tools/ymodem/readme.txt $(ROMDISK_DIR)/files/docs/ymodem.txt
	make -C ../tools/uiptools
	cp ../tools/uiptools/uip.tos $(ROMDISK_DIR)/files/
	cp ../tools/uiptools/uip.txt $(ROMDISK_DIR)/files/docs/uip.txt
	$(ROMDISK_DIR)/romdisk.sh --diskfile $@ $(ROMDISK_DIR)/files/*

clean:
	$(MAKE) -C $(MON_DIR) clean
	$(MAKE) -C $(EMUTOS_DIR) clean
	rm -f $(ROM_MON) $(PRG_EMUCON) $(ROM_EMUTOS) $(ROM_EMUTOS_DBG) $(ROM_EMUTOS_CON) $(EMUTOS_IMG_ST) $(ROMDISK_IMG)
