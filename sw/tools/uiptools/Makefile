BIN			=	uiptools.prg

ROOTDIR		= 	../..
LIBDIR		= 	$(ROOTDIR)/lib

VPATH		=	$(LIBDIR)


ifeq ($(driver),)
	driver = rtl8019
endif

ifeq ($(driver),rtl8019)
	DRV_OPTS = -DNE2000ISA_DRIVER
	DRV_OBJS = src/uip/drivers/rtl8019/rtl8019.o
endif

ifeq ($(driver),slip)
	DRV_OPTS = -DSLIP_DRIVER
	DRV_OBJS = src/uip/drivers/slip/slip_dev.o
endif


CPUFLAGS	= 	-m68020-60
OPTS		= 	-Os -DMACHINE_RAVEN \
				-Wno-address-of-packed-member \
				-Wno-pointer-sign \
				-I. -I$(LIBDIR) -I$(LIBDIR)/../isa \
				-Isrc \
				-Isrc/uip \
				-Isrc/uip/drivers \
				-Isrc/atari_http_server \
				-Isrc/atari_ftp_server \
				-DVERSION=20250215 \
				-DWITHOUT_HTTP \
				$(DRV_OPTS) \
				#end

#				-DWITHOUT_FTP \
#				-DWITHOUT_DHCP \


OBJS	= 	\
			src/main.o \
			src/uip_arch.o \
			src/clock-arch.o \
			src/common.o \
			\
			src/uip/delay.o \
			src/uip/dhcpc.o \
			src/uip/psock.o \
			src/uip/timer.o \
			src/uip/uip_arp.o \
			src/uip/uip-fw.o \
			src/uip/uip-split.o \
			src/uip/uip.o \
			src/uip/uiplib.o \
			\
			$(DRV_OBJS) \
			\
			src/atari_http_server/httpd.o \
			src/atari_ftp_server/ftpd.o \
			\
			#end

#			src/ioredirect.o \
#			src/ioredirect-trap.o \


include $(ROOTDIR)/Makefile.cmn

