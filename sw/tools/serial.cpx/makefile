
NAME = serial

BIN = $(NAME).bin
PRJ = $(NAME).prj

POST = cat $(NAME).hdr $(NAME).bin > $(NAME).cpx
CLEAN = $(NAME).cpx

INSTALL_ROOT = c:/raven
INSTALL_BINS = $(BIN)

OBJS = cpxstart.o serial.o

ROOTDIR = ../..
include $(ROOTDIR)/Makefile.cmn

.PHONY: header

header:
	perl genhdr.pl -o serial.hdr serial.cph
