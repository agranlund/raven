#ifndef _MIDI_H_
#define _MIDI_H_
#ifndef __ASM__
#include "../sys.h"

bool    midi_Init();
bool    midi_txrdy();
bool    midi_rxrdy();
uint16  midi_sendbuf(uint8* data, uint16 count);
bool    midi_send(uint8 data);
uint8   midi_recv();

#endif //!__ASM__
#endif // _MFP_H_

