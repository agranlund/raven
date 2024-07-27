#include "sys.h"
#include "lib.h"

extern int test_ym_write(int size);

#define MONBUFFERSIZE 1024
char monBuffer[MONBUFFERSIZE];
uint16 monActive;

typedef struct
{
    uint32 buscr;
    uint32 itt1;
    uint32 itt0;
    uint32 dtt1;
    uint32 dtt0;
    uint32 tc;
    uint32 srp;
    uint32 urp;
    uint32 cacr;
    uint32 dfc;
    uint32 sfc;
    uint32 vbr;
    uint32 pcr;
    uint32 usp;
    uint32 d0,d1,d2,d3,d4,d5,d6,d7;
    uint32 a0,a1,a2,a3,a4,a5,a6,a7;
    uint16 sr;
    uint32 pc;
} TRegs;

void monHelp();
void monRegs(TRegs* regs);
void monDump(uint32 addr, uint32 size);
void monRead(uint32 bits, uint32 addr);
void monWrite(uint32 bits, uint32 addr, uint32 val);
void monLoad(uint32 addr, uint32 size);
void monRun(uint32 addr);
void monTest(char* cmd, uint32 val);

__attribute__((interrupt)) void IntIkbd(void)
{
    uint8 c = IOB(PADDR_UART1, UART_RHR);
    fmt("ikbd int: %b\n", c);
}

void InitMonitor()
{
    monActive = 0;
/*
    *((uint32*)0x74) = (uint32)IntIkbd;
    //IOB(PADDR_UART1, UART_FCR) &= 0xFE;     // fifo off
    IOB(PADDR_UART1, UART_IER) = 0x01;      // RHR interrupt on
    SetIPL(5);
*/
}

void Monitor(TRegs* regs)
{
    if (monActive)
        return;

    monActive = 1;
    puts("");
    monRegs(regs);

    uint16 exit = 0;
    while(exit == 0)
    {
        putchar('>');
        putchar(' ');

        if (gets(monBuffer, sizeof(monBuffer)) == NULL)
            continue;
        size_t monBufferSize = strlen(monBuffer);

        // parse command arguments
        // XXX TODO use scan()
        int args = 0;
        char* argc[8];

        uint32 start = 0;
        for (int i=0; i<8; i++) {
            argc[i] = 0;
            // skip whitespaces
            while ((start < monBufferSize) && (monBuffer[start] == 0)) {
                start++;
            }
            // find length
            if (start < monBufferSize) {
                uint32 end = start;
                while ((end < monBufferSize) && (monBuffer[end] != 0)) {
                    end++;
                }
                uint32 size = end - start;
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
    monActive = 0;
}

void monTest(char* test, uint32 val) {
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

void monRegs(TRegs* regs)
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

void monDump(uint32 addr, uint32 size)
{
    const uint32 sizeDefault = 256;
    const uint32 sizeMin = 16;
    const uint32 sizeMax = 16*256;
    if (size == 0)              size = sizeDefault;
    else if (size < sizeMin)    size = sizeMin;
    else if (size > sizeMax)    size = sizeMax;

    hexdump((uint8 *)addr, addr, size, 'b');
}

void monRead(uint32 bits, uint32 addr)
{
    fmt("%l : ", addr);
    switch (bits)
    {
        case 8: {
            uint8 v = IOB(addr, 0);
            fmt("$%b\n", v);
        } break;
        case 16: {
            uint16 v = IOW(addr, 0);
            fmt("$%w\n", v);
        } break;
        case 32: {
            uint32 v = IOL(addr, 0);
            fmt("$%l\n", v);
        } break;
    }
}

void monWrite(uint32 bits, uint32 addr, uint32 val)
{
    switch (bits)
    {
        case 8:
            IOB(addr, 0) = (uint8) val;
            break;
        case 16:
            IOW(addr, 0) = (uint16) val;
            break;
        case 32:
            IOL(addr, 0) = (uint32) val;
            break;
    }
}

void monLoad(uint32 addr, uint32 size)
{
    uint8* dst = (uint8*)addr;
    while (size > 0)
    {
        volatile uint8 lsr = IOB(PADDR_UART2, UART_LSR);
        if ((lsr & (1 << 0)) != 0)
        {
            *dst = IOB(PADDR_UART2, UART_RHR);
            dst++; size--;
        }
    }
}

void monRun(uint32 addr)
{
    monActive = 0;
    Call(addr);
}

