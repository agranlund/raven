#
# Raven bootrom + monitor
#

ROM_ADDR	= 0x40000000

BOOT_ELF	= boot.elf
BOOT_ROM	= boot.bin
BOOT_SREC	= boot.s19

TOOLCHAIN	= m68k-atari-mintelf
CC		 	= $(TOOLCHAIN)-gcc
OBJCOPY		= $(TOOLCHAIN)-objcopy
LIBGCC		:= $(shell $(CC) --print-file-name libgcc.a)
CC_INCLUDES	= $(dir $(LIBGCC))include

LIBDIR	:= ../../lib

vpath %.c $(LIBDIR)
vpath %.S $(LIBDIR)

BUILD_DATE_HEX	:= $(shell date +%Y%m%d)
BUILD_DATE_DEC	:= $(shell printf %d 0x$(BUILD_DATE_HEX))

SFILES 	:= $(wildcard *.S) $(wildcard hw/*.S) $(wildcard vga/*.S)
CFILES 	:= $(wildcard *.c) $(wildcard hw/*.c) $(wildcard vga/*.c) x86/x86core.c x86/x86emu.c
OFILES 	= $(SFILES:%.S=%.o) $(CFILES:%.c=%.o)
DEPS	= $(MAKEFILE_LIST) boot.ld

INCLUDES	=	-I. -I../../lib

LDFLAGS		= 	-nolibc -nostartfiles -ffreestanding -Wl,-Map,$@.map -T boot.ld

DEFS		= 	-DRAVEN_ROM -DVERSION=$(BUILD_DATE_DEC) -DROM_ADDR=$(ROM_ADDR)

SFLAGS		=	$(DEFS) $(INCLUDES) -m68060 -D__ASM__

CFLAGS_BASE	= 	$(DEFS) $(INCLUDES) \
				-m68060 \
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
				#

CFLAGS		= 	-Os $(CFLAGS_BASE)
CFLAGS_OPT	= 	-O3 $(CFLAGS_BASE)


.PHONY: all
all: $(BOOT_ROM) $(BOOT_SREC)

$(BOOT_ELF): $(OFILES) $(DEPS)
	$(CC) $(LDFLAGS) $(OFILES) -o $@

$(BOOT_ROM): $(BOOT_ELF)
	$(OBJCOPY) -O binary --set-start $(ROM_ADDR) --gap-fill 0xff $(BOOT_ELF) $@

$(BOOT_SREC): $(BOOT_ELF)
	$(OBJCOPY) -O srec --srec-forceS3 $(BOOT_ELF) $@

%.o : %.S $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(SFLAGS) -c $< -o $@

hw/cpu.o : hw/cpu.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_OPT) -c $< -o $@

x86/x86emu.o : x86/x86emu.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_OPT) -c $< -o $@

x86/x86core.o : x86/x86core.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_OPT) -c $< -o $@

%.o : %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.elf *.bin *.map *.ram *.s19 $(OFILES) $(OFILES:%.o=%.d)

-include *.d
-include hw/*.d
