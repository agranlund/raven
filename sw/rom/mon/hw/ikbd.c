#include "hw/uart.h"
#include "hw/ikbd.h"

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

typedef struct
{
    uint8_t dlm;
    uint8_t dll;
    uint8_t dld;
    uint8_t pad;
} ikbd_baud_regs_t;

const ikbd_baud_regs_t ikbd_baud_regs[8] =
{  //  dlm  dll  dld
    { 0x00, 192, 0x00 },    // 7812
    { 0x00,  96, 0x00 },    // 15625
    { 0x00,  48, 0x00 },    // 31250
    { 0x00,  24, 0x00 },    // 62500
    { 0x00,  12, 0x00 },    // 125000
    { 0x00,   6, 0x00 },    // 250000
    { 0x00,   3, 0x00 },    // 500000
    { 0x00,   1, 0x08 },    // 1000000
};

static uint8_t ikbd_baud;       // according to table above
static uint32_t ikbd_version;   // yymmddtt (tt = Ex for Eiffel, Cx for Ckbd)

bool ikbd_Init(uint8_t baud)
{
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

    ikbd_baud = baud & 0x7;
    uart1[UART_DLM] = ikbd_baud_regs[ikbd_baud].dlm;
    uart1[UART_DLL] = ikbd_baud_regs[ikbd_baud].dll;
    uart1[UART_DLD] = ikbd_baud_regs[ikbd_baud].dld;

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


//-----------------------------------------------------------------------
void ikbd_Info()
{
    // flush input
    while (ikbd_rxrdy()) {
        ikbd_recv();
    }

    // send eiffel get_temp command
    uint8_t buf[24];
    ikbd_send(0x03);

    // retrieve up to 24 bytes with timeout
    uint8_t size = 0;
    uint32_t silent = 0;
    uint32_t timeout = 1000000;
    while ((size < 24) && (silent < timeout)) {
        silent++;
        if (ikbd_rxrdy()) {
            silent = 0;
            buf[size++] = ikbd_recv();
        }
    }

    printf("size %d : ", size);
    if (size < 8) {
        printf("unknown\n");
    } else if (size < 16) {
        printf("eiffel\n");
    } else {
        printf("ckbd\n");
    }

    for (int i=0; i<size; i+=8) {
        printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",
            buf[i+0], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
    }
}

//-----------------------------------------------------------------------
void ikbd_Reset(bool bootloader)
{
    if (bootloader) {
        ikbd_send(0x2D);    // ckbd reset to bootloader
    } else {
        ikbd_send(0x80);    // ikbd reset
        ikbd_send(0x01);
    }
}



//-----------------------------------------------------------------------
void ikbd_Connect(uint8_t baud)
{
    // change baud rate if necessary
    baud &= 0x7;
    if (baud != ikbd_baud) {
        volatile uint8_t* uart1 = (volatile uint8_t*) RV_PADDR_UART1;
        uint8_t ier = uart1[UART_IER];  // save interrupt register
        uart1[UART_IER] = 0x00;         // disable interrupts
        uart1[UART_LCR] = 0x00;         // access normal regs
        uart1[UART_FCR] = 0x01;         // fifo enabled
        uart1[UART_FCR] = 0x07;         // clear fifo buffers

        uart1[UART_LCR] = 0xBF;         // access efr
        uart1[UART_EFR] = 0x10;         // enable dld access
        uart1[UART_LCR] = 0x80;         // access baud regs

        uart1[UART_DLM] = ikbd_baud_regs[baud].dlm;
        uart1[UART_DLL] = ikbd_baud_regs[baud].dll;
        uart1[UART_DLD] = ikbd_baud_regs[baud].dld;

        uart1[UART_LCR] = 0xBF;         // access efr
        uart1[UART_EFR] = 0x00;         // latch and disable dld access
        uart1[UART_LCR] = 0x03;         // 8 data bits, no parity, 1 stop bit
        uart1[UART_IER] = ier;          // restore interrupt register
        ikbd_baud = baud;
    }

    // identify ikbd type and get version info
    ikbd_Info();    // todo
    ikbd_version = 0x00000000;  // yy.mm.dd.type (Ex for Eiffel, Cx for Ckbd, 00 for unknown)
}
