#include "sys.h"
#include "lib.h"
#include "hw/cpu.h"
#include "hw/uart.h"
#include "hw/rtc.h"
#include "monitor.h"
#include "config.h"

extern int test_ym_write(int size);

#define MONBUFFERSIZE 1024
char monBuffer[MONBUFFERSIZE];

static void mon_Main(regs_t* regs);
static void monHelp();
static void monRegs(regs_t* regs);
static void monDump(uint32_t addr, uint32_t size);
static void monRead(uint32_t bits, uint32_t addr);
static void monWrite(uint32_t bits, uint32_t addr, uint32_t val);
static void monRtcDump();
static void monRtcClear();
static void monCfgList();
static void monCfgRead(char* cfg);
static void monCfgWrite(char* cfg, uint32_t val);
static void monLoad(uint32_t addr, uint32_t size);
static void monRun(uint32_t addr);
static void monTest(char* cmd, uint32_t val);
static void monSrec();

bool mon_Init()
{
    cpu_SetNMI(mon_Main);
    return true;
}

void mon_Start()
{
    cpu_TriggerNMI();
}

void mon_Main(regs_t* regs)
{
    puts("");
    monRegs(regs);

    uint16_t exit = 0;
    while(exit == 0)
    {
        putchar('>');
        putchar(' ');

        if (gets(monBuffer, sizeof(monBuffer)) == NULL)
            continue;
        size_t monBufferSize = strlen(monBuffer);

        // convert whitespaces to 0 as argument delimiters
        for (int i=0; i<monBufferSize; i++) {
            if (monBuffer[i] <= 32 || monBuffer[i] >= 127) {
                monBuffer[i] = 0;
            }
        }

        // parse command arguments
        // XXX TODO use scan()
        int args = 0;
        char* argc[8];

        uint32_t start = 0;
        for (int i=0; i<8; i++) {
            argc[i] = 0;
            // skip whitespaces
            while ((start < monBufferSize) && (monBuffer[start] == 0)) {
                start++;
            }
            // find length
            if (start < monBufferSize) {
                uint32_t end = start;
                while ((end < monBufferSize) && (monBuffer[end] != 0)) {
                    end++;
                }
                uint32_t size = end - start;
                if (size > 0) {
                    argc[i] = (char*)&monBuffer[start];
                    start += size;
                    args++;
                }
            }
        }

        // execute command
        if (args > 0) {
            if (strcmp(argc[0], "x") == 0)              { exit = 1; }
            else if (strcmp(argc[0], "reset") == 0)     { extern void start(); start(); }
            else if (strcmp(argc[0], "r") == 0)         { monRegs(regs); }
            else if (strcmp(argc[0], "d") == 0)         { monDump(strtoi(argc[1]), strtoi(argc[2])); }
            else if (strcmp(argc[0], "pb") == 0)        { if (args>2) { monWrite( 8, strtoi(argc[1]), strtoi(argc[2])); } else { monRead( 8, strtoi(argc[1])); } }
            else if (strcmp(argc[0], "pw") == 0)        { if (args>2) { monWrite(16, strtoi(argc[1]), strtoi(argc[2])); } else { monRead(16, strtoi(argc[1])); } }
            else if (strcmp(argc[0], "pl") == 0)        { if (args>2) { monWrite(32, strtoi(argc[1]), strtoi(argc[2])); } else { monRead(32, strtoi(argc[1])); } }
            else if (strcmp(argc[0], "rtc") == 0) {
                if ((args>1) && (strcmp(argc[1], "clear") == 0)) {
                    monRtcClear();
                } else {
                    monRtcDump();
                }
            }
            else if (strcmp(argc[0], "cfg") == 0) {
                if (args == 1) {
                    monCfgList();
                } else if (args == 2) {
                    monCfgRead(argc[1]);
                } else {
                    monCfgWrite(argc[1], strtoi(argc[2]));
                }
            }
            else if (strcmp(argc[0], "load") == 0)      { monLoad(strtoi(argc[1]), strtoi(argc[2])); }
            else if (strcmp(argc[0], "run") == 0)       { monRun(strtoi(argc[1])); }
            else if (strcmp(argc[0], "test") == 0)      { monTest((args > 1) ? argc[1] : 0, (args > 2) ? strtoi(argc[2]) : 0); }
            else if (strncmp(argc[0], "S0", 2) == 0)    { monSrec(argc[0]); }
            else                                        { monHelp(); }
        }
    }
}

