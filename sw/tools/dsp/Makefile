
FOLDERS = \
	dspxbios \
	dsprun \
	dsptest \
	#

all:
	for dir in $(FOLDERS); do \
		make -C $$dir ; \
	done

clean:
	for dir in $(FOLDERS); do \
		make -C $$dir clean ; \
	done
