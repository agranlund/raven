PCROOT = ../..
CFILES = $(wildcard *.c)
SFILES = $(wildcard *.s)
OFILES = $(CFILES:%.c=%.o) $(SFILES:%.s=%.o)


$(PCROOT)/lib/pctoslib.lib: $(OFILES)
	$(PCROOT)/plink.ttp -J -G -O=$@ $(OFILES)


%.o: %.s
	$(PCROOT)/pasm.ttp -I$(PCROOT)/include/ -S $<


%.o: %.c
	$(PCROOT)/pcc.ttp -I$(PCROOT)/include/ -E1 -S $< 


nf_ops.o : nf_ops.c
	$(PCROOT)/pcc.ttp -I$(PCROOT)/include/ -E1 $<

clean:
	rm -f *.o *.lib *.pdb *.O *.LIB *.PDB $(PCROOT)/lib/pctoslib.lib

