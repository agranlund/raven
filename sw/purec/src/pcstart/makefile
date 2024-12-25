PCROOT = ../..
include $(PCROOT)/Makefile.pc

SFILES = $(wildcard *.s)
OFILES = $(SFILES:%.s=%.o)

all: $(OFILES)

%.o: %.s
	$(PASM) -S -U $<
	cp $@ $(PCROOT)/lib/$@

clean:
	rm -f *.o *.O
	rm -f $(OFILES:%.o=$(PCROOT)/lib/%.o)
