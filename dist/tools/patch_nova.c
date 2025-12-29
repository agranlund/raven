#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* remove 68030 cache functions from TT nova drivers */
/* appears to only be used from functions that delays by reading from isa bus */
/* these are already marked as cache-precise in the pmmu so we can simply remove them */

const unsigned char patch_org[] =
{
    0x30, 0x28, 0x00, 0x88,                 /*      move.w   (0x88,a0),d0    <- rts */
    0xb0, 0x7c, 0x00, 0x14,                 /*      cmp.w    #0x14,d0               */
    0x6d, 0x16,                             /*      blt.b    .1                     */
    0x4e, 0x7a, 0x00, 0x02,                 /*      movec    cacr,d0                */
    0x22, 0x00,                             /*      move.l   d0,d1                  */
    0xc2, 0xbc, 0xff, 0xff, 0xfe, 0xfe,     /*      and.l    #-0x102,d1             */
    0x82, 0xbc, 0x00, 0x00, 0x08, 0x08,     /*      or.l     #0x0808,d1             */
    0x4e, 0x7b, 0x10, 0x02,                 /*      movec    d1,cacr                */
    0x4e, 0x75,                             /* 1:   rts                             */

    0x32, 0x28, 0x00, 0x88,                 /*      move.w   (0x88,a0),d1    <- rts */
    0xb2, 0x7c, 0x00, 0x14,                 /*      cmp.w    #0x14,d1               */
    0x6d, 0x04,                             /*      blt.b    .2,d1                  */
    0x4e, 0x7b, 0x00, 0x02,                 /*      movec    d0,cacr                */
    0x4e, 0x75                              /* 2:   rts                             */
};

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: patch_nova <file>\n");
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
            buf[i +  0] = 0x4e; buf[i +  1] = 0x75; /* rts */
            buf[i + 34] = 0x4e; buf[i + 35] = 0x75; /* rts */
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
