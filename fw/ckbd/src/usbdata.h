#ifndef __USBDATA_H__
#define __USBDATA_H__
#include <stdbool.h>
#include <stdint.h>

extern __xdata const uint8_t StandardKeyboardDescriptor[];
extern __xdata const uint8_t StandardMouseDescriptor[];

#if 0
extern __code uint8_t ASCIItoHID[];
extern __code JoyPreset DefaultJoyMaps[];
extern __code JoyPreset ConfigGameNoMouse[];
extern __code JoyPreset ConfigGameMouse[];
#endif


#endif /* __USBDATA_H__ */
