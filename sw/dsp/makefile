
A56 = asm56k/build/asm56k
AFLAGS = -c 56301

DEPS = asm56k dsp56303.inc

all: asm56k dspboot

asm56k: $(A56)
	make -C asm56k

dspboot: $(DEPS) boot.asm
	$(A56) $(AFLAGS) -k dspboot.c boot.asm
	$(A56) $(AFLAGS) -p dspboot.p56 boot.asm
	$(A56) $(AFLAGS) -o dspboot.lod boot.asm

.PHONY: $(A56) clean

distclean: clean
	make -C asm56k clean

clean:
	rm -f dspboot.c dspboot.p56
