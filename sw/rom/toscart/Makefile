#
# Raven TOS extension cartridge
#
BOARD_REV	 	= 0xA1
BIOS_ROM	 	= 0x00000000

# Tools
TOOLCHAIN	= m68k-atari-mintelf
CC		 	= $(TOOLCHAIN)-gcc
OBJCOPY		= $(TOOLCHAIN)-objcopy
LIBGCC		:= $(shell $(CC) --print-file-name libgcc.a)
CC_INCLUDES	= $(dir $(LIBGCC))include

# Products
OUT_ELF		= cart.elf
OUT_BIN		= cart.bin

# Programgs
PROGS		= rvbios.bin

# Sources
SFILES		:= cart.S
CFILES		:= $(wildcard *.c)
OFILES		= $(SFILES:%.S=%.o) $(CFILES:%.c=%.o)
DEPS		= $(MAKEFILE_LIST) cart.ld rvbios.bin

BUILD_DATE_HEX	:= $(shell date +%Y%m%d)
BUILD_DATE_DEC	:= $(shell printf %d 0x$(BUILD_DATE_HEX))

DEFS = 		-DVERSION=$(BUILD_DATE_DEC)
		   
INCLUDES	=

SFLAGS		= $(DEFS) $(INCLUDES) -m68060 -D__ASM__ -Wa,--register-prefix-optional


CFLAGS		= $(DEFS) $(INCLUDES) \
		   -m68060 -Os \
		   -std=c17 \
		   -ffreestanding \
		   -fomit-frame-pointer \
		   -fno-common \
		   -fdata-sections \
		   -ffunction-sections \
		   -Wa,--register-prefix-optional \
		   -Wall \
		   -MMD \
		   -nostdinc \
		   -I$(CC_INCLUDES)

LDFLAGS		= -nolibc \
		   	-nostartfiles \
		   	-ffreestanding \
		   	-Wl,-Map,$@.map \
			-T cart.ld \
			#

.PHONY: all
all: $(OUT_BIN) $(MON_SREC)

$(OUT_ELF): $(OFILES) $(DEPS)
	$(CC) $(LDFLAGS) $(OFILES) -o $@

$(OUT_BIN): $(OUT_ELF)
	$(OBJCOPY) -O binary --gap-fill 0xff $(OUT_ELF) $@

.PHONY: rvbios.bin
rvbios.bin:
	make -C ../../rvbios
	cp ../../rvbios/rvbios.prg rvbios.bin

%.o : %.S $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(SFLAGS) -c $< -o $@

%.o : %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.elf *.bin *.prg *.sys *.map $(OFILES) $(OFILES:%.o=%.d)

-include *.d
