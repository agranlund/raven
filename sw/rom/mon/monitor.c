#include "sys.h"

#define MONBUFFERSIZE 1024
uint8 monBuffer[MONBUFFERSIZE];
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


__attribute__((interrupt)) void IntIkbd(void)
{
    uint8 c = IOB(PADDR_UART1, UART_RHR);
    uart_printHex(8, "ikbd int: ", c, "\n");
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
    uart_printString("\n");
    monRegs(regs);

    uint16 exit = 0;
    while(exit == 0)
    {
        uart_printString("> ");

        uint16 monBufferSize = 0;
        uint16 done = 0;
        while (!done)
        {
            uint16 echo = 1;
            volatile uint8 lsr = IOB(PADDR_UART2, UART_LSR);
            if ((lsr & (1 << 0)) != 0)
            {
                volatile uint8 c = IOB(PADDR_UART2, UART_RHR);
                if (c != '\r')
                {
                    if (c == 8) {
                        // backspace
                        if (monBufferSize > 0)
                            monBufferSize -= 1;
                        else
                            echo = 0;
                    } else if (c == '\n') {
                        // newline
                        done = 1;
                    } else if (c >= 32 && c <= 126) {
                        // character
                        monBuffer[monBufferSize] = c;
                        monBufferSize++;
                        if (monBufferSize >= (MONBUFFERSIZE-1))
                            done = 1;
                    } else {
                        echo = 0;
                    }

                    if (echo) {
                        uart_printChar(c);
                    }
                }
            }

/*
            lsr = IOB(PADDR_UART1, UART_LSR);
            if (lsr & (1 << 0) != 0)
            {
                volatile uint8 c = IOB(PADDR_UART1, UART_RHR);
                uart_printHex(8, "IKBD: ", c, "\n");
            }
*/
        }

        // cleanup input buffer
        monBuffer[monBufferSize] = 0;
        for (int i=0; i<monBufferSize; i++) {
            if (monBuffer[i] <= 32 || monBuffer[i] >= 127) {
                monBuffer[i] = 0;
            }
        }

        // parse command arguments
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
            else                                        { monHelp(); }
        }
    }
    monActive = 0;
}

void monHelp()
{
    uart_printString("\n");
    uart_printString("Commands:\n");
    uart_printString("  rb {addr}         : read byte\n");
    uart_printString("  rw {addr}         : read word\n");
    uart_printString("  rl {addr}         : read long\n");
    uart_printString("  wb {addr} {val}   : write byte\n");
    uart_printString("  ww {addr} {val}   : write word\n");
    uart_printString("  wl {addr} {val}   : write long\n");
    uart_printString("  d  {addr} {len}   : dump memory\n");
    uart_printString("  r                 : show registers\n");
    uart_printString("  x                 : exit monitor\n");
    uart_printString("  reset             : reset computer\n");
    uart_printString("\n");
}

void monRegs(TRegs* regs)
{
    if (!regs)
        return;
    uart_printString("\n");
    uart_printHex(32, " d0: ", regs->d0,    " ");
    uart_printHex(32, " d2: ", regs->d2,    " ");
    uart_printHex(32, " d4: ", regs->d4,    " ");
    uart_printHex(32, " d6: ", regs->d6,    " ");
    uart_printString("\n");
    uart_printHex(32, " d1: ", regs->d1,    " ");
    uart_printHex(32, " d3: ", regs->d3,    " ");
    uart_printHex(32, " d5: ", regs->d5,    " ");
    uart_printHex(32, " d7: ", regs->d7,    " ");
    uart_printString("\n");
    uart_printHex(32, " a0: ", regs->a0,    " ");
    uart_printHex(32, " a2: ", regs->a2,    " ");
    uart_printHex(32, " a4: ", regs->a4,    " ");
    uart_printHex(32, " a6: ", regs->a6,    " ");
    uart_printString("\n");
    uart_printHex(32, " a1: ", regs->a1,    " ");
    uart_printHex(32, " a3: ", regs->a3,    " ");
    uart_printHex(32, " a5: ", regs->a5,    " ");
    uart_printHex(32, " a7: ", regs->a7,    " ");
    uart_printString("\n");
    uart_printHex(32, " pc: ", regs->pc,    " ");
    uart_printHex(16, " sr: ", regs->sr,    " ");
    uart_printString( "                  ");
    uart_printHex(32, "usp: ", regs->usp,   " ");
    uart_printString("\n");
    uart_printHex(32, "vbr: ", regs->vbr,   " ");
    uart_printHex(32, " tc: ", regs->tc,    " ");
    uart_printHex(32, "srp: ", regs->srp,   " ");
    uart_printHex(32, "urp: ", regs->urp,   " ");
    uart_printString("\n");
    uart_printHex(32, "dt0: ", regs->dtt0,  " ");
    uart_printHex(32, "dt1: ", regs->dtt1,  " ");
    uart_printHex(32, "it0: ", regs->itt0,  " ");
    uart_printHex(32, "it1: ", regs->itt1,  " ");
    uart_printString("\n");
    uart_printHex(32, "pcr: ", regs->pcr,   " ");
    uart_printHex(32, "bcr: ", regs->buscr, " ");
    uart_printHex(32, "ccr: ", regs->cacr,  " ");
    uart_printString("\n");
    uart_printString("\n");
}

void monDump(uint32 addr, uint32 size)
{
    const char* ascii = "................................ !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_<`abcdefghijklmnopqrstuvwxyz{|}~.................................................................................................................................";
    const uint32 sizeDefault = 256;
    const uint32 sizeMin = 16;
    const uint32 sizeMax = 16*256;
    if (size == 0)              size = sizeDefault;
    else if (size < sizeMin)    size = sizeMin;
    else if (size > sizeMax)    size = sizeMax;

    uint8 lineRaw[16];
    uint8 lineStr[17];
    lineStr[16] = 0;

    uint32 end = addr + size;
    while (addr < end)
    {
        for (int i=0; i<16; i++) {
            uint8 b = IOB(addr, i);
            lineRaw[i] = b;
            lineStr[i] = ascii[b];
        }
        uart_printHex(32, 0, addr, " : ");
        for (int i=0; i<16; i+=4) {
            uart_printHex(8, 0, lineRaw[i+0], 0);
            uart_printHex(8, 0, lineRaw[i+1], 0);
            uart_printHex(8, 0, lineRaw[i+2], 0);
            uart_printHex(8, 0, lineRaw[i+3], " ");
        }
        uart_printString((char*)lineStr);
        uart_printChar('\n');
        addr += 16;
    }
}

void monRead(uint32 bits, uint32 addr)
{
    uart_printHex(32, 0, addr, " : ");
    switch (bits)
    {
        case 8: {
            uint8 v = IOB(addr, 0);
            uart_printHex(8, "$", v, "\n");
        } break;
        case 16: {
            uint16 v = IOW(addr, 0);
            uart_printHex(16, "$", v, "\n");
        } break;
        case 32: {
            uint32 v = IOL(addr, 0);
            uart_printHex(32, "$", v, "\n");
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

