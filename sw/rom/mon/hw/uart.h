#ifndef _UART_H_
#define _UART_H_

#define UART_CLK            24000000

#define UART_DLL            0x00
#define UART_RHR            0x00
#define UART_THR            0x00
#define UART_DLM            0x04
#define UART_IER            0x04
#define UART_DLD            0x08
#define UART_ISR            0x08
#define UART_FCR            0x08
#define UART_EFR            0x08
#define UART_LCR            0x0C
#define UART_MCR            0x10
#define UART_LSR            0x14
#define UART_MSR            0x18
#define UART_TCR            0x18
#define UART_SPR            0x1C
#define UART_TLR            0x1C
#define UART_FRD            0x1C


#ifndef __ASM__

#include "sys.h"

bool        uart_Init();

bool        uart_txrdy();
bool        uart_rxrdy();
uint16_t    uart_sendbuf(uint8_t* data, uint16_t count);
bool        uart_send(uint8_t data);
uint8_t     uart_recv();

void        uart_sendChar(const char d);
int         uart_recvChar();

#endif //!__ASM__
#endif // _UART_H_

