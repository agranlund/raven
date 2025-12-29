APPS =	patch_xboot \
		patch_nova \
		#

all: $(APPS)

clean:
	rm -f $(APPS)

patch_xboot: patch_xboot.c
	$(CC) $< -o $@

patch_nova: patch_nova.c
	$(CC) $< -o $@
