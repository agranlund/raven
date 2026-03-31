
DIRS = \
	rvbios \
	rvnova \
	tools \
	isa/isa_bios \
	isa/drivers/gus \
	isa/drivers/mpu401 \
	#end

DIRS_FORCE_PC = \
	#end

DIRS_FORCE_GCC = \
#	isa/isa_bios \
#	isa/drivers/gus \
	#end

all: sw rom

clean: sw_clean rom_clean

init:
#	make -C gcc

distclean: clean
	make -C gcc/src/libcmini clean
	rm -f gcc/lib/lgcc.o
	rm -f gcc/bin/stripx

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

