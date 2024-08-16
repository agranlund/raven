#
# Raven ROM Monitor
#
BOARD_REV	 = 0xA1
BIOS_ROM	 = 0x03000000
BIOS_BSS	 = 0x00114000

# Tools
CC		 = m68k-atari-mint-gcc
LIBGCC		:= $(shell $(CC) --print-file-name libgcc.a)
CC_INCLUDES	 = $(dir $(LIBGCC))include

# Products
MON_BIN		 = mon.bin
MON_SREC	 = mon.s19


# Sources
SFILES		:= $(wildcard *.S)
CFILES		:= $(wildcard *.c)
OFILES		 = $(SFILES:%.S=build/%.o) $(CFILES:%.c=build/%.o)
DEPS   		 = $(MAKEFILE_LIST)

BUILD_DATE_HEX	:= $(shell date +%Y%m%d)
BUILD_DATE_DEC	:= $(shell printf %d 0x$(BUILD_DATE_HEX))

DEFS		 = -DVERSION=$(BUILD_DATE_DEC) -DREV=$(BOARD_REV)
SFLAGS		 = $(DEFS) -m68060 -D__ASM__

CFLAGS		 = $(DEFS) \
		  -m68060 \
		  -Os \
		  -std=c17 \
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

.PHONY: all mon
all: $(MON_BIN) $(MON_SREC)

folders:
	mkdir -p build

$(MON_BIN): $(OFILES) $(DEPS)
	$(CC) $(LFLAGS) $(OFILES) -o $@

$(MON_SREC): $(OFILES) $(DEPS)
	$(CC) $(LFLAGS) -Wl,--oformat,srec $(OFILES) -o $@

build/%.o : %.S $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(SFLAGS) -c $< -o $@

build/%.o : %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f build/*.o
	rm -f *.bin *.map

-include build/*.d
