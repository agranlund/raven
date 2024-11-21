
SUBDIRS := $(wildcard */.)

define submake
	for d in $(SUBDIRS); \
	do \
		make -C $$d $(1); \
	done
endef

.PHONY: all clean $(SUBDIRS)

all:
	$(call submake,$@)

clean:
	$(call submake,$@)

