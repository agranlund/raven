#
# Raven ROM Monitor
#
BOARD_REV	 = 0xA1
BIOS_ROM	 = 0x40000000
BIOS_BSS	 = 0x00518000
BIOS_EMU	 = 0x00510000
BIOS_RCTEXT	 = 0x005E0000
BIOS_RCBSS	 = 0x005FF000

# Tools
CC		 = m68k-atari-mint-gcc
LIBGCC		:= $(shell $(CC) --print-file-name libgcc.a)
CC_INCLUDES	 = $(dir $(LIBGCC))include

# Products
MON_BIN		 = mon.bin
MON_SREC	 = mon.s19
MON_RAM		 = mon.ram

# Sources
SFILES		:= $(wildcard *.S) $(wildcard hw/*.S)
CFILES		:= $(wildcard *.c) $(wildcard hw/*.c)
OFILES		 = $(SFILES:%.S=build/%.o) $(CFILES:%.c=build/%.o)
DEPS   		 = $(MAKEFILE_LIST)

# Ram-sources
SFILES_RAM	:= $(wildcard ram/*.S)
CFILES_RAM	:= $(wildcard ram/*.c)
OFILES_RAM	 = $(SFILES_RAM:%.S=build/%.o) $(CFILES_RAM:%.c=build/%.o)


BUILD_DATE_HEX	:= $(shell date +%Y%m%d)
BUILD_DATE_DEC	:= $(shell printf %d 0x$(BUILD_DATE_HEX))

DEFS		 = -DVERSION=$(BUILD_DATE_DEC) -DREV=$(BOARD_REV) -DBIOS_ROM=$(BIOS_ROM) -DBIOS_EMU_MEM=$(BIOS_EMU) -DCONF_ATARI
SFLAGS		 = $(DEFS) -m68060 -D__ASM__

CFLAGS		 = $(DEFS) \
		  -m68060 \
		  -Os \
		  -std=c17 \
		  -ffreestanding \
		  -fomit-frame-pointer \
		  -fno-common \
		  -Wall \
		  -MMD \
		  -nostdinc \
		  -I$ $(CC_INCLUDES)

ifeq ($(target),tos)
CFLAGS		+= -DLAUNCH_TOS
endif

LFLAGS		 = -nostartfiles \
		   -nostdlib \
		   -Wl,-Map,mon.map \
		   -Wl,--oformat=binary,-Tbss=$(BIOS_BSS),-Ttext=$(BIOS_ROM),--entry=$(BIOS_ROM)

LFLAGS_RAM	 = -nostartfiles \
		   -nostdlib \
		   -Wl,-Map,mon.ram.map \
		   -Wl,--oformat=binary,-Tbss=$(BIOS_RCBSS),-Ttext=$(BIOS_RCTEXT),--entry=$(BIOS_RCTEXT)


.PHONY: all mon
all: $(MON_BIN) $(MON_SREC)

folders:
	mkdir -p build

$(MON_BIN): $(OFILES) $(DEPS)
	$(CC) $(LFLAGS) $(OFILES) -o $@

$(MON_RAM): $(OFILES_RAM) $(DEPS)
	$(CC) $(LFLAGS_RAM) $(OFILES_RAM) -o $@

$(MON_SREC): $(OFILES) $(DEPS)
	$(CC) $(LFLAGS) -Wl,--oformat,srec $(OFILES) -o $@

build/%.o : %.S $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(SFLAGS) -c $< -o $@

build/%.o : %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

build/_boot.o: $(MON_RAM)

clean:
	rm -f -r build/*
	rm -f *.bin *.map *.s19 *.ram

-include build/*.d
