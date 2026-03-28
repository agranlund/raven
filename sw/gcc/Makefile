
all: libcmini lgcc

libcmini:
	make -C src/libcmini install
	cp include/mint/linea.h include/linea.h

lgcc:
	m68k-atari-mintelf-gcc -m68000 -c src/lgcc.S -o lib/lgcc.o

clean:
	make -C src/libcmini clean
