#include "sys.h"
#include "hw/midi.h"
#include "hw/mfp.h"

bool midi_Init()
{
    // MFP2:TimerD @ 62500hz / 32150 baud
    volatile uint8_t *mfp2 = (volatile uint8_t*) RV_PADDR_MFP2;
	mfp2[MFP_UCR]   = 0x08;		// divide by 1, 8 bits, no parity, 1 start bit , 1 stop bit
    mfp2[MFP_RSR]   = 0x01;     // rx enable
    mfp2[MFP_TSR]   = 0x01;     // tx enable
    mfp2[MFP_TCDCR] &= 0xF0;    // disable timerD
    mfp2[MFP_TDDR]  = 2;        // divide by 2
    mfp2[MFP_TCDCR] |= 0x03;    // divide by 16
    return true;
}

bool midi_txrdy()
{
    return (IOB(RV_PADDR_MFP2, MFP_TSR) & 0x80) ? true : false;
}

bool midi_rxrdy()
{
    return (IOB(RV_PADDR_MFP2, MFP_RSR) & 0x80) ? true : false;
}

uint16_t midi_sendbuf(uint8_t* data, uint16_t count)
{
    for (uint16_t i=0; i<count; i++) {
        if (!midi_send(data[i])) {
            return i;
        }
    }
    return count;
}

bool midi_send(uint8_t data)
{
    // todo: timeout?
    while (!midi_txrdy()) {
    }
    IOB(RV_PADDR_MFP2, MFP_UDR) = data;
    return true;
}

uint8_t midi_recv()
{
    // todo: timeout?
    while (!midi_rxrdy()) {
    }
    return IOB(RV_PADDR_MFP2, MFP_UDR);
}

