#include "sys.h"
#include "lib.h"
#include "hw/cpu.h"
#include "hw/uart.h"
#include "monitor.h"

extern int test_ym_write(int size);

#define MONBUFFERSIZE 1024
char monBuffer[MONBUFFERSIZE];

void monHelp();
void monRegs(regs_t* regs);
void monDump(uint32_t addr, uint32_t size);
void monRead(uint32_t bits, uint32_t addr);
void monWrite(uint32_t bits, uint32_t addr, uint32_t val);
void monLoad(uint32_t addr, uint32_t size);
void monRun(uint32_t addr);
void monTest(char* cmd, uint32_t val);


bool mon_Init()
{
    void mon_Main(regs_t* regs);
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
            else if (strcmp(argc[0], "r") == 0)         { monRegs(regs); }
            else if (strcmp(argc[0], "d") == 0)         { monDump(strtoi(argc[1]), strtoi(argc[2])); }
            else if (strcmp(argc[0], "rb") == 0)        { monRead( 8, strtoi(argc[1])); }
            else if (strcmp(argc[0], "rw") == 0)        { monRead(16, strtoi(argc[1])); }
            else if (strcmp(argc[0], "rl") == 0)        { monRead(32, strtoi(argc[1])); }
            else if (strcmp(argc[0], "wb") == 0)        { monWrite( 8, strtoi(argc[1]), strtoi(argc[2])); }
            else if (strcmp(argc[0], "ww") == 0)        { monWrite(16, strtoi(argc[1]), strtoi(argc[2])); }
            else if (strcmp(argc[0], "wl") == 0)        { monWrite(32, strtoi(argc[1]), strtoi(argc[2])); }
            else if (strcmp(argc[0], "load") == 0)      { monLoad(strtoi(argc[1]), strtoi(argc[2])); }
            else if (strcmp(argc[0], "run") == 0)       { monRun(strtoi(argc[1])); }
            else if (strcmp(argc[0], "reset") == 0)     { extern void vec_boot(); vec_boot(); }
            else if (strcmp(argc[0], "test") == 0)      { monTest((args > 1) ? argc[1] : 0, (args > 2) ? strtoi(argc[2]) : 0); }
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
         "  rb {addr}         : read byte\n"
         "  rw {addr}         : read word\n"
         "  rl {addr}         : read long\n"
         "  wb {addr} {val}   : write byte\n"
         "  ww {addr} {val}   : write word\n"
         "  wl {addr} {val}   : write long\n"
         "  d  {addr} {len}   : dump memory\n"
         "  r                 : show registers\n"
         "  x                 : exit monitor\n"
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

