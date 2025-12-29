APPS =	patch_xboot \
		#

all: $(APPS)

clean:
	rm -f $(APPS)

patch_xboot: patch_xboot.c
	$(CC) $< -o $@
