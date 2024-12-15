#
# Raven ROM Monitor
#
BOARD_REV	 	= 0xA1
BIOS_ROM	 	= 0x40000000
BIOS_ROM_END	= 0x40040000
BIOS_EMU	 	= 0x00500000

# Tools
TOOLCHAIN	= m68k-atari-mintelf
CC		 	= $(TOOLCHAIN)-gcc
OBJCOPY		= $(TOOLCHAIN)-objcopy
LIBGCC		:= $(shell $(CC) --print-file-name libgcc.a)
CC_INCLUDES	= $(dir $(LIBGCC))include

# Products
MON_ELF		= mon.elf
ROM_MON		= mon.bin
MON_SREC	= mon.s19

LIBDIR	:= ../../lib

vpath %.c $(LIBDIR)
vpath %.S $(LIBDIR)

# Sources
SFILES		:= $(wildcard *.S) $(wildcard hw/*.S) $(wildcard vga/*.S)
CFILES		:= $(wildcard *.c) $(wildcard hw/*.c) $(wildcard vga/*.c) x86/x86core.c x86/x86emu.c
OFILES		= $(SFILES:%.S=%.o) $(CFILES:%.c=%.o)
DEPS		= $(MAKEFILE_LIST) mon.ld

BUILD_DATE_HEX	:= $(shell date +%Y%m%d)
BUILD_DATE_DEC	:= $(shell printf %d 0x$(BUILD_DATE_HEX))

DEFS		 = -DRAVEN_ROM \
			-DVERSION=$(BUILD_DATE_DEC) \
		   	-DREV=$(BOARD_REV) \
		   	-DBIOS_ROM=$(BIOS_ROM) \
		   	-DBIOS_EMU_MEM=$(BIOS_EMU)
		   
INCLUDES	= -I. -I../../lib

SFLAGS		= $(DEFS) $(INCLUDES) -m68060 -D__ASM__

CFLAGS		= $(DEFS) $(INCLUDES) \
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
all: $(ROM_MON) $(MON_SREC)

$(MON_ELF): $(OFILES) $(DEPS)
	$(CC) $(LDFLAGS_MON) $(OFILES) -o $@

$(ROM_MON): $(MON_ELF)
	$(OBJCOPY) -O binary --set-start $(BIOS_ROM) --pad-to $(BIOS_ROM_END) --gap-fill 0xff $(MON_ELF) $@

$(MON_SREC): $(MON_ELF)
	$(OBJCOPY) -O srec --srec-forceS3 $(MON_ELF) $@

%.o : %.S $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(SFLAGS) -c $< -o $@

%.o : %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.elf *.bin *.map *.ram *.s19 $(OFILES) $(OFILES:%.o=%.d)

-include *.d
-include hw/*.d
