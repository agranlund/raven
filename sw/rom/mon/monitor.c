#include "sys.h"
#include "lib.h"
#include "hw/cpu.h"
#include "hw/uart.h"
#include "hw/rtc.h"
#include "hw/vga.h"
#include "monitor.h"
#include "config.h"
#include "m68k_disasm.h"

#define MONBUFFERSIZE 1024
char monBuffer[MONBUFFERSIZE];

char oldcmd;
int oldarg1;
int oldarg2;

uint32_t dump_old_addr;
uint32_t dump_old_size;
uint32_t dasm_old_addr;
uint32_t dasm_old_size;

static void mon_Main(regs_t* regs);
static void monHelp();
static void monRegs(regs_t* regs);
static void monDump(uint32_t addr, uint32_t size);
static void monDisasm(uint32_t addr, uint32_t size);
static void monRead(uint32_t bits, uint32_t addr);
static void monWrite(uint32_t bits, uint32_t addr, uint32_t val);
static void monRtcDump();
static void monRtcClear();
static void monRtcReset();
static void monCfgList();
static void monCfgRead(char* cfg);
static void monCfgWrite(char* cfg, uint32_t val);
static void monSrec();
extern void monTest(char* cmd, uint32_t val);   // test.c

static void monCookieList();
static void monCookieRead(char* id);
static void monCookieWrite(char* id, char* val);


bool mon_DetectTos()
{
    extern bool atari_DetectTos();
    return atari_DetectTos();
}

bool mon_Init()
{
    dump_old_addr = 0x00000000;
    dump_old_size = 256;
    dasm_old_addr = 0x40000400;
    dasm_old_size = 16;

    cpu_SetNMI(mon_Main);
    return true;
}

void mon_Start()
{
    cpu_TriggerNMI();
}

uint16_t mon_Parse(regs_t* regs)
{
    // convert whitespaces to 0 as argument delimiters
    size_t monBufferSize = strlen(monBuffer);
    for (int i=0; i<monBufferSize; i++) {
        if (monBuffer[i] <= 32 || monBuffer[i] >= 127) {
            monBuffer[i] = 0;
        }
    }

    // parse command arguments
    // XXX TODO use scan()
    int args = 0;
    char* argc[8];

    uint16_t exit = 0;
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
        else if (strcmp(argc[0], "d") == 0)         { monDump(args > 1 ? strtoi(argc[1]) : 0xffffffff, args > 2 ? strtoi(argc[2]) : 0xffffffff); }
        else if (strcmp(argc[0], "a") == 0)         { monDisasm(args > 1 ? strtoi(argc[1]) : 0xffffffff, args > 2 ? strtoi(argc[2]) : 0xffffffff); }
        else if (strcmp(argc[0], "pb") == 0)        { if (args>2) { monWrite( 8, strtoi(argc[1]), strtoi(argc[2])); } else { monRead( 8, strtoi(argc[1])); } }
        else if (strcmp(argc[0], "pw") == 0)        { if (args>2) { monWrite(16, strtoi(argc[1]), strtoi(argc[2])); } else { monRead(16, strtoi(argc[1])); } }
        else if (strcmp(argc[0], "pl") == 0)        { if (args>2) { monWrite(32, strtoi(argc[1]), strtoi(argc[2])); } else { monRead(32, strtoi(argc[1])); } }
        else if (mon_DetectTos() && (strcmp(argc[0], "c") == 0)) {
            if (args==1) { monCookieList(); }
            else if (args==2) { monCookieRead(argc[1]); }
            else if (args==3) { monCookieWrite(argc[1], argc[2]); }
        }
        else if (strcmp(argc[0], "rtc") == 0) {
            if ((args>1) && (strcmp(argc[1], "clear") == 0)) { monRtcClear(); }
            else if ((args>1) && (strcmp(argc[1], "reset") == 0)) { monRtcReset(); }
            monRtcDump();
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
        else if (strcmp(argc[0], "vga") == 0)
        {
            if (args == 1) {
                puts(   "Commands:\n"
                        "   vga init\n"
                        "  vga test\n" );
            } else if (strcmp(argc[1], "init") == 0) {
                vga_Init();
            } else if (strcmp(argc[1], "test") == 0) {
                vga_Test();
            }
        }
        else if (strcmp(argc[0], "run") == 0)       { cpu_Call(strtoi(argc[1])); }
        else if (strncmp(argc[0], "S0", 2) == 0)    { monSrec(argc[0]); }
        else                                        { monHelp(); }
    }
    return exit;
}

