#include "sys.h"
#include "lib.h"

extern uint32 GetPCR();

#define KMEMSIZE  512 * 1024
uint8 kheap[KMEMSIZE];
uint32 kheapPtr;
uint8 kgfx;


//-----------------------------------------------------------------------
//
// hardware init
//
//-----------------------------------------------------------------------
void sys_Init()
{
    // uart1
    //      eiffel
    // mfp1
    //      
    // mfp2
    //      



    volatile uint8* uart1 = (volatile uint8*) PADDR_UART1;
    uart1[UART_LCR] &= 0x7F;        // normal regs
    uart1[UART_IER] = 0x00;         // disable interrupts
    uart1[UART_FCR] = 0x01;         // fifo enabled
    uart1[UART_FCR] = 0x07;         // clear fifo buffers
    uart1[UART_LCR] = 0x03;         // 8 data bits, no parity, 1 stop bit
    uart1[UART_SPR] = 0x00;         // clear scratch register
    uart1[UART_MCR] = 0x00;         // modem control)
                                        // bit0 = #dtr -> powerled on/off
                                        // bit1 = #rts -> spare output TP301
    uart1[UART_LCR] |= 0x80;        // baud regs
    uart1[UART_DLM] = 0;
    uart1[UART_DLL] = 201;
    uart1[UART_LCR] &= 0x7F;        // normal regs

    // acia<->ikbd = 7812.5 bits/sec
    // rdiv = (XTAL1 / prescaler) / (baud * 16)
    // rdiv = (24*1024*1024) / (7812.5 * 16)
    // rdiv = (24*1024*1024) / 125000
    // rdiv = 201,326592 = 201
    // DLM = (trunc(rdiv) >> 8) & 0xff          = 0
    // DLL = trunc(rdiv) & 0xff                 = 201
    // DLD = round((rdiv-trunc(trdiv)) * 16)    = 5
    // without DLD = 7825 = 0,16% error


    // fake acia regs
    uint8* regemu = (uint8*)0x00000C00;
    for (uint32 i=0; i<256; i++) {
        regemu[i] = 0;
    }
    regemu[0] = 0x2;        // ikbd tx reg empty
    regemu[4] = 0x2;        // midi tx reg empty
}

void uart1_gpo(uint8 bit, uint8 output)
{
    // bit0 = powerled enable
    // bit1 = unused TP301
    uint8 mask = (1 << bit);
    if (output)
        IOB(PADDR_UART1, UART_MCR) &= ~mask;
    else
        IOB(PADDR_UART1, UART_MCR) |= mask;
}


//-----------------------------------------------------------------------
//
// kmemory
//
//-----------------------------------------------------------------------
void kmem_Init()
{
    kheapPtr = ((uint32)&kheap[0] + KMEMSIZE - 4) & ~16UL;
}

uint32 kmem_Alloc(uint32 size, uint32 alignment)
{
    if (alignment < 4)
        alignment = 4;

    uint32 m = ((kheapPtr - size) & ~(alignment - 1));
    if (m >= (uint32) &kheap[0])
    {
        kheapPtr = m;
        return kheapPtr;
    }
    puts("ERROR: kmem_Alloc()");
    return 0;
}


//-----------------------------------------------------------------------
//
// cpu
//
//-----------------------------------------------------------------------
uint32 DetectCPU(uint32* revout, uint32* idout)
{
    uint32 sku = ECPUSKU_UNKNOWN;
    uint32 rev = 1;

    uint32 pcr = GetPCR();
    rev = (pcr >> 8) & 0xFF;
    if ((pcr & 0x000f0000) == 0)
        sku = ECPUSKU_RC;
    else if (rev == 4)
        sku = ECPUSKU_LC;
    else if (rev == 3)
        sku = ECPUSKU_EC;

    if (idout)
        *idout = pcr & 0xffffff00;

    if (revout)
        *revout = rev;

    return sku;
}


//-----------------------------------------------------------------------
//
// uart
//
//-----------------------------------------------------------------------
void uart_sendChar(const char d)
{
    if (d == '\n')
        uart_sendChar('\r');
    while ((IOB(PADDR_UART2, UART_LSR) & (1 << 5)) == 0)
        nop();
    IOB(PADDR_UART2, UART_THR) = d;

    /* line-buffered */
    while ((d == '\n') && (IOB(PADDR_UART2, UART_LSR) & (1 << 6)) == 0)
        nop();
}

int uart_recvChar()
{
    uint8 lsr = IOB(PADDR_UART2, UART_LSR);

    return ((lsr & (1 << 0)) == 0) ? -1 : IOB(PADDR_UART2, UART_RHR);
}

//-----------------------------------------------------------------------
//
// misc helpers
//
//-----------------------------------------------------------------------
uint32 strtoi(char* s)
{
    uint32 v = 0;
    if (*s == '$')
    {
        s++;
        while (*s != 0)
        {
            uint32 b = (uint32) *s; s++;
            if (b >= '0' && b <= '9')       b = b - '0';
            else if (b >= 'a' && b <= 'f')  b = 10 + b - 'a';
            else return 0;
            v <<= 4;
            v |= b;
        }
    }
    else
    {
        while (*s != 0)
        {
            uint32 b = (uint32) *s; s++;
            if (b >= '0' && b <= '9')   b = b - '0';
            else return 0;
            v *= 10;
            v += b;
        }
    }
    return v;
}
