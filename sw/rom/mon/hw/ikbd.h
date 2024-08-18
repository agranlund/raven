#ifndef _IKBD_H_
#define _IKBD_H_
#include "../sys.h"
#ifndef __ASM__

#define IKBD_GPO_PWRLED     0
#define IKBD_GPO_TP307      1


bool    ikbd_Init();
void    ikbd_GPO(uint8 bit, uint8 output);

bool    ikbd_txrdy();
bool    ikbd_rxrdy();
uint16  ikbd_sendbuf(uint8* data, uint16 count);
bool    ikbd_send(uint8 data);
uint8   ikbd_recv();

#endif //!__ASM__
#endif // _IKBD_H_

