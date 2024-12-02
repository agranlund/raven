
CPU_TARGET	= 68000
#CPU_TARGET	= 68060

ifeq ($(CPU_TARGET),68060)
CPUFLAGS	= 	-m68060
CPUOPTS		=	
CPUOBJS		=	
else
CPUFLAGS	= 	-m68000
CPUOPTS		= 	-DFPE_SUPPORT_68000 -DFPE_SUPPORT_TOS1
CPUOBJS		= 	fpe/fpu_68020.o
endif

BIN			=	fpemu.prg
ROOTDIR 	= 	../..
OPTS 		=	-Ifpe -Ifpe/include $(CPUOPTS) -O2

OBJS = 	main.o \
		$(CPUOBJS) \
		fpe/fpe_atari_asm.o \
		fpe/fpe_atari.o \
		fpe/fpu_add.o \
		fpe/fpu_calcea.o \
		fpe/fpu_cordic.o \
		fpe/fpu_div.o \
		fpe/fpu_emulate.o \
		fpe/fpu_exp.o \
		fpe/fpu_explode.o \
		fpe/fpu_fmovecr.o \
		fpe/fpu_fscale.o \
		fpe/fpu_fstore.o \
		fpe/fpu_getexp.o \
		fpe/fpu_hyperb.o \
		fpe/fpu_implode.o \
		fpe/fpu_int.o \
		fpe/fpu_log.o \
		fpe/fpu_mul.o \
		fpe/fpu_rem.o \
		fpe/fpu_sqrt.o \
		fpe/fpu_subr.o \
		fpe/fpu_trig.o \

include $(ROOTDIR)/Makefile.common

fpe/fpu_68020.o : fpe/fpu_68020.c
	$(CC) $(CFLAGS) -m68020-60 -c $< -o $@
