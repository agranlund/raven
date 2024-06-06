
OUTBIN  = i2ctest.prg

EXTDIR = ../ext/

# ------------------------------------------------------
TARGET_CC		= m68k-atari-mint-gcc
NATIVE_CC		= gcc
OUTDIR			= .
EXE_STRIPX		= $(EXTDIR)/stripx/stripx
LIBCMINI_DIR	= $(EXTDIR)/libcmini
LIBCMINI_INC	= -I$(LIBCMINI_DIR)/include -I$(EXTDIR)
LIBCMINI_LIB	= -L$(LIBCMINI_DIR)/build
LIBCMINI_CRT	= $(LIBCMINI_DIR)/build/minicrt0.o
LIBCMINI_CFL	= -ffast-math -nostdlib 
LIBCMINI_LFL	= -nodefaultlibs -nostdlib -nostartfiles -lcmini
LGCC_OBJ		= $(EXTDIR)/lgcc.o
CC				= $(TARGET_CC)
STRIP 			= $(EXE_STRIPX) -v -f
# ------------------------------------------------------


OBJS =  $(LIBCMINI_CRT) \
		$(LGCC_OBJ) \
		i2c.o \
		rtc.o

CPUFLAGS = -m68000

#OPTS = -DDEBUG
OPTS = -Os

SFLAGS = 

CFLAGS = \
	$(LIBCMINI_INC) \
	$(LIBCMINI_CFL) \
	$(OPTS) \
	-std=c99 \
	-fomit-frame-pointer -fno-exceptions \
	-Wall -Wno-multichar -Wno-unused-but-set-variable -Wno-unused-variable

LFLAGS = \
	$(LIBCMINI_LIB) $(LIBCMINI_LFL) \
 	-Wl,--traditional-format \
	-Wl,-Map,mapfile


## Rules
.PHONY: clean all

all: $(OUTBIN)

$(OUTBIN): common Makefile $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $(OUTBIN)
	$(STRIP) $(OUTBIN)

clean:
	rm -f *.o
	rm -f mapfile
	rm -f $(OUTBIN)




# ------------------------------------------------------
common: $(EXE_STRIPX) $(LIBCMINI_CRT) $(LGCC_OBJ) $(JAM_OBJS)

$(EXE_STRIPX) : $(EXE_STRIPX).c
	$(NATIVE_CC) $(EXE_STRIPX).c -o $(EXE_STRIPX)

$(LIBCMINI_CRT):
	cd $(LIBCMINI_DIR) && make

$(LGCC_OBJ) : $(LGCC_OBJ:.o=.S)
	$(CC) -std=c99 -fomit-frame-pointer -fno-exceptions -Wall -m68000 -O0 -c $< -o $@

%.o : %.S
	$(CC) $(INCLUDE) $(CPUFLAGS) $(SFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(INCLUDE) $(CPUFLAGS) $(CFLAGS) -c $< -o $@

