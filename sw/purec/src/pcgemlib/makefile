PCROOT = ../..
CFILES = $(wildcard *.c)
SFILES = $(wildcard *.s)
OFILES = $(CFILES:%.c=%.o) $(SFILES:%.s=%.o)


$(PCROOT)/lib/pcgemlib.lib: $(OFILES)
	$(PCROOT)/plink.ttp -J -G -O=$@ $(OFILES)


%.o: %.s
	$(PCROOT)/pasm.ttp -I$(PCROOT)/include/ -DNEW_MT=0 -O$@ $<


%.o: %.c
	$(PCROOT)/pcc.ttp -I$(PCROOT)/include/ -L32 -S -DNEW_MT=0 -O$@ $< 


clean:
	rm -f *.o *.lib *.pdb *.O *.LIB *.PDB $(PCROOT)/lib/pcgemlib.lib

