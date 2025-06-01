#ifndef __USBREPORT_H__
#define __USBREPORT_H__

#include "ch559.h"
#include "usbhost.h"

void HandleRepeats(void);
void HandleReceived(uint8_t port);
void RepeatTimer(void);

bool ParseReport(__xdata INTERFACE *interface, uint32_t len, __xdata uint8_t *report);

#define SetRepeatState(st) \
    TR0 = 0;               \
    RepeatState = st;      \
    TR0 = 1;



extern __xdata uint16_t StatusMode;

extern __xdata uint8_t LEDDelayMs;

// Mouse buffer size must be at least 6 for intellimouse detection, more for debugging
#define MOUSE_BUFFER_SIZE 32

extern __xdata uint8_t MouseBuffer[];
extern uint16_t BatCounter;
extern __code unsigned char KEY_BATCOMPLETE[];

#endif //__USBREPORT_H__

