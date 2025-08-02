#include <stdio.h>
#include <stdlib.h>
#include <tusb.h>

#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "flash.h"

#define APP_NAME    "Raven ROM programmer"
#define APP_VER     "20250731"
#define HDR_MAGIC   0x5241564E      /* 'RAVN' */
#define CFG_MAGIC   0x434F4E46      /* 'CONF' */
#define GPIO_LED    25

#define cfgbuflen 4096
char cfgbuf[cfgbuflen];

#define iobuflen 512
char iobuf[iobuflen];
int iobufpos;

void cmd_help()
{
    printf(APP_NAME " " APP_VER "\n"
           " [h]elp\n"
           " [i]nfo\n"
           " [d]ump {offset} {len}\n"
           " [p]rog\n"
           " [e]rase\n");
}

void cmd_info()
{
    uint mid, did;
    flash_Identify(&mid, &did);
    const char* midstring = flash_ManufacturerString(mid);
    const char* didstring = flash_DeviceString(mid, did);
    printf("[$%04x:$%04x] %s : %s  \n", mid, did, midstring, didstring, mid);
}

char printable_char(char c)
{
    return (c >= 32) ? c : '.';
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

        printf(" %08x : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
            addr,
            iobuf[ 3], iobuf[ 2], iobuf[ 1], iobuf[ 0], iobuf[ 7], iobuf[ 6], iobuf[ 5], iobuf[ 4],
            iobuf[11], iobuf[10], iobuf[ 9], iobuf[ 8], iobuf[15], iobuf[14], iobuf[13], iobuf[12],
            printable_char(iobuf[ 3]), printable_char(iobuf[ 2]), printable_char(iobuf[ 1]), printable_char(iobuf[ 0]),
            printable_char(iobuf[ 7]), printable_char(iobuf[ 6]), printable_char(iobuf[ 5]), printable_char(iobuf[ 4]),
            printable_char(iobuf[11]), printable_char(iobuf[10]), printable_char(iobuf[ 9]), printable_char(iobuf[ 8]),
            printable_char(iobuf[15]), printable_char(iobuf[14]), printable_char(iobuf[13]), printable_char(iobuf[12]));
        addr += 16;
    }
}

uint swap32(uint d) {
    return  ((d & 0xFF000000) >> 24) | ((d & 0x00FF0000) >> 8) | ((d & 0x0000FF00) << 8) | ((d & 0x000000FF) << 24);
}

uint save_cfg(void) {
    memset(cfgbuf, 0, cfgbuflen);
    if (flash_Read(0x400 + 0) == 0x5F524F4D) {                      // '_ROM'
        if (flash_Read(0x400 + 32) == 0x5F434647) {                 // '_CFG'
            uint size = flash_Read(0x400 + 32 + 8);                 // size
            uint addr = flash_Read(0x400 + 32 + 4) & 0x00ffffff;    // addr
            if (addr && size && (size <= cfgbuflen)) {
                for (uint i=0; i<size; i+=4) {
                    *((uint*)(&cfgbuf[i])) = flash_Read(addr + i);
                }
                return size;
            }
        }
    }
    return 0;
}

void cmd_prog(uint addr)
{
    printf("Backup config space...");
    uint cfgsave_size = save_cfg();
    printf(" %d bytes\n", cfgsave_size);

    printf("Erasing flash...\n");
    flash_Erase();

    uint size = 0;
    uint counter = 0;
    bool started = false;
    bool finished = false;
    uint oldsec = 0xffffffff;

    uint rvhdr_addr = 0;
    uint cfgdata_addr = 0;
    uint cfgdata_size = 0;

    addr &= ~3UL;
    printf("Receive file at $%08x\n", addr);

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
            if ((size == 0x400) && cfgsave_size) {
                if (swap32(*((uint*)&iobuf[0])) == 0x5F524F4D) { // '_ROM'
                    if (swap32(*((uint*)&iobuf[32+0])) == 0x5F434647) { // '_CFG'
                        cfgdata_addr = swap32(*((uint*)&iobuf[32+4])) & 0x00ffffff;
                        cfgdata_size = swap32(*((uint*)&iobuf[32+8]));
                    printf("Config space at $%08x (%d bytes)", cfgdata_addr, cfgdata_size);
                    }
                }
            }

            for (int i=0; i<512; i+=4)
            {
                uint cfgoffs = ((size + i) >= cfgdata_addr) ? ((size + i) - cfgdata_addr) : 0x0fffffff;
                if ((cfgoffs < cfgdata_size) && (cfgoffs < cfgsave_size))
                {
                    // rewrite previous config data
                    flash_Write(addr + size + i, *((uint*)&cfgbuf[cfgoffs]));
                }
                else
                {
                    // write new data
                    uint data = *((uint*)&iobuf[i]);
                    if (data != 0xffffffff)
                    {
                        flash_Write(addr + size + i, swap32(data));
                    }
                }
            }

            size += pos;
            counter++;
            gpio_put_masked(1<<GPIO_LED, counter & 4 ? 0xffffffff : 0x00000000);
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
        cmd_prog(0);
    }
    else if ((strcmp(iobuf, "erase") == 0) || (strcmp(iobuf, "e") == 0))
    {
        cmd_erase();
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

    memset(cfgbuf, 0, cfgbuflen);

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
