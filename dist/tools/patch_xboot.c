#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* remove 68030 cache functions from xboot  */
/* todo: replace with 68060 methods?        */

const unsigned char patch_org[] =
{
    0x20, 0x3c, 0x00, 0x00, 0x39, 0x19, /* move.l #0x3919,d0    */
    0x4e, 0x7b, 0x00, 0x02,             /* movec  d0,cacr       */
    0x4e, 0x75,                         /* rts                  */
    0x20, 0x3c, 0x00, 0x00, 0x08, 0x08, /* move.l #0x0808,d0    */
    0x4e, 0x7b, 0x00, 0x02,             /* movec  d0,cacr       */
    0x4e, 0x75,                         /* rts                  */
};

const unsigned char patch_new[] =
{
    0x4e, 0x75, 0x00, 0x00, 0x39, 0x19, /* rts                  */
    0x4e, 0x7b, 0x00, 0x02,             /* movec  d0,cacr       */
    0x4e, 0x75,                         /* rts                  */
    0xe4, 0x75, 0x00, 0x00, 0x08, 0x08, /* rts                  */
    0x4e, 0x7b, 0x00, 0x02,             /* movec  d0,cacr       */
    0x4e, 0x75,                         /* rts                  */
};

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: patch_xboot32e <file>\n");
        return 0;
    }

    const char* fname = argv[1];
    FILE* f = fopen(fname, "rb");
    if (!f) {
        printf("failed opening %s for read\n", fname);
        return -2;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* buf = malloc(len);
    fread(buf, len, 1, f);
    fclose(f);

    long plen = sizeof(patch_org);
    for (long i=0; i<len - plen; i++) {
        if (memcmp(&buf[i], patch_org, plen) == 0) {
            memcpy(&buf[i], patch_new, plen);
            f = fopen(fname, "wb");
            if (f) {
                fwrite(buf, len, 1, f);
                fclose(f);
                printf("%s patched at %08lx\n", fname, i);
            } else {
                printf("failed opening %s for write\n", fname);
            }
            free(buf);
            return 0;
        }
    }

    printf("not patched\n");
    free(buf);
    return 0;
}
