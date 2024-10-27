PCROOT = ../..
SFILES = $(wildcard *.s)
OFILES = $(SFILES:%.s=%.o)

all: $(OFILES)

%.o: %.s
	$(PCROOT)/pasm.ttp -S -U $<
	cp $@ $(PCROOT)/lib/$@

clean:
	rm -f *.o *.O
	rm -f $(OFILES:%.o=$(PCROOT)/lib/%.o)