void mon_Main(regs_t* regs)
{
    oldcmd = oldarg1 = oldarg2 = 0;

    printf("\n# Raven monitor %06x #\n", VERSION);
    monRegs(regs);
    putchar('\n');
    monHelp();
    putchar('\n');

    uint16_t exit = 0;
    while(exit == 0)
    {
        putchar('>');
        putchar(' ');

        if (gets(monBuffer, sizeof(monBuffer)) == NULL) {
            continue;
        }

        exit = mon_Parse(regs);
    }
}

void mon_MainOnce(regs_t* regs)
{
    cpu_SetNMI(mon_Main);
    mon_Parse(regs);
}

void mon_Exec(const char* s)
{
    if (s && *s) {
        strcpy(monBuffer, s);
        cpu_SetNMI(mon_MainOnce);
        cpu_TriggerNMI();
    } else {
        cpu_SetNMI(mon_Main);
        cpu_TriggerNMI();
    }
}

void monHelp()
{
    puts("Commands:\n"
         "  x                 : exit monitor\n"
         "  r                 : show registers\n"
         "  pb [addr] {val}   : peek/poke byte\n"
         "  pw [addr] {val}   : peek/poke word\n"
         "  pl [addr] {val}   : peek/poke long\n"
         "  d  [addr] {len}   : dump memory\n"
         "  a  [addr] {len}   : disassemble");
    if (mon_DetectTos()) { puts(
         "  c {id} {val}      : cookie (TOS)"); }
    puts(
         "  rtc {clear/reset} : dump/clear/reset rtc\n"
         "  vga {cmd} {opt}   : screen commands\n"
         "  cfg {opt} {val}   : list/get/set option\n"
         "  run [addr]        : call program at address\n"
         "  reset             : reset computer");
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

void monDisasm(uint32_t addr, uint32_t size)
{
    int i, n;
	struct DisasmPara_68k dp;
	static char opcode[16];
	static char operands[128];
	m68k_word* p = (addr == 0xffffffff) ? (m68k_word*)dasm_old_addr : (m68k_word*)addr;

    if (size == 0xffffffff) { size = dasm_old_size; }
    if (size == 0) { size = 8; }
    dasm_old_size = size;

    dp.get_areg = 0;
    dp.find_symbol = 0;                                
    dp.opcode = opcode;
    dp.operands = operands;
    for (i = 0; i<size; i++) {
        dp.instr = dp.iaddr = p;
        p = M68k_Disassemble(&dp);
        if (opcode[0] == 0) {
            strcpy(opcode, "???");
            strcpy(operands, "???");
        }
        fmt("%l: ", (uint32_t)dp.iaddr);
        for (n = 0; n < 12; n++) {
            if (opcode[n] == 0)
                break;
            putchar(opcode[n]);
        }
        for (; n < 8; n++) {
            putchar(' ');
        }
        puts(operands);
    }
    dasm_old_addr = (uint32_t)p;
}

void monDump(uint32_t addr, uint32_t size)
{
    if (addr == 0xffffffff)     addr = dump_old_addr;
    if (size == 0xffffffff)     size = dump_old_size;
    if (size == 0)              size = 256;
    else if (size > (16*256))   size = 16*256;
    else if (size < (16*1))     size = 16*1;
    dump_old_addr = addr + size;
    dump_old_size = size;
    hexdump((uint8_t *)addr, addr, size, 'b');
}

void monRead(uint32_t bits, uint32_t addr)
{
    fmt("%l : ", addr);
    switch (bits)
    {
        case 8: {
            uint8_t v = cpu_SafeReadByte(addr);
            fmt("$%b\n", v);
        } break;
        case 16: {
            uint16_t v = cpu_SafeReadWord(addr);
            fmt("$%w\n", v);
        } break;
        case 32: {
            uint32_t v = cpu_SafeReadLong(addr);
            fmt("$%l\n", v);
        } break;
    }
}

void monWrite(uint32_t bits, uint32_t addr, uint32_t val)
{
    switch (bits)
    {
        case 8:
            cpu_SafeWriteByte(addr, (uint8_t)val);
            break;
        case 16:
            cpu_SafeWriteWord(addr, (uint16_t)val);
            break;
        case 32:
            cpu_SafeWriteLong(addr, (uint32_t)val);
            break;
    }
}

void monCookieList() {
    volatile uint32_t* p = *((volatile uint32_t**)0x5a0);
    while (p && p[0]) {
        char b0 = (p[0] >> 24) & 0xff; b0 = isprint(b0) ? b0 : ' ';
        char b1 = (p[0] >> 16) & 0xff; b1 = isprint(b1) ? b1 : ' ';
        char b2 = (p[0] >>  8) & 0xff; b2 = isprint(b2) ? b2 : ' ';
        char b3 = (p[0] >>  0) & 0xff; b3 = isprint(b3) ? b3 : ' ';
        printf("[%c%c%c%c] : $%08x : $%08x\n", b0, b1, b2, b3, p[0], p[1]);
        p += 2;
    }
}

void monCookieRead(char* id) {
    uint32_t cid = *((uint32_t*)id);
    if (*id == '$' || (strlen(id) != 4)) { cid = strtoi(id); }
    volatile uint32_t* p = *((volatile uint32_t**)0x5a0);
    while (p && p[0]) {
        if (p[0] == cid) {
            char b0 = (p[0] >> 24) & 0xff; b0 = isprint(b0) ? b0 : ' ';
            char b1 = (p[0] >> 16) & 0xff; b1 = isprint(b1) ? b1 : ' ';
            char b2 = (p[0] >>  8) & 0xff; b2 = isprint(b2) ? b2 : ' ';
            char b3 = (p[0] >>  0) & 0xff; b3 = isprint(b3) ? b3 : ' ';
            printf("[%c%c%c%c] : $%08x : $%08x\n", b0, b1, b2, b3, p[0], p[1]);
            break;
        }
        p += 2;
    }
}

void monCookieWrite(char* id, char* vl)
{
    uint32_t cvl = strtoi(vl);
    uint32_t cid = *((uint32_t*)id);
    if (*id == '$' || (strlen(id) != 4)) { cid = strtoi(id); }
    volatile uint32_t* p = *((volatile uint32_t**)0x5a0);
    while (p && p[0]) {
        if (p[0] == cid) {
            p[1] = cvl;
            break;
        }
        p += 2;
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
    rtc_ClearRam();
    cfg_Reset();
}

void monRtcReset()
{
    rtc_Reset();
    cfg_Reset();
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
                uint32_t v = cfg_GetValue(c);
                fmt(" %s : %d [%d-%d]\n", c->name, v, c->min, c->max);
            }
        }
    }
}

