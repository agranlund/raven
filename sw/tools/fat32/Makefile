
BIN			=	fat32.prg
ROOTDIR 	= 	../..

CPUFLAGS	=	-m68000

OPTS 		=	-Os -Wno-stringop-truncation \
				#

OBJS = 	main.o \
		debug.o \
		hdd.o \
		asm.o \
		fatfs/source/ff.o \
		fatfs/source/ffunicode.o \
		#


INSTALL_ROOT = c:/raven
INSTALL_BINS = $(BIN)

include $(ROOTDIR)/Makefile.cmn
