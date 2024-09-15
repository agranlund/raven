#include <stdio.h>
#include <stdlib.h>
#include <tusb.h>

#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "flash.h"

#define APP_NAME    "Raven ROM programmer"
#define APP_VER     "0.2A"

#define GPIO_LED    25

#define iobuflen 512
char iobuf[iobuflen];
int iobufpos;

void cmd_help()
{
    printf(APP_NAME " " APP_VER "\n"
           " [h]elp\n"
           " [i]nfo\n"
           " [d]ump {offset} {len}\n"
           " [p]rog {offset}\n"
           " [e]rase\n"
           "  or start sending S-records\n");
}

void cmd_info()
{
    uint mid, did;
    flash_Identify(&mid, &did);
    const char* midstring = flash_ManufacturerString(mid);
    const char* didstring = flash_DeviceString(mid, did);
    printf("[$%04x:$%04x] %s : %s  \n", mid, did, midstring, didstring, mid);
}

void cmd_dump(uint addr, uint len)
{
    // align address and size
    uint start = addr & ~15UL;
    uint end = (addr + len + 15) & ~15UL;
    len = (end > start) ? end - start : 256;

    // dump the data
    uint* pd0 = (uint*)&iobuf[ 0];
    uint* pd1 = (uint*)&iobuf[ 4];
    uint* pd2 = (uint*)&iobuf[ 8];
    uint* pd3 = (uint*)&iobuf[12];

    addr = start;
    printf("             0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    while (addr < (start + len))
    {
        *pd0 = flash_Read(addr +  0);
        *pd1 = flash_Read(addr +  4);
        *pd2 = flash_Read(addr +  8);
        *pd3 = flash_Read(addr + 12);
        printf(" %08x : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            addr,
            iobuf[ 3], iobuf[ 2], iobuf[ 1], iobuf[ 0], iobuf[ 7], iobuf[ 6], iobuf[ 5], iobuf[ 4],
            iobuf[11], iobuf[10], iobuf[ 9], iobuf[ 8], iobuf[15], iobuf[14], iobuf[13], iobuf[12]);
        addr += 16;
    }
}

void cmd_prog(uint addr)
{
    printf("Erasing flash...\n");
    flash_Erase();

    addr &= ~3UL;
    printf("Receive file at $%08x\n", addr);

    uint size = 0;
    uint counter = 0;
    bool started = false;
    bool finished = false;
    uint oldsec = 0xffffffff;

    while(!finished)
    {
        memset(iobuf, 0xff, iobuflen);
        int pos = 0;
        while (pos < 512)
        {
            int v = getchar_timeout_us(2000000);
            if (v < 0 || v > 255)
            {
                if (started) {
                    finished = true;
                    break;
                }
                continue;
            }

            iobuf[pos++] = (unsigned char)v;
            started = true;
        }

        if (started && !finished)
        {
            int written = 0;
            for (int i=0; i<512; i+=4)
            {
#if 0                
                uint newsec = flash_GetSector(addr + size + i);
                if (newsec != oldsec)
                {
                    flash_EraseSector(newsec);
                    oldsec = newsec;
                }
#endif
                uint dataLE = *((uint*)&iobuf[i]);
                if (dataLE != 0xffffffff)
                {
                    uint dataBE =
                        ((dataLE & 0xff000000) >> 24) |
                        ((dataLE & 0x00ff0000) >>  8) |
                        ((dataLE & 0x0000ff00) <<  8) |
                        ((dataLE & 0x000000ff) << 24);

                    flash_Write(addr + size + i, dataBE);
                    written++;
                }
            }

            size += pos;
            counter++;
            if (written != 0) {
                gpio_put_masked(1<<GPIO_LED, counter & 1 ? 0xffffffff : 0x00000000);
            }
        }
    }

    gpio_put_masked(1<<GPIO_LED, 0xffffffff);
    printf("\nDone. %i bytes written\n", size);
}

void cmd_erase()
{
    printf("Erase...");
    flash_Erase();
    printf("Done\n");
}

static int srec_byte(const char *buf, int index)
{
    unsigned int v;

    if (sscanf(buf + (index * 2), "%2x", &v) == 1) {
        return v;
    }
    return -1;
}

typedef struct
{
    uint32_t        address;
    uint32_t        data_len;
    const char      *data;
} srec_info_t;

static int srec_validate(const char *buf, int buf_len, srec_info_t *info)
{
    /* header sanity */
    if (buf_len < 9)
    {
        if (info != NULL)
            printf("srec: buffer too short\n");
        return -1;
    }
    if (buf[0] != 'S')
    {
        if (info != NULL)
            printf("srec: not 'S'\n");
        return -1;
    }

    /* get/validate length byte */
    int srec_len = srec_byte(buf, 1);
    if (srec_len < 3)
    {
        printf("srec: len too short\n");
        return -1;
    }
    if ((srec_len * 2 + 4) > buf_len)
    {
        printf("srec: len too long\n");
        return -1;
    }

    /* validate checksum */
    int sum = 0;
    for (int i = 0; i <= srec_len; i++) 
    {
        sum += srec_byte(buf + 2, i);
    }
    if ((sum & 0xff) != 0xff)
    {
        printf("srec: sum mismatch\n");
        return -1;
    }

    /* type-specific validation */
    const char *afmt = NULL;
    int asize = 0;
    int type = buf[1] - '0';
    switch (type)
    {
    case 0:
        afmt = "%4x";
        asize = 4;
        break;
    case 3:
        afmt = "%8x";
        asize = 8;
        break;
    case 7:
        afmt = "%8x";
        asize = 0;
        break;
    default:
        printf("srec: bad type\n");
        return -1;
    }
    if (srec_len < ((asize / 2) + 1)) {
        printf("srec: len wrong for type\n");
        return -1;
    }
    if (info)
    {
        if (sscanf(buf + 4, afmt, &info->address) != 1)
        {
            printf("srec: can't parse address");
            return -1;
        }
        info->data_len = srec_len - (asize / 2) - 1;
        info->data = buf + 4 + asize;
    }

    return type;
}

static void srec_flash(uint8_t *buf, uint32_t address, int len)
{
    if (len > 0)
    {
        /* pad to 4B multiple */
        while (len % 4)
            buf[len++] = 0xff;

        /* write to flash, swizzle to suit layout (XXX shouldn't the flash code do this?) */
        const uint32_t *p = (const uint32_t *)buf;
        for (int i = 0;
             i < len;
             p++, i += 4)
        {
            flash_Write(address + i, __builtin_bswap32(*p));
        }
    }
}

static void cmd_srecord()
{
    /* we are here because a valid S0 record was received */
    printf("S-record upload, erasing flash...");
    flash_Erase();
    printf("receiving data...\n");

    /* set initial buffer state */
    uint32_t iobuf_base = 0;
    uint32_t iobuf_len = 0;

    /* receive S-records */
    for (;;) {
        char linebuf[128];
        int linelen = 0;

        /* receive one S-record */
        for (;;)
        {
            /* receive bytes, quit when upload stops */
            int v = getchar_timeout_us(2000000);
            if (v == PICO_ERROR_TIMEOUT)
            {
                /* we expect S7 at the end to flush, so buffer should be empty */
                if (iobuf_len == 0) {
                    printf("done.\n");
                } else {
                    printf("\nERROR: S-record upload timeout\n");
                }
                return;
            }

            /* end of record? */
            if ((v == '\r') || (v == '\n'))
            {
                /* non-empty line, process */
                if (linelen > 0)
                {
                    linebuf[linelen] = '\0';
                    break;
                }
                /* empty line, ignore */
                continue;
            }

            /* accumulate bytes */
            if (linelen < (sizeof(linebuf) - 2))
            {
                linebuf[linelen++] = v;
            } else {
                printf("ERROR: S-record too long\n");
                return;
            }
        }

        srec_info_t sinfo;
        switch (srec_validate(linebuf, linelen, &sinfo))
        {
            case 0:
                /* ignore additional S0 records, probably concatenated uploads */
                continue;
            case 3:
                /* process S3 record below */
                break;
            case 7:
                /* entrypoint, usually last, clean any buffer contents ready for completion */
                srec_flash(iobuf, iobuf_base, iobuf_len);
                iobuf_len = 0;
                continue;
            default:
                printf("ERROR: invalid / unsupported S-record '%s'\n", linebuf);
                return;
        }

        /* new S-record not contiguous? */
        if (sinfo.address != (iobuf_base + iobuf_len))
        {
            /* flush any existing buffer contents */
            srec_flash(iobuf, iobuf_base, iobuf_len);

            /* set initial buffer base address, 4-aligned, pad leading bytes */
            iobuf_base = sinfo.address & 0xfffffffc;
            iobuf_len = sinfo.address - iobuf_base;

            /*
             * We depend on the flash behaviour of not writing / reading back 0xff bytes
             * to deal with the case where the 4-aligned buffer overlaps some byte(s)
             * that were previously programmed.
             *
             * This is only likely in the case where the S-record stream is highly optimised,
             * i.e. almost never.
             */
            iobuf[0] = iobuf[1] = iobuf[2] = 0xff;
        }

        /* update the buffer with bytes from the S-record */
        for (int i = 0; i < sinfo.data_len; i++)
        {
            iobuf[iobuf_len++] = srec_byte(sinfo.data, i);

            /* has buffer become full? */
            if (iobuf_len == iobuflen)
            {
                /* yes, flush it */
                srec_flash(iobuf, iobuf_base, iobuf_len);
                iobuf_base = iobuf_base + iobuflen;
                iobuf_len = 0;
            }
        }
    }
}

uint string_to_uint(char* str)
{
    if (str[0] == '$')
        return strtoul(str+1, 0, 16);
    else if (str[0] == '0' && str[1] == 'x')
        return strtoul(str+2, 0, 16);
    return strtoul(str, 0, 10);
}

void parse_command()
{
    for (int i=0; i<iobufpos+4; i++) {
        if ((i < iobuflen) && (
            (iobuf[i] == ' ') || 
            (iobuf[i] == '\r') || 
            (iobuf[i] == '\n') || 
            (iobuf[i] == '\t') || 
            (i >= iobufpos) ))
        {
            iobuf[i] = 0;
        }
    }

    char* arg1 = &iobuf[strlen(iobuf)+1];
    char* arg2 = &arg1[strlen(arg1)+1];

    if ((strcmp(iobuf, "help") == 0) || (strcmp(iobuf, "h") == 0))
    {
        cmd_help();
    }
    else if ((strcmp(iobuf, "info") == 0) || (strcmp(iobuf, "i") == 0))
    {
        cmd_info();
    }
    else if ((strcmp(iobuf, "dump") == 0) || (strcmp(iobuf, "d") == 0))
    {
        cmd_dump(string_to_uint(arg1), string_to_uint(arg2));
    }
    else if ((strcmp(iobuf, "prog") == 0) || (strcmp(iobuf, "p") == 0))
    {
        cmd_prog(string_to_uint(arg1));
    }
    else if ((strcmp(iobuf, "erase") == 0) || (strcmp(iobuf, "e") == 0))
    {
        cmd_erase();
    }
    else if (srec_validate(iobuf, iobufpos, NULL) == 0)
    {
        cmd_srecord();
    }
    else
    {
    }
}

int main()
{
    // init and wait for a serial connection
	bi_decl(bi_program_description(APP_NAME));
	stdio_init_all();
    flash_Init();

    gpio_init(GPIO_LED);
    gpio_set_dir(GPIO_LED, GPIO_OUT);
    gpio_put_masked(1<<GPIO_LED, 0xffffffff);

    int waitcounter = 0;
	while (!tud_cdc_connected())
    {
        sleep_ms(100);
    }

    // display some initial info
    printf("\n");
    cmd_help();
    printf("\n> ");

    // parse input and call commands
    iobufpos = 0;
    while(1)
    {
        int c = getchar();
        iobuf[iobufpos] = (char) c;
        if (iobufpos < (iobuflen - 4))
            iobufpos++;

        if (c == '\r')
        {
            printf("\n");
            parse_command();
            printf("> ");
            iobufpos = 0;
        }
        else if (c != '\n')
        {
            putchar(c);
        }
    }
	return 0;
}