void monCfgRead(char* cfg)
{
    const cfg_entry_t* e = cfg_Find(cfg);
    if (e) {
        uint32_t v = cfg_GetValue(e);
        uint32_t s = ((e->bits + 7) >> 3);
        if (s <= 1) {
            fmt("$%b\n", v);
        } else if (s <= 2) {
            fmt("$%w\n", v);
        } else {
            fmt("$%l\n", v);
        }
    }
}

void monCfgWrite(char* cfg, uint32_t val)
{
    cfg_SetValue(cfg_Find(cfg), val);
}

static uint8_t srec_get_nyb()
{
    for (;;)
    {
        int c = getchar();
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


int srec_sum;
#define srec_mem_start  0x00600000
#define srec_mem_end    0x00800000

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

    // get/discard sum byte
    (void)srec_get_byte();

    // discard any additional bytes
    while (getchar() >= 0);

    // if no offset, upload was to DRAM - run it
    if (address_offset == 0)
    {
        if ((address < low_address) || (address >= (high_address - 2)))
        {
            puts("srec: S7 bad address");
            return;
        }
        if (1) {
            fmt("S-record upload complete at address %p\n", address);
        } else {
            fmt("S-record upload complete, jumping to %p...\n\n", address);
            cpu_Call(address);
        }
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
            int c = getchar();
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
            int c = getchar();
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
            if (address >= BIOS_ROM)
            {
                address_offset = BIOS_ROM - srec_mem_start;
                address -= address_offset;
            }
            low_address = address;
        }
        if ((address < srec_mem_start) || (address >= srec_mem_end))
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
