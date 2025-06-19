

# base settings, should not need to change these
FREQ_SYS 		= 48000000
XRAM_SIZE 		= 0x1600
XRAM_LOC 		= 0x0200
#CODE_SIZE 		= 0xEFFF
CODE_SIZE 		= 0xE7FF

# internal or external oscillator
# todo: remove and replace with runtime detect
OSC_TYPE 		= OSC_INTERNAL
#OSC_TYPE 		= OSC_EXTERNAL


# baudrate for debug port (UART0)
BAUD_DEBUG		= 115200

# baudrate for ikbd connection (UART1)
BAUD_IKBD		= 7812

# board layout
#BOARD_TYPE  	= BOARD_PROTO
BOARD_TYPE		= BOARD_RAVEN_A1
#BOARD_TYPE		= BOARD_RAVEN_A2

# Feature defines
# To use the RAVEN_A1 board as a pure Eiffel replacement you may want to disable
# TEMP2 and POWERSW as these share the pins for Eiffel-LCD.
#  -DDISABLE_TEMPS
#  -DDISABLE_PS2
#  -DDISABLE_TEMP2
#  -DDISABLE_POWERSW

# Common defines
#  -DDEBUG


# proto : prototype pcb for raven.a1
ifeq ($(target),proto)
BOARD_TYPE = BOARD_PROTO
CONF_FLAGS = 

# a1 : standalone eiffel replacement for raven.a1
else ifeq ($(target),a1)
BOARD_TYPE = BOARD_RAVEN_A1
CONF_FLAGS = 

# a2 : raven.a2 motherboard
else ifeq ($(target),a2)
BOARD_TYPE = BOARD_RAVEN_A2
CONF_FLAGS = -DDISABLE_PS2

# default : raven.a1 board used in non-raven situations
else
BOARD_TYPE = BOARD_RAVEN_A1
CONF_FLAGS = -DISABLE_TEMP2 -DDISABLE_POWERSW
endif

ifeq ($(debug),)
debug = 0
endif
ifeq ($(debug),1)
CONF_FLAGS += -DDEBUG
endif


BUILD_DATE_HEX	:= $(shell date +%Y%m%d)
BUILD_DATE_DEC	:= $(shell printf %d 0x$(BUILD_DATE_HEX))

BOARD_FLAGS	= \
	-DFREQ_SYS=$(FREQ_SYS) \
	-D$(OSC_TYPE) \
	-D$(BOARD_TYPE) \
	$(CONF_FLAGS) \
	-DBUILD_VERSION=$(BUILD_DATE_DEC) \
	-DBAUD_DEBUG=$(BAUD_DEBUG) \
	-DBAUD_IKBD=$(BAUD_IKBD)
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

TARGET = ckbd_$(target)

OBJDIR = ./build
SRCDIR = ./src

OBJS = \
	$(OBJDIR)/main.rel \
	$(OBJDIR)/isp0.rel \
	$(OBJDIR)/isp1.rel \
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
	-I/ -I$(SRCDIR) $(BOARD_FLAGS) \
	-Wl -bCSEG=0x0800 -Wl -bCSEG_ISP=0x0400

LFLAGS := $(CFLAGS)

print-%  : ; @echo $* = $($*)

makebuilddir:
	@-mkdir build

$(OBJDIR)/isp0.rel : $(SRCDIR)/isp.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/isp1.rel : $(SRCDIR)/isp.c
	$(CC) -DISPCODE --codeseg=CSEG_ISP -c $(CFLAGS) -o $@ $<

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

.DEFAULT_GOAL := info

info:
	@echo 'usage: make <target> (debug=0/1) where target can be:'
	@echo '  proto     : Raven.A1 expansion board (proto)'
	@echo '  a1        : Raven.A1 expansion board'
	@echo '  a2        : Raven.A2 motherboard'
	@echo '  clean     : clean everything'

proto:
	make all target=proto debug=$(debug)

a1:
	make all target=a1 debug=$(debug)

a2:
	make all target=a2 debug=$(debug)


all: $(OBJDIR)/$(TARGET).bin $(OBJDIR)/$(TARGET).hex size

clean:
	-rm -f $(OBJDIR)/*
