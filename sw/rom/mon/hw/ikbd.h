#ifndef _IKBD_H_
#define _IKBD_H_
#ifndef __ASM__
#include "sys.h"

#define IKBD_GPO_PWRLED     0
#define IKBD_GPO_TP307      1

#define IKBD_BAUD_7812      0
#define IKBD_BAUD_15625     1
#define IKBD_BAUD_31250     2
#define IKBD_BAUD_62500     3
#define IKBD_BAUD_125000    4
#define IKBD_BAUD_250000    5
#define IKBD_BAUD_500000    6
#define IKBD_BAUD_1000000   7

bool        ikbd_Init();
uint32_t    ikbd_Connect(uint8_t baud);
void        ikbd_GPO(uint8_t bit, bool enable);
bool        ikbd_GPI(uint8_t bit);

bool        ikbd_txrdy();
bool        ikbd_rxrdy();
uint16_t    ikbd_sendbuf(uint8_t* data, uint16_t count);
bool        ikbd_send(uint8_t data);
uint8_t     ikbd_recv();

uint32_t    ikbd_Version();
uint32_t    ikbd_Info();
void        ikbd_Reset();
uint8_t     ikbd_Baud();

// --- ckbd only ---
void        ikbd_HardReset(bool bootloader);
void        ikbd_ClearSettings();
void        ikbd_SaveSettings();
void        ikbd_ReadSetting(uint8_t idx);
void        ikbd_WriteSetting(uint8_t idx, uint8_t val);

#endif //!__ASM__
#endif // _IKBD_H_

