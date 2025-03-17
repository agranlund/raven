#include "hw/uart.h"
#include "hw/ikbd.h"

bool ikbd_Init()
{
    //-----------------------------------------------------------------------
    // uart1 : eiffel connection
    //-----------------------------------------------------------------------
    //  acia<->ikbd = 7812.5 bits/sec
    //  rdiv = (XTAL1 / prescaler) / (baud * 16)
    //  rdiv = (24*1000*1000) / (7812.5 * 16)
    //  rdiv = (24*1000*1000) / 125000
    //  rdiv = 192
    //  DLM = (trunc(rdiv) >> 8) & 0xff          = 0
    //  DLL = trunc(rdiv) & 0xff                 = 192
    //  DLD = round((rdiv-trunc(trdiv)) * 16)    = 0
    //-----------------------------------------------------------------------
    volatile uint8_t* uart1 = (volatile uint8_t*) RV_PADDR_UART1;

    uart1[UART_LCR] = 0x00;         // access normal regs
    uart1[UART_IER] = 0x00;         // disable interrupts
    uart1[UART_FCR] = 0x01;         // fifo enabled
    uart1[UART_FCR] = 0x07;         // clear fifo buffers
    uart1[UART_SPR] = 0x00;         // clear scratch register
    uart1[UART_MCR] = 0x00;         // modem control)
                                        // bit0 = #dtr -> powerled on/off
                                        // bit1 = #rts -> spare output TP301

    uart1[UART_LCR] = 0xBF;         // access efr
    uart1[UART_EFR] = 0x10;         // enable access

    uart1[UART_LCR] = 0x80;         // access baud regs
    uart1[UART_DLM] = 0;

    uart1[UART_DLL] = 192;          // 7812,5 baud
    //uart1[UART_DLL] = 96;           // 15625 baud
    //uart1[UART_DLL] = 48;           // 31250 baud

    uart1[UART_DLD] = 0;

    uart1[UART_MCR] |= (1 << 6);    // enable tcr/tlr access
    uart1[UART_TCR] = 0x00;         // rx fifo halt/resume
    uart1[UART_TLR] = 0x11;         // rx/tx fifo trigger level (4/4)
    uart1[UART_MCR] &= ~(1 << 6);   // diable tcr/tlr access

    uart1[UART_LCR] = 0xBF;         // access efr
    uart1[UART_EFR] = 0x00;         // latch and disable access

    uart1[UART_LCR] = 0x03;         // 8 data bits, no parity, 1 stop bit
    return true; 
}


//-----------------------------------------------------------------------
void ikbd_GPO(uint8_t bit, bool enable)
{
    // 0 : powerled
    // 1 : TP301
    uint8_t mask = (1 << bit);
    if (enable) {
        IOB(RV_PADDR_UART1, UART_MCR) &= ~mask;
    } else {
        IOB(RV_PADDR_UART1, UART_MCR) |= mask;
    }
}

bool ikbd_GPI(uint8_t bit)
{
    return 0;
}

//-----------------------------------------------------------------------
bool ikbd_txrdy()
{
    return (IOB(RV_PADDR_UART1, UART_LSR) & (1 << 5)) ? true : false;
}


//-----------------------------------------------------------------------
bool ikbd_rxrdy()
{
    return (IOB(RV_PADDR_UART1, UART_LSR) & (1 << 0)) ? true : false;
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
    IOB(RV_PADDR_UART1, UART_THR) = data;
    return true;
}


//-----------------------------------------------------------------------
uint8_t ikbd_recv()
{
    // todo: timeout?
    while (!ikbd_rxrdy()) {
    }
    return IOB(RV_PADDR_UART1, UART_RHR);
}

