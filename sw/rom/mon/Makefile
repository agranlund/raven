#
# Raven ROM Monitor
#
BOARD_REV	 = 0xA1
BIOS_ROM	 = 0x40000000
BIOS_ROM_END	 = 0x40040000
BIOS_BSS	 = 0x00518000
BIOS_EMU	 = 0x00510000
BIOS_RCTEXT	 = 0x005E0000
BIOS_RCBSS	 = 0x005FF000

# Tools
#TOOLCHAIN	 = m68k-elf
TOOLCHAIN	 = m68k-atari-mintelf
CC		 = $(TOOLCHAIN)-gcc
OBJCOPY		 = $(TOOLCHAIN)-objcopy
LIBGCC		:= $(shell $(CC) --print-file-name libgcc.a)
CC_INCLUDES	 = $(dir $(LIBGCC))include

# Products
MON_ELF		 = mon.elf
MON_BIN		 = mon.bin
MON_SREC	 = mon.s19

# Sources
SFILES		:= $(wildcard *.S) $(wildcard hw/*.S)
CFILES		:= $(wildcard *.c) $(wildcard hw/*.c)
OFILES		 = $(SFILES:%.S=build/%.o) $(CFILES:%.c=build/%.o)
DEPS   		 = $(MAKEFILE_LIST) mon.ld

BUILD_DATE_HEX	:= $(shell date +%Y%m%d)
BUILD_DATE_DEC	:= $(shell printf %d 0x$(BUILD_DATE_HEX))

DEFS		 = -DVERSION=$(BUILD_DATE_DEC) \
		   -DREV=$(BOARD_REV) \
		   -DBIOS_ROM=$(BIOS_ROM) \
		   -DBIOS_EMU_MEM=$(BIOS_EMU) \
		   -DCONF_ATARI
ifeq ($(target),tos)
DEFS		+= -DLAUNCH_TOS
endif

SFLAGS		 = $(DEFS) -m68060 -D__ASM__

CFLAGS		 = $(DEFS) \
		   -m68060 \
		   -Os \
		   -std=c17 \
		   -ffreestanding \
		   -fomit-frame-pointer \
		   -fno-common \
		   -fdata-sections \
		   -ffunction-sections \
		   -Wall \
		   -MMD \
		   -nostdinc \
		   -I$(CC_INCLUDES)

LDFLAGS_COMMON	 = -nolibc \
		   -nostartfiles \
		   -ffreestanding \
		   -Wl,-Map,$@.map

# XXX leave this until things are working
#		   -Wl,-gc-sections

LDFLAGS_MON	 = $(LDFLAGS_COMMON) -T mon.ld

.PHONY: all
all: $(MON_BIN) $(MON_SREC)

$(MON_ELF): $(OFILES) $(DEPS)
	$(CC) $(LDFLAGS_MON) $(OFILES) -o $@

$(MON_BIN): $(MON_ELF)
	$(OBJCOPY) -O binary --set-start $(BIOS_ROM) --pad-to $(BIOS_ROM_END) --gap-fill 0xff $(MON_ELF) $@

$(MON_SREC): $(MON_ELF)
	$(OBJCOPY) -O srec --srec-forceS3 $(MON_ELF) $@

build/%.o : %.S $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(SFLAGS) -c $< -o $@

build/%.o : %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f -r build/*
	rm -f *.elf *.bin *.map *.s19

-include build/*.d
