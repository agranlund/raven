BOARD_TYPE  	= BOARD_PROTO
#BOARD_TYPE		= BOARD_RAVEN_A1
#BOARD_TYPE		= BOARD_RAVEN_A2

BOARD_CONF		= CONF_RAVEN
#BOARD_CONF		= CONF_EIFFEL

OSC_TYPE 		= OSC_INTERNAL
#OSC_TYPE 		= OSC_EXTERNAL

FREQ_SYS 		= 48000000
XRAM_SIZE 		= 0x1600
XRAM_LOC 		= 0x0200
#CODE_SIZE 		= 0xEFFF
CODE_SIZE 		= 0xE7FF

BAUD_DEBUG		= 115200
BAUD_IKBD		= 7812

BUILD_DATE_HEX	:= $(shell date +%Y%m%d)
BUILD_DATE_DEC	:= $(shell printf %d 0x$(BUILD_DATE_HEX))


BOARD_FLAGS	= \
	-DFREQ_SYS=$(FREQ_SYS) \
	-D$(OSC_TYPE) \
	-D$(BOARD_TYPE) \
	-D$(BOARD_CONF) \
	-DBUILD_VERSION=$(BUILD_DATE_DEC) \
	-DBAUD_DEBUG=$(BAUD_DEBUG) \
	-DBAUD_IKBD=$(BAUD_IKBD) \
#    -DDISABLE_PS2 \
	#end



CC = sdcc
GCC = gcc
OBJCOPY = sdobjcopy
PACK_HEX = packihx
CH55XTOOL = ./tools/ch552tool/ch55xtool/ch55xtool.py
WCHISPCMD = ./tools/WCHISPTool_CMD/MacOS/bin/WCHISPTool_CMD
WCHISPDEV = 0x01320000

ifeq ($(FLASHTOOL),)
FLASHTOOL = $(CH55XTOOL)
endif

TARGET = chkbd

OBJDIR = ./build
SRCDIR = ./src

EXTRA_FLAGS = -DDEBUG

OBJS = \
	$(OBJDIR)/main.rel \
	$(OBJDIR)/isp.rel \
	$(OBJDIR)/ch559.rel \
	$(OBJDIR)/system.rel \
	$(OBJDIR)/settings.rel \
	$(OBJDIR)/ikbd.rel \
	$(OBJDIR)/usbhost.rel \
	$(OBJDIR)/usbdescr.rel \
	$(OBJDIR)/usbreport.rel \
	$(OBJDIR)/usbdata.rel \
	$(OBJDIR)/ps2.rel \
	$(OBJDIR)/joyport.rel \
	$(OBJDIR)/temps.rel \
	$(OBJDIR)/keyboardled.rel \
	#end

CFLAGS := -V -mmcs51 --model-large --stack-auto \
	--opt-code-speed \
	--disable-warning 85 \
	--disable-warning 110 \
	--xram-size $(XRAM_SIZE) --xram-loc $(XRAM_LOC) --code-size $(CODE_SIZE) \
	-I/ -I$(SRCDIR) $(BOARD_FLAGS) $(EXTRA_FLAGS) \
	-Wl -bCSEG=0x0800 -Wl -bCSEG_ISP=0x0400

LFLAGS := $(CFLAGS)

print-%  : ; @echo $* = $($*)

makebuilddir:
	@-mkdir build

$(OBJDIR)/isp.rel : $(SRCDIR)/isp.c
	$(CC) --codeseg=CSEG_ISP -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.rel : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/$(TARGET).ihx: makebuilddir $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) -o $(OBJDIR)/$(TARGET).ihx

$(OBJDIR)/$(TARGET).hex: $(OBJDIR)/$(TARGET).ihx
	$(PACK_HEX) $(OBJDIR)/$(TARGET).ihx > $(OBJDIR)/$(TARGET).hex

$(OBJDIR)/$(TARGET).bin: $(OBJDIR)/$(TARGET).ihx
	$(OBJCOPY) -I ihex -O binary $(OBJDIR)/$(TARGET).ihx $(OBJDIR)/$(TARGET).bin

size: $(OBJDIR)/$(TARGET).ihx
	@echo '---------- Segments ----------'
	@egrep '(ABS,CON)|(REL,CON)' $(OBJDIR)/$(TARGET).map | gawk --non-decimal-data '{dec = sprintf("%d","0x" $$2); print dec " " $$0}' | /usr/bin/sort -n -k1 | cut -f2- -d' ' | uniq
	@echo '---------- Memory ----------'
	@egrep 'available|EXTERNAL|FLASH' $(OBJDIR)/$(TARGET).mem

flash_ch55xtool: $(OBJDIR)/$(TARGET).bin
	python3 $(CH55XTOOL) -f $(OBJDIR)/$(TARGET).bin -r

flash_wchispcmd: $(OBJDIR)/$(TARGET).bin
	chmod +x $(WCHISPCMD)
	$(WCHISPCMD) -p $(WCHISPDEV) -c ./tools/wchisp.ini -o program -f $(OBJDIR)/$(TARGET).bin

flash: $(OBJDIR)/$(TARGET).bin
ifeq ($(FLASHTOOL),$(CH55XTOOL))
	make flash_ch55xtool
else
	make flash_wchisp
endif

.DEFAULT_GOAL := all
all: $(OBJDIR)/$(TARGET).bin $(OBJDIR)/$(TARGET).hex size

clean:
	-rm -f $(OBJDIR)/*
