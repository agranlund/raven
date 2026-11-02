
FOLDERS = \
	cacheoff \
	cacheon \
	dsp \
	eiffel \
	fat32 \
	flash \
	fpu_emu \
	fpu_off \
	mon \
	patch_xc \
	resetnvm \
	serial.cpx \
	thingpal \
	uiptools \
	ymodem \
	#

all:
	for dir in $(FOLDERS); do \
		make -C $$dir ; \
	done

clean:
	for dir in $(FOLDERS); do \
		make -C $$dir clean ; \
	done
