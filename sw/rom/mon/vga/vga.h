#ifndef _VGA_H_
#define _VGA_H_

#include "../sys.h"

bool vga_Init();
void vga_Clear();
void vga_SetMode(uint16_t mode);
void vga_Test();
void vga_Atari();

#endif // _VGA_H_

