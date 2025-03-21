# build with gputils
# https://gputils.sourceforge.io/

TARGETS =	eiffel_1.10.2-raven_A1 \
			eiffel_1.10.2-raven_A2 \
			#eiffel_1.10.1-Mq_fixedFire_noLcd \
			#

DEPS = eiffel.inc Makefile

.PHONY: all clean

all: $(TARGETS:=.hex)

eiffel_1.10.2-raven_A1.hex : eiffel_1.10.2-raven.asm $(DEPS)
	gpasm -D MHZ_8=0 -D BAUDRATE=BAUD_7812_5 -D RAVEN=0xA1 -o $@ $< 

eiffel_1.10.2-raven_A2.hex : eiffel_1.10.2-raven.asm $(DEPS)
	gpasm -D MHZ_8=1 -D BAUDRATE=BAUD_31250 -D RAVEN=0xA2 -o $@ $< 

%.hex: %.asm $(DEPS)
	gpasm -o $@ $<

clean:
	rm -f $(TARGETS:=.hex) $(TARGETS:=.lst) $(TARGETS:=.cod)
