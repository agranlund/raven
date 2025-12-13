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
    uint32_t speed;
} ikbd_baud_regs_t;

const ikbd_baud_regs_t ikbd_baud_regs[8] =
{  //  dlm  dll  dld
    { 0x00, 192, 0x00,    7812 },
    { 0x00,  96, 0x00,   15625 },
    { 0x00,  48, 0x00,   31250 },
    { 0x00,  24, 0x00,   62500 },
    { 0x00,  12, 0x00,  125000 },
    { 0x00,   6, 0x00,  250000 },
    { 0x00,   3, 0x00,  500000 },
    { 0x00,   1, 0x08, 1000000 },
};

static uint8_t ikbd_baud;       // according to table above
static uint32_t ikbd_version;   // ckbd: YYMMDDXX, eiffel: 00000001

static inline uint32_t ckbd_version(void) {
    return ((ikbd_version & 0xf0) != 0) ? ikbd_version : 0;
}

static inline uint32_t eiffel_version() {
    return ((ikbd_version & 0xf0) == 0) ? ikbd_version : 0;
}

bool ikbd_Init()
{
    ikbd_version = 0;
    ikbd_baud = IKBD_BAUD_7812;

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

uint8_t ikbd_SetBaud(uint8_t baud)
{
    baud &= 0x7;
    uint8_t oldbaud = ikbd_baud;
    if (baud != oldbaud) {
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
    return oldbaud;
}

uint32_t ikbd_Connect(uint8_t baud)
{
    #define connect_retries     3
    #define timeout_long        1000000
    #define timeout_short       100000

    ikbd_SetBaud(baud);
    ikbd_version = 0x00000000;

    bool connected = false;
    for (int i=0; i<connect_retries && !connected; i++)
    {
        // flush inputs
        while (ikbd_rxrdy()) {
            ikbd_recv();
        }
        // send reset command
        ikbd_send(0x80);
        ikbd_send(0x01);
        // wait for ack
        for (int j=0; j<timeout_long && !connected; j++) {
            if (ikbd_rxrdy()) {
                (void)ikbd_recv();
                connected = true;
            }
        }
    }

    uint8_t infosize = 0;
    uint8_t infodata[32];
    if (connected) {
        // pause ikbd and wait for silence
        ikbd_send(0x13);
        sys_Delay(100);
        while (ikbd_rxrdy()) {
            ikbd_recv();
            sys_Delay(100);
        }
        // send eiffel get_temp command
        ikbd_send(0x03);
        // retrieve up to 24 bytes with timeout
        uint32_t silent = 0;
        uint32_t timeout = timeout_long;
        while ((infosize < 24) && (silent < timeout)) {
            silent++;
            if (ikbd_rxrdy()) {
                silent = 0;
                timeout = timeout_short;
                infodata[infosize++] = ikbd_recv();
            }
        }
        // resume ikbd and wait for silence
        ikbd_send(0x11);
        sys_Delay(100);
        while (ikbd_rxrdy()) {
            ikbd_recv();
            sys_Delay(100);
        }
    }

    if (infosize < 8) {
        ikbd_version = 0x00000000;
    } else if (infosize < 24) {
        ikbd_version = 0x00000001;
    } else {
        ikbd_version = (infodata[21] << 24) | (infodata[22] << 16) | (infodata[23] << 8) | (infodata[18] << 0);
    }
    return ikbd_version;
}

uint32_t ikbd_ConnectEx(uint8_t default_baud, uint8_t ideal_baud)
{
    // connect with default or warmboot baudrate
    ikbd_Connect(default_baud);
    if ((ikbd_version == 0) && (default_baud != IKBD_BAUD_7812)) {
        // retry with standard rate if default failed
        sys_Delay(100);
        default_baud = IKBD_BAUD_7812;
        ikbd_Connect(default_baud);
    }

    // negotiate higher connection speed if we are connected with a CKBD at 7812bps
    if ((ikbd_version & 0xF0) && (default_baud != ideal_baud)) {
        sys_Delay(100);
        ikbd_WriteSetting(0xFE, ideal_baud);
        sys_Delay(100);
        if (!ikbd_Connect(ideal_baud)) {
            sys_Delay(100);
            ikbd_Connect(default_baud);
        }
    }
    return ikbd_version;
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
uint32_t ikbd_Version(void)
{
    return ikbd_version;
}

//-----------------------------------------------------------------------
uint32_t ikbd_Info(void)
{
    if (ckbd_version()) {
        printf("CKBD.%08x %dbps\n", ikbd_version, ikbd_baud_regs[ikbd_baud].speed);
    } else if (eiffel_version()) {
        printf("Eiffel %dbps\n", ikbd_baud_regs[ikbd_baud].speed);
    } else {
        printf("Unknown IKBD\n");
    }
    return ikbd_version;
}

uint8_t ikbd_Baud(void)
{
    return ikbd_baud;
}

void ikbd_Reset(void)
{
    ikbd_send(0x80);
    ikbd_send(0x01);
}

//-----------------------------------------------------------------------
void ikbd_SystemPoweroff(void)
{
    if (ckbd_version()) {
        ikbd_send(0x2F);
        ikbd_send(0x5A);
    }
}

//-----------------------------------------------------------------------
void ikbd_HardReset(bool bootloader)
{
    if (!ckbd_version()) {
        printf("requires ckbd controller\n");
    } else if (bootloader) {
        ikbd_send(0x13);
        while(ikbd_rxrdy()) { ikbd_recv(); }
        ikbd_send(0x2E);
        ikbd_send(0x5A);
        sys_Delay(1000);
        uint32_t prevbaud = ikbd_baud;
        ikbd_SetBaud(IKBD_BAUD_7812);
        ikbd_recv();
        ikbd_ConnectEx(IKBD_BAUD_7812, prevbaud);
        ikbd_Info();
    } else {
        ikbd_send(0x13);
        ikbd_send(0x2E);
        ikbd_send(0x00);
        sys_Delay(100);
        uint32_t prevbaud = ikbd_baud;
        ikbd_SetBaud(IKBD_BAUD_7812);
        sys_Delay(1000);
        ikbd_ConnectEx(IKBD_BAUD_7812, prevbaud);
        ikbd_Info();
    }
}

void ikbd_WriteSetting(uint8_t idx, uint8_t val)
{
    if (!ckbd_version()) {
        printf("requires ckbd controller\n");
    } else {
        ikbd_send(0x2B);
        ikbd_send(idx);
        ikbd_send(val);
    }
}

void ikbd_ReadSetting(uint8_t idx)
{
    if (!ckbd_version()) {
        printf("requires ckbd controller\n");
    } else {
        // pause ikbd and wait for silence
        ikbd_send(0x13);
        sys_Delay(100);
        while (ikbd_rxrdy()) {
            ikbd_recv();
            sys_Delay(100);
        }
        // request setting
        ikbd_send(0x2A);
        ikbd_send(idx);
        // retrieve up to 8 bytes with timeout
        uint32_t silent = 0;
        uint8_t infosize = 0;
        uint8_t infodata[8];
        while ((infosize < 8) && (silent < 100000)) {
            silent++;
            if (ikbd_rxrdy()) {
                silent = 0;
                infodata[infosize++] = ikbd_recv();
            }
        }
        // resume ikbd and wait for silence
        ikbd_send(0x11);
        sys_Delay(100);
        while (ikbd_rxrdy()) {
            ikbd_recv();
            sys_Delay(100);
        }
        //printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", infodata[0], infodata[1], infodata[2], infodata[3], infodata[4], infodata[5], infodata[6], infodata[7]);
        if ((infodata[0] == 0xF6) && (infodata[1] == 0x2C)) {
            printf("%02x : %02x %02x %02x %02x\n", infodata[3], infodata[4], infodata[5], infodata[6], infodata[7]);
        } else {
            printf("fail [%02x%02x%02x%02x%02x%02x%02x%02x]\n", infodata[0], infodata[1], infodata[2], infodata[3], infodata[4], infodata[5], infodata[6], infodata[7]);
        }
    }
}

void ikbd_ClearSettings(void)
{
    ikbd_WriteSetting(0xFF, 0x5A);
}

void ikbd_SaveSettings(void)
{
    ikbd_WriteSetting(0xFF, 0x00);
}

void ikbd_TargetBaud(uint8_t baud)
{
    ikbd_WriteSetting(0xFE, baud&7);
}

void ikbd_Flash(uint8_t* data, uint32_t size)
{
    if (!ckbd_version()) {
        printf("requires ckbd controller\n");
        return;
    }

    uint16_t block_count = ((size+1023) >> 10);
    if (block_count > 64) {
        printf("image too large (%d / 64 blocks)\n", block_count, 64);
        return;
    }

    printf("flashing %d blocks\n", block_count);
    ikbd_send(0x13);    // pause output
    sys_Delay(1000);
    while (ikbd_rxrdy()) {
        ikbd_recv();
    }

    // flash start
    ikbd_send(0x2C);
    ikbd_send(0x55);
    ikbd_send(0xAA);
    ikbd_send(0x00);
    ikbd_send(0x00);

    while(1) {
        // wait for request
        uint16_t block = (uint16_t)ikbd_recv();
        if (block >= block_count) {
            ikbd_send(0xFF);
            break;
        }
        // acknowledge
        ikbd_send((uint8_t)block);
        // wait for ready
        ikbd_recv();
        // send all the bytes
        for (int j=0; j<1024; j++) {
            ikbd_send(data[(block<<10)+j]);
        }
        printf(".");
    }

    // wait for ckbd to reset and reconnect
    printf("\ndone\n");
    sys_Delay(1000);
    ikbd_ConnectEx(IKBD_BAUD_7812, ikbd_baud);
    ikbd_Info();
}
