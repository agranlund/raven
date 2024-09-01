#ifndef _MIDI_H_
#define _MIDI_H_
#ifndef __ASM__
#include "../sys.h"

bool        midi_Init();
bool        midi_txrdy();
bool        midi_rxrdy();
uint16_t    midi_sendbuf(uint8_t* data, uint16_t count);
bool        midi_send(uint8_t data);
uint8_t     midi_recv();

#endif //!__ASM__
#endif // _MFP_H_

