#include "hw/uart.h"

// todo: include uart functions in bios exports

//-----------------------------------------------------------------------
bool uart_Init()
{
    // rs232 already inited in boot.S
    return true; 
}


//-----------------------------------------------------------------------
uint32_t uart_get_baud()
{
    static const uint32_t common_rates[] =
    {
        110, 150, 300, 600, 900, 1200, 2400, 3600, 4800,
        9600, 14400, 19200, 28800, 31250, 38400, 57600,
        100000, 115200, 230400, 250000, 460800,
        500000, 750000, 921600, 1000000,
    };

    // fetch precise baud rate from hardware registers
    uint32_t regs = 0;
    uint8_t lcr = IOB(RV_PADDR_UART2, UART_LCR);
    IOB(RV_PADDR_UART2, UART_LCR) = 0xBF;            // switch to enhanced regs
    IOB(RV_PADDR_UART2, UART_EFR) |= 0x10;           // unlock dld
    IOB(RV_PADDR_UART2, UART_LCR) = 0x80;            // switch to baud regs
    regs |= ((((uint32_t)IOB(RV_PADDR_UART2, UART_DLL)) & 0xff) <<  4);
    regs |= ((((uint32_t)IOB(RV_PADDR_UART2, UART_DLM)) & 0xff) << 12);
    regs |= ((((uint32_t)IOB(RV_PADDR_UART2, UART_DLD)) & 0x0f) <<  0);
    IOB(RV_PADDR_UART2, UART_LCR) = 0xBF;            // switch to enhanced regs
    IOB(RV_PADDR_UART2, UART_EFR) &= ~0x10;          // lock dld
    IOB(RV_PADDR_UART2, UART_LCR) = lcr;             // switch to normal regs
    uint32_t baud = (UART_CLK / regs);

    // try matching against common baud rates
    for (int i = (sizeof(common_rates)/sizeof(uint32_t)) - 1; i>=0; i--) {
        uint32_t tolerance = (common_rates[i] / 100); // 1%
        if ((baud >= (common_rates[i]-tolerance)) && (baud <= (common_rates[i]+tolerance))) {
            baud = common_rates[i];
            i = 0;
        }
    }

    return baud;
}

//-----------------------------------------------------------------------
void uart_set_baud(uint32_t baud)
{
    uint32_t regs = (((UART_CLK << 4) / baud) & 0x00ffff00UL);      /* mm.ll.00 */
    regs |= (((((UART_CLK << 1) / baud) + 1) >> 1) & 0x0000000fUL); /* mm.ll.dd */
    uint8_t lcr = IOB(RV_PADDR_UART2, UART_LCR);
    IOB(RV_PADDR_UART2, UART_LCR) = 0xBF;            // switch to enhanced regs
    IOB(RV_PADDR_UART2, UART_EFR) |= 0x10;           // unlock dld
    IOB(RV_PADDR_UART2, UART_LCR) = 0x80;            // switch to baud regs
    IOB(RV_PADDR_UART2, UART_DLL) = (uint8_t)(((regs >>  8) & 0xff));
    IOB(RV_PADDR_UART2, UART_DLM) = (uint8_t)(((regs >> 16) & 0xff));
    IOB(RV_PADDR_UART2, UART_DLD) = (uint8_t)(((regs >>  0) & 0x0f) );
    IOB(RV_PADDR_UART2, UART_LCR) = 0xBF;            // switch to enhanced regs
    IOB(RV_PADDR_UART2, UART_EFR) &= ~0x10;          // lock dld
    IOB(RV_PADDR_UART2, UART_LCR) = lcr;             // switch to normal regs
}

//-----------------------------------------------------------------------
bool uart_txrdy()
{
    return (IOB(RV_PADDR_UART2, UART_LSR) & (1 << 5)) ? true : false;
}


//-----------------------------------------------------------------------
bool uart_rxrdy()
{
    return (IOB(RV_PADDR_UART2, UART_LSR) & (1 << 0)) ? true : false;
}


//-----------------------------------------------------------------------
uint16_t uart_sendbuf(uint8_t* data, uint16_t count)
{
   for (uint16_t i=0; i<count; i++) {
        if (!uart_send(data[i])) {
            return i;
        }
    }
    return count;
}


//-----------------------------------------------------------------------
bool uart_send(uint8_t data)
{
    // todo: timeout?
    while (!uart_txrdy()) { nop(); }
    IOB(RV_PADDR_UART2, UART_THR) = data;
    return true;
}


//-----------------------------------------------------------------------
uint8_t uart_recv()
{
    // todo: timeout?
    while (!uart_rxrdy()) { nop(); }
    return IOB(RV_PADDR_UART2, UART_RHR);
}


//-----------------------------------------------------------------------
void uart_sendChar(const char d)
{
    if (d == '\n')
        uart_sendChar('\r');
    while ((IOB(RV_PADDR_UART2, UART_LSR) & (1 << 5)) == 0)
        nop();
    IOB(RV_PADDR_UART2, UART_THR) = d;

    /* line-buffered */
    while ((d == '\n') && (IOB(RV_PADDR_UART2, UART_LSR) & (1 << 6)) == 0)
        nop();
}

int uart_recvChar()
{
    uint8_t lsr = IOB(RV_PADDR_UART2, UART_LSR);
    return ((lsr & (1 << 0)) == 0) ? -1 : IOB(RV_PADDR_UART2, UART_RHR);
}

