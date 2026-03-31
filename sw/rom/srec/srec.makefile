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


SWDIR       := ../../..

vpath %.c $(SWDIR)
vpath %.S $(SWDIR)

LDFILE		:= $(SWDIR)/rom/srec/srec.ld
STARTFILE	:= $(SWDIR)/rom/srec/start.o

SFILES		:= $(wildcard *.S) \
            rom/mon/hw/asm.S
			
          
CFILES		:= $(wildcard *.c) \
			rom/mon/lib.c \
			rom/mon/hw/uart.c

OFILES		= $(SFILES:%.S=%.o) $(CFILES:%.c=%.o) $(EXTRA_OFILES)

DEPS		= $(MAKEFILE_LIST) $(LDFILE)

SFLAGS		= $(DEFS) -m68060 -D__ASM__

ifeq ($(DOPT),)
DOPT        = -Os
endif

ifeq ($(DCPU),)
DCPU        = -m68060
endif

CFLAGS		=   $(CPU) \
                $(DOPT) \
                -DRAVEN_ROM \
		        -std=c17 \
		        -ffreestanding \
		        -fomit-frame-pointer \
		        -fno-common \
		        -fdata-sections \
		        -ffunction-sections \
		        -Wall \
                -Wno-unused-function \
		        -MMD \
		        -nostdinc \
                -I. \
                -I$(SWDIR)/rom/mon \
                -I$(SWDIR)/lib \
		        -I$(CC_INCLUDES) \
                #-save-temps \
                #


LDFLAGS		= -nolibc -nostartfiles -ffreestanding \
		   -Wl,-Map,$@.map -T $(LDFILE)

.PHONY: all
all: clean $(OUT_SREC)

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
