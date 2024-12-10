#
# common makefile for srec programs
#

ifeq ($(NAME),)
NAME		= $(notdir $(CURDIR))
endif

TOOLCHAIN	= m68k-atari-mintelf
CC		 	= $(TOOLCHAIN)-gcc
OBJCOPY		= $(TOOLCHAIN)-objcopy
LIBGCC		:= $(shell $(CC) --print-file-name libgcc.a)
CC_INCLUDES	= $(dir $(LIBGCC))include

OUT_ELF		= $(NAME).elf
OUT_SREC	= $(NAME).s19

MONDIR		:= ../../mon
CRTDIR		:= ..

LDFILE		:= $(CRTDIR)/srec.ld
STARTFILE	:= $(CRTDIR)/start.o

SFILES		:= $(wildcard *.S)
			
CFILES		:= $(wildcard *.c) \
			$(MONDIR)/lib.c \
			$(MONDIR)/hw/uart.c

OFILES		= $(SFILES:%.S=%.o) $(CFILES:%.c=%.o)

DEPS		= $(MAKEFILE_LIST) $(LDFILE)

SFLAGS		= $(DEFS) -m68060 -D__ASM__

ifeq ($(DOPT),)
DOPT        = -Os
endif

ifeq ($(DCPU),)
DCPU        = -m68060
endif

CFLAGS		= $(DEFS) \
			$(DCPU) $(DOPT) -std=c17 -Wall -MMD -nostdinc \
			-ffreestanding -fomit-frame-pointer -fno-common -fdata-sections -ffunction-sections \
		   -I$(CC_INCLUDES) \
		   -I$(MONDIR)

LDFLAGS		= -nolibc -nostartfiles -ffreestanding \
		   -Wl,-Map,$@.map -T $(LDFILE)

.PHONY: all
all: $(OUT_SREC)

$(OUT_ELF): $(STARTFILE) $(OFILES) $(DEPS)
	$(CC) $(LDFLAGS) $(STARTFILE) $(OFILES) -o $@

$(OUT_SREC): $(OUT_ELF)
	$(OBJCOPY) -O srec --srec-forceS3 $(OUT_ELF) $@

%.o : %.S $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(SFLAGS) -c $< -o $@

%.o : %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.elf *.map *.s19 $(STARTFILE) $(OFILES) $(OFILES:%.o=%.d)

-include *.d
