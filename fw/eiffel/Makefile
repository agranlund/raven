# build with gputils
# https://gputils.sourceforge.io/

TARGETS =	eiffel_1.10.2-raven_A1 \
			eiffel_1.10.2-raven_A1_ansi \
			eiffel_1.10.2-raven_A2 \
			eiffel_1.10.2-raven_A2_ansi \
			#eiffel_1.10.1-Mq_fixedFire_noLcd \
			#

DEPS = eiffel.inc Makefile

.PHONY: all clean

all: $(TARGETS:=.hex)


OPTS_RAVEN_A1 = -D RAVEN=0xA1 -D MHZ_8=0 -D BAUDRATE=BAUD_7812_5
OPTS_RAVEN_A2 = -D RAVEN=0xA2 -D MHZ_8=1 -D BAUDRATE=BAUD_31250

OPTS_LAYOUT_ISO  = -D ANSI=0
OPTS_LAYOUT_ANSI = -D ANSI=1

eiffel_1.10.2-raven_A1.hex : eiffel_1.10.2-raven.asm $(DEPS)
	gpasm $(OPTS_RAVEN_A1) $(OPTS_LAYOUT_ISO) -o $@ $< 

eiffel_1.10.2-raven_A2.hex : eiffel_1.10.2-raven.asm $(DEPS)
	gpasm $(OPTS_RAVEN_A2) $(OPTS_LAYOUT_ISO) -o $@ $< 

eiffel_1.10.2-raven_A1_ansi.hex : eiffel_1.10.2-raven.asm $(DEPS)
	gpasm $(OPTS_RAVEN_A1) $(OPTS_LAYOUT_ANSI) -o $@ $< 

eiffel_1.10.2-raven_A2_ansi.hex : eiffel_1.10.2-raven.asm $(DEPS)
	gpasm $(OPTS_RAVEN_A2) $(OPTS_LAYOUT_ANSI) -o $@ $< 

%.hex: %.asm $(DEPS)
	gpasm -o $@ $<

clean:
	rm -f $(TARGETS:=.hex) $(TARGETS:=.lst) $(TARGETS:=.cod)
