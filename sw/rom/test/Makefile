#
# Build an uploadable test app.
#

TOOLCHAIN	 = m68k-atari-mintelf
CC		 = $(TOOLCHAIN)-gcc
OBJCOPY		 = $(TOOLCHAIN)-objcopy
LIBGCC		:= $(shell $(CC) --print-file-name libgcc.a)
CC_INCLUDES	 = $(dir $(LIBGCC))include

TEST_ELF	 = test.elf
TEST_SREC	 = test.s19

SFILES		:= $(wildcard *.S)
CFILES		:= $(wildcard *.c) \
		   ../mon/lib.c \
		   ../mon/hw/uart.c

OFILES		 = $(SFILES:%.S=build/%.o) $(CFILES:%.c=build/%.o)
DEPS   		 = $(MAKEFILE_LIST) test.ld

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
		   -I$(CC_INCLUDES) \
		   -I../mon

LDFLAGS 	 = -nolibc \
		   -nostartfiles \
		   -ffreestanding \
		   -Wl,-Map,$@.map \
		   -T test.ld

.PHONY: all
all: $(TEST_SREC)

$(TEST_ELF): $(OFILES) $(DEPS)
	$(CC) $(LDFLAGS) $(OFILES) -o $@

$(TEST_SREC): $(TEST_ELF)
	$(OBJCOPY) -O srec --srec-forceS3 $(TEST_ELF) $@

build/%.o : %.S $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(SFLAGS) -c $< -o $@

build/%.o : %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f -r build/*
	rm -f *.elf *.map *.s19

-include build/*.d