void monTest(char* test, uint32_t val)
{
    if (strcmp(test, "ym") == 0) {
        test_ym_write(1);
    }
    else {
        puts("\n"
             "Test commands:\n"
             "  ym\r\n");        
    }
}

void monHelp()
{
    puts("\n"
         "Commands:\n"
         "  x                 : exit monitor\n"
         "  r                 : show registers\n"
         "  pb [addr] {val}   : peek/poke byte\n"
         "  pw [addr] {val}   : peek/poke word\n"
         "  pl [addr] {val}   : peek/poke long\n"
         "  d  [addr] {len}   : dump memory\n"
         "  rtc {clear}       : dump/clear rtc\n"
         "  cfg {opt} {val}   : list/get/set option\n"
         "  reset             : reset computer\n"
         "  test {cmd} {val}  : test hardware\n");
}

void monRegs(regs_t* regs)
{
    if (!regs)
        return;
    fmt("\n d0: %l  d2: %l  d4: %l  d6: %l", regs->d0, regs->d2, regs->d4, regs->d6);
    fmt("\n d1: %l  d3: %l  d5: %l  d7: %l", regs->d1, regs->d3, regs->d5, regs->d7);
    fmt("\n a0: %l  a2: %l  a4: %l  a6: %l", regs->a0, regs->a2, regs->a4, regs->a6);
    fmt("\n a1: %l  a3: %l  a5: %l  a7: %l", regs->a1, regs->a3, regs->a5, regs->a7);
    fmt("\n pc: %l  sr: %w                   usp: %l", regs->pc, regs->sr, regs->usp);
    fmt("\nvbr: %l  tc: %l srp: %l urp: %l", regs->vbr, regs->tc, regs->srp, regs->urp);
    fmt("\ndt0: %l dt1: %l it0: %l it1: %l", regs->dtt0, regs->dtt1, regs->itt0, regs->itt1);
    fmt("\npcr: %l bcr: %l ccr: %l\n", regs->pcr, regs->buscr, regs->cacr);
}

void monDump(uint32_t addr, uint32_t size)
{
    const uint32_t sizeDefault = 256;
    const uint32_t sizeMin = 16;
    const uint32_t sizeMax = 16*256;
    if (size == 0)              size = sizeDefault;
    else if (size < sizeMin)    size = sizeMin;
    else if (size > sizeMax)    size = sizeMax;

    hexdump((uint8_t *)addr, addr, size, 'b');
}

void monRead(uint32_t bits, uint32_t addr)
{
    fmt("%l : ", addr);
    switch (bits)
    {
        case 8: {
            uint8_t v = IOB(addr, 0);
            fmt("$%b\n", v);
        } break;
        case 16: {
            uint16_t v = IOW(addr, 0);
            fmt("$%w\n", v);
        } break;
        case 32: {
            uint32_t v = IOL(addr, 0);
            fmt("$%l\n", v);
        } break;
    }
}

void monWrite(uint32_t bits, uint32_t addr, uint32_t val)
{
    switch (bits)
    {
        case 8:
            IOB(addr, 0) = (uint8_t) val;
            break;
        case 16:
            IOW(addr, 0) = (uint16_t) val;
            break;
        case 32:
            IOL(addr, 0) = (uint32_t) val;
            break;
    }
}

void monRtcDump()
{
    uint8_t regs[RTC_RAM_END];
    rtc_Read(0x00, regs, RTC_RAM_END);
    hexdump(regs, 0, 0x40, 'b');
}

void monRtcClear()
{
    uint8_t regs[0x40];
    memset(regs, 0, 0x40);
    rtc_Write(RTC_RAM_START, regs, RTC_RAM_END - RTC_RAM_START - 0x04);
}

void monCfgList()
{
    for (int i=0; i<cfg_Num(); i++) {
        const cfg_entry_t* c = cfg_Get(i);
        if (c) {
            if (c->opts) {
                fmt(" %s : %d [", c->name, cfg_GetValue(c));
                for (int j=0; j<=c->max; j++) {
                    fmt("%d:%s", j, c->opts[j]);
                    if (j < c->max) { fmt(" "); }
                }
                fmt("]\n");
            } else {
                fmt(" %s : %d [%d-%d]\n", c->name, cfg_GetValue(c), 0, c->max);
            }
        }
    }
}

