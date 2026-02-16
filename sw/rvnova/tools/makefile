
CFLAGS = -I../../lib

all: bibs

.PHONY: bibs
bibs:
	gcc $(CFLAGS) mbib.c -o mbib
	./mbib

.PHONY: clean
clean:
	rm -f mbib *.bib

