TARGETS = Iksi Narnia Nessi

.PHONY: all clean

all:
	@for dir in $(TARGETS); do \
		echo --------------------------------------------; \
		echo Building $$dir ; \
		echo --------------------------------------------; \
		$(MAKE) -C $$dir || exit 1 ; \
	done

clean:
	@for dir in $(TARGETS); do \
		echo --------------------------------------------; \
		echo Cleaning $$dir ; \
		echo --------------------------------------------; \
		$(MAKE) -C $$dir clean || exit 1 ; \
	done
