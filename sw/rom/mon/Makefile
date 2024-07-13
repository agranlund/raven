#Makefile
CC = m68k-atari-mint-gcc
VC = vc

HFILES = sys.h
SFILES = boot.S sysa.S 
CFILES = sysc.c vbr.c pmmu.c init.c monitor.c test.c
OFILES = $(SFILES:%.S=build/%.o) $(CFILES:%.c=build/%.o)
DEPS   = $(HFILES) Makefile


BOARD_REV		= 0xA1
BIOS_ROM		= 0x03000000
BIOS_BSS		= 0x00114000

BUILD_DATE_HEX := $(shell date +%Y%m%d)
BUILD_DATE_DEC := $(shell printf %d 0x$(BUILD_DATE_HEX))

DEFS = -DVERSION=$(BUILD_DATE_DEC) -DREV=$(BOARD_REV)
SFLAGS = $(DEFS) -m68060 -D__ASM__

CFLAGS = $(DEFS) -m68060 -Os \
	-std=c99 -fomit-frame-pointer -fno-common \
	-Wall \

ifeq ($(target),tos)
	CFLAGS += -DLAUNCH_TOS
endif

LFLAGS = -nostartfiles -nostdlib \
	-Wl,-Map,mon.map \
	-Wl,--oformat=binary,-Tbss=$(BIOS_BSS),-Ttext=$(BIOS_ROM),--entry=$(BIOS_ROM)
	
	
#	-Wl,-T,mon.ld

all: folders mon

folders:
	mkdir -p build
	mkdir -p build/sp060

mon: $(OFILES) $(DEPS)
	$(CC) $(LFLAGS) $(OFILES) -o $@.bin

build/%.o : %.S $(DEPS)
	$(CC) $(SFLAGS) -c $< -o $@

build/%.o : %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f build/*.o
	rm -f *.bin *.map
