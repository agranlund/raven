
FOLDERS = \
	mon \
	cacheoff \
	cacheon \
	eiffel \
	fpu_off \
	patch_xc \
	resetnvm \
	thingpal \
	fpu_emu \
	serial.cpx

all:
	for dir in $(FOLDERS); do \
		make -C $$dir ; \
	done

clean:
	for dir in $(FOLDERS); do \
		make -C $$dir clean ; \
	done
