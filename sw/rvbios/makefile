
NAME = rvbios

LOGO_BMP = rvlogo_1bpp.bmp

BIN = $(NAME).prg
PRJ = $(NAME).prj

INSTALL_ROOT = c:/auto
INSTALL_BINS = $(BIN)

OBJS = main.o xbios_c.o utils.o xbios.o eiffel.o sp060.o \
	logo.o \
	setup/setup.o setup/form_vt.o setup/misc.o \
	setup/f_nvram.o \
	setup/f_cpu.o \
	setup/f_nova.o \
	setup/f_exit.o \
	../rvnova/loader/rvnova.o

ROOTDIR = ..
include $(ROOTDIR)/Makefile.cmn

.PHONY: logo
logo: clean
	gcc genlogo.c -o genlogo.exe
	cp $(LOGO_BMP) logo.bmp
	./genlogo.exe
	xxd -i logo.bin > logo.c
	rm -f ./genlogo.exe ./logo.bmp ./logo.bin