void monCfgRead(char* cfg)
{
    int v = cfg_GetValue(cfg_Find(cfg));
    fmt("$%b\n", v);
}

void monCfgWrite(char* cfg, uint32_t val)
{
    cfg_SetValue(cfg_Find(cfg), val);
}

void monLoad(uint32_t addr, uint32_t size)
{
    uint8_t* dst = (uint8_t*)addr;
    while (size > 0)
    {
        volatile uint8_t lsr = IOB(PADDR_UART2, UART_LSR);
        if ((lsr & (1 << 0)) != 0)
        {
            *dst = IOB(PADDR_UART2, UART_RHR);
            dst++; size--;
        }
    }
}

void monRun(uint32_t addr)
{
    cpu_Call(addr);
}

static uint8_t srec_get_nyb()
{
    for (;;)
    {
        int c = getc();
        switch (c)
        {
        case -1:
            break;
        case '0'...'9':
            return c - '0';
        case 'a'...'f':
            return c - 'a' + 10;
        case 'A'...'F':
            return c - 'A' + 10;
        default:
            puts("srec: bad digit");
            return 0;
        }
    }
}

static int srec_sum;

static uint8_t srec_get_byte()
{
    uint8_t tmp = srec_get_nyb() << 4;
    tmp += srec_get_nyb();
    srec_sum += tmp;
    return tmp;
}

static uint32_t srec_get_long()
{
    uint32_t tmp = srec_get_byte() << 24;
    tmp += srec_get_byte() << 16;
    tmp += srec_get_byte() << 8;
    return tmp + srec_get_byte();
}

static void srec_s7(uint32_t address_offset, uint32_t low_address, uint32_t high_address)
{
    // check S7 record length
    if (srec_get_byte() != 5)
    {
        puts("srec: S7 bad length");
        return;
    }

    // get S7 record address
    uint32_t address = srec_get_long();

    // if no offset, upload was to DRAM - run it
    if (address_offset == 0)
    {
        if ((address < low_address) || (address >= (high_address - 2)))
        {
            puts("srec: S7 bad address");
            return;
        }
        fmt("S-record upload complete, jumping to %p...\n", address);
        cpu_Call(address);
    }

    // otherwise we just received something to be flashed...
}

void monSrec()
{
    uint32_t address_offset = 0;
    uint32_t low_address = 0;
    uint32_t high_address =0;

    for (;;)
    {
        // wait for 'S'
        for (;;)
        {
            int c = getc();
            if (c == 'S') break;
            switch (c)
            {
            case -1:
            case '\r':
            case '\n':
                break;
            default:
                puts("srec: sync lost");
                return;
            }
        }
        srec_sum = 0;

        // get record type
        for (;;)
        {
            int c = getc();
            if (c == '3') break;
            if (c == '7')
            {
                srec_s7(address_offset, low_address, high_address);
                return;
            }
            if (c != -1)
            {
                puts("srec: sync lost");
                return;
            }
        }

        // get S3 record length
        int srec_len = srec_get_byte();
        if (srec_len < 5) 
        {
            puts("srec: S3 bad length");
            return;
        }

        // get S3 record address
        uint32_t address = srec_get_long() - address_offset;
        srec_len -= 4;
        if (low_address == 0) {
            if (address > BIOS_ROM)
            {
                address_offset = BIOS_ROM - 0x400;
                address -= address_offset;
            }
            low_address = address;
        }
        if ((address < 0x400) || (address > (2 * 1024 * 1024)))
        {
            puts("srec: address out of range");
            return;
        }

        // get data bytes
        while (srec_len-- > 1)
        {
            uint8_t b = srec_get_byte();
            *(uint8_t *)address++ = b;
        }
        if (address > high_address) {
            high_address = address;
        }

        // get/discard sum byte
        (void)srec_get_byte();

        // validate checksum
        if ((srec_sum & 0xff) != 0xff)
        {
            puts("srec: bad checksum");
            return;
        }
    }
}
