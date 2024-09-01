#include "uart.h"
#include "ikbd.h"

bool ikbd_Init()
{
    //-----------------------------------------------------------------------
    // uart1 : eiffel connection
    //-----------------------------------------------------------------------
    //  acia<->ikbd = 7812.5 bits/sec
    //  rdiv = (XTAL1 / prescaler) / (baud * 16)
    //  rdiv = (24*1024*1024) / (7812.5 * 16)
    //  rdiv = (24*1024*1024) / 125000
    //  rdiv = 201,326592 = 201
    //  DLM = (trunc(rdiv) >> 8) & 0xff          = 0
    //  DLL = trunc(rdiv) & 0xff                 = 201
    //  DLD = round((rdiv-trunc(trdiv)) * 16)    = 5
    //  without DLD = 7825 = 0,16% error
    //-----------------------------------------------------------------------
    volatile uint8_t* uart1 = (volatile uint8_t*) PADDR_UART1;
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
    uart1[UART_DLL] = 201;          // 7825 baud
    uart1[UART_LCR] &= 0x7F;        // normal regs
    return true; 
}


//-----------------------------------------------------------------------
void ikbd_GPO(uint8_t bit, uint8_t output)
{
    // 0 : powerled
    // 1 : TP301
    uint8_t mask = (1 << bit);
    if (output) {
        IOB(PADDR_UART1, UART_MCR) &= ~mask;
    } else {
        IOB(PADDR_UART1, UART_MCR) |= mask;
    }
}


//-----------------------------------------------------------------------
bool ikbd_txrdy()
{
    return (IOB(PADDR_UART1, UART_LSR) & (1 << 5)) ? true : false;
}


//-----------------------------------------------------------------------
bool ikbd_rxrdy()
{
    return (IOB(PADDR_UART1, UART_LSR) & (1 << 0)) ? true : false;
}


//-----------------------------------------------------------------------
uint16_t ikbd_sendbuf(uint8_t* data, uint16_t count)
{
   for (uint16_t i=0; i<count; i++) {
        if (!ikbd_send(data[i])) {
            return i;
        }
    }
    return count;
}


//-----------------------------------------------------------------------
bool ikbd_send(uint8_t data)
{
    // todo: timeout?
    while (!ikbd_txrdy()) {
    }
    IOB(PADDR_UART1, UART_THR) = data;
    return true;
}


//-----------------------------------------------------------------------
uint8_t ikbd_recv()
{
    // todo: timeout?
    while (!ikbd_rxrdy()) {
    }
    return IOB(PADDR_UART1, UART_RHR);
}

