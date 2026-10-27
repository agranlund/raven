#ifndef _VGA_H_
#define _VGA_H_

#include "sys.h"

uint32_t vga_Init();
void vga_Test();
uint32_t vga_Addr();

void vga_SetMode(uint16_t mode);
void vga_320x200x8bpp();
void vga_640x480x1bpp();

void vga_SetColor(uint8_t idx, uint8_t r, uint8_t g, uint8_t b);
void vga_SetPal(uint32_t idx, uint32_t num, uint8_t* pal);
void vga_GetPal(uint32_t idx, uint32_t num, uint8_t* pal);

void vga_Clear();
void vga_Fill(uint8_t color);
void vga_WaitVbl();

uint8_t vga_ReadPort(uint16_t port);
void vga_WritePort(uint16_t port, uint8_t val);
void vga_WriteReg(uint16_t port, uint8_t idx, uint8_t val);

#endif // _VGA_H_
