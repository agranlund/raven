#include "uart.h"


//-----------------------------------------------------------------------
bool uart_Init()
{
    // rs232 already inited in boot.S
    return true; 
}

//-----------------------------------------------------------------------
bool uart_txrdy()
{
    return (IOB(PADDR_UART2, UART_LSR) & (1 << 5)) ? true : false;
}


//-----------------------------------------------------------------------
bool uart_rxrdy()
{
    return (IOB(PADDR_UART2, UART_LSR) & (1 << 0)) ? true : false;
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
    IOB(PADDR_UART2, UART_THR) = data;
    return true;
}


//-----------------------------------------------------------------------
uint8_t uart_recv()
{
    // todo: timeout?
    while (!uart_rxrdy()) { nop(); }
    return IOB(PADDR_UART2, UART_RHR);
}


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
    uint8_t lsr = IOB(PADDR_UART2, UART_LSR);
    return ((lsr & (1 << 0)) == 0) ? -1 : IOB(PADDR_UART2, UART_RHR);
}

