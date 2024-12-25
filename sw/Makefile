
DIRS = \
	rvbios \
	rvnova \
	tools \
	#end

DIRS_FORCE_PC = \
	#end

DIRS_FORCE_GCC = \
	isa/isa_bios \
	isa/drivers/gus \
	#end

all: sw rom

clean: sw_clean rom_clean

init:
	make -C gcc/libcmini

distclean: clean
	make -C gcc/libcmini clean
	rm -f gcc/lgcc.o
	rm -f gcc/stripx/stripx

sw: init
	for dir in $(DIRS); do \
		make -C $$dir ; \
	done
	for dir in $(DIRS_FORCE_GCC); do \
		make -C $$dir gcc ; \
	done
	for dir in $(DIRS_FORCE_PC); do \
		make -C $$dir pc ; \
	done

sw_clean:
	for dir in $(DIRS) $(DIRS_FORCE_GCC) $(DIRS_FORCE_PC); do \
		make -C $$dir clean ; \
	done


rom: init
	make -C rom tos
	make -C rom/srec

rom_clean:
	make -C rom clean
	make -C rom/srec clean

