TOOLCHAIN	= m68k-atari-mint
CC		= $(TOOLCHAIN)-gcc

#CPU_TARGET	= 68000
CPU_TARGET	= 68060

ifeq ($(CPU_TARGET),68000)
CPUFLAGS	=
CPUOPTS		= 	-DFPE_SUPPORT_68000 -DFPE_SUPPORT_TOS1
CPUOBJS		= 	fpu_68020.o
else
CPUFLAGS	= 	-m$(CPU_TARGET)
CPUOPTS		=	
CPUOBJS		=	
endif

CPPFLAGS = -I.. -I../include
CFLAGS = $(CPUFLAGS) $(CPUOPTS) -O2 -fomit-frame-pointer -Wall -Werror

ROOTDIR 	= 	../../../..

OBJS = \
	fpu_add.o \
	fpu_cordic.o \
	fpu_calcea.o \
	fpu_div.o \
	fpu_emulate.o \
	fpu_exp.o \
	fpu_explode.o \
	fpu_fmovecr.o \
	fpu_fscale.o \
	fpu_fstore.o \
	fpu_getexp.o \
	fpu_hyperb.o \
	fpu_implode.o \
	fpu_int.o \
	fpu_log.o \
	fpu_mul.o \
	fpu_rem.o \
	fpu_sqrt.o \
	fpu_subr.o \
	fpu_trig.o \
	$(CPUOBJS) \
	$(empty)

TEST_PROGRAMS = \
	fabs.ttp \
	facos.ttp \
	fadd.ttp \
	fasin.ttp \
	fatan.ttp \
	fatanh.ttp \
	fcmp.ttp \
	fcos.ttp \
	fcosh.ttp \
	fdiv.ttp \
	fetox.ttp \
	fetoxm1.ttp \
	fgetexp.ttp \
	fgetman.ttp \
	fint.ttp \
	fintrz.ttp \
	flog10.ttp \
	flog2.ttp \
	flogn.ttp \
	flognp1.ttp \
	fmod.ttp \
	fmul.ttp \
	fneg.ttp \
	frem.ttp \
	fscale.ttp \
	fsin.ttp \
	fsincos.ttp \
	fsinh.ttp \
	fsqrt.ttp \
	fsub.ttp \
	ftan.ttp \
	ftanh.ttp \
	ftentox.ttp \
	ftwotox.ttp \
	$(empty)


MISC_PROGRAMS = \
	facosh.ttp \
	fasinh.ttp \
	fatan2.ttp \
	fpow.ttp \
	$(empty)

all: $(TEST_PROGRAMS) runtests.sh

check::

$(OBJS): ../fpe_atari.h ../fpu_arith.h ../fpu_emulate.h Makefile


CPU_FLAGS_fpu_68020 = -m68020-60

define CC_TEMPLATE
$(basename $(1).o): ../$(basename $(1)).c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(CPU_FLAGS_$(basename $(1))) -c -o $$@ $$<

endef
$(foreach FILE,$(OBJS),$(eval $(call CC_TEMPLATE,$(FILE))))

define LD_TEMPLATE
$(basename $(1)).o: $(basename $(1)).c testdriver.h testconfig.h
$(1): $(basename $(1)).o $(OBJS) Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) -o $$@ $$< $(OBJS)
check:: $(1)
	./$(1) 2> $(basename $(1)).log

endef
$(foreach FILE,$(TEST_PROGRAMS),$(eval $(call LD_TEMPLATE,$(FILE))))

runtests.sh: Makefile
	rm -f $@
	for i in $(TEST_PROGRAMS); do echo "./$$i 2> $${i%%.*}.log" >> $@; done

clean:
	rm -f *.o *.map *.ttp *.log
