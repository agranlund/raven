APPS =	patch_xboot \
		patch_nova \
		patch_aes34 \
		#

all: $(APPS)

clean:
	rm -f $(APPS)

patch_xboot: patch_xboot.c
	$(CC) $< -o $@

patch_nova: patch_nova.c
	$(CC) $< -o $@

patch_aes34: patch_aes34.c
	$(CC) $< -o $@
