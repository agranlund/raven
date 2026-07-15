A56DIR = asm56k
A56 = $(A56DIR)/build/asm56k
AFLAGS = -c 56301

DEPS = asm56k dsp56303.inc

OBJS = boot.p56 test.p56

.PHONY: $(A56) clean

all: asm56k $(OBJS)

asm56k: $(A56)
	make -C $(A56DIR)

%.p56: %.asm $(DEPS)
	$(A56) $(AFLAGS) -p $(basename $@).p56 $<
	$(A56) $(AFLAGS) -k $(basename $@).c56 $<
	$(A56) $(AFLAGS) -e $(basename $@).i56 $<
	$(A56) $(AFLAGS) -s -o $(basename $@).l56 $<

distclean: clean
	make -C $(A56DIR) clean

clean:
	rm -f $(OBJS) $(OBJS:.p56=.l56) $(OBJS:.p56=.c56) $(OBJS:.p56=.i56)
