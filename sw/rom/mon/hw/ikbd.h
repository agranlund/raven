#ifndef _IKBD_H_
#define _IKBD_H_
#ifndef __ASM__
#include "sys.h"

#define IKBD_GPO_PWRLED     0
#define IKBD_GPO_TP307      1

bool        ikbd_Init();
void        ikbd_GPO(uint8_t bit, bool enable);
bool        ikbd_GPI(uint8_t bit);

bool        ikbd_txrdy();
bool        ikbd_rxrdy();
uint16_t    ikbd_sendbuf(uint8_t* data, uint16_t count);
bool        ikbd_send(uint8_t data);
uint8_t     ikbd_recv();

#endif //!__ASM__
#endif // _IKBD_H_

