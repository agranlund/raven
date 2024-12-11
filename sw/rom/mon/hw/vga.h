#ifndef _VGA_H_
#define _VGA_H_

#include "sys.h"

uint32_t vga_Init();
void vga_Clear();
void vga_SetMode(uint16_t mode);
void vga_Test();

void vga_Atari();
void vga_Mode13h();
void vga_SetPal(uint32_t idx, uint32_t num, uint8_t* pal);
void vga_GetPal(uint32_t idx, uint32_t num, uint8_t* pal);
uint32_t vga_Addr();


#endif // _VGA_H_

