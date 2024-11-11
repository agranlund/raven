/*
 * raven.h - Raven specific functions
 *
 * Copyright (C) 2013-2024 The EmuTOS development team
 *
 * Authors:
 *  Anders Granlund
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef RAVEN_H
#define RAVEN_H

#ifdef MACHINE_RAVEN

#define RAVEN_BOARD_REV			0xA1
#define RAVEN_PADDR_MFP2        0xa0000a00
#define RAVEN_UART1_BASE        0x20000000
#define RAVEN_UART2_BASE        0x20000020

#ifndef __RAVEN__ASM__

#if (RAVEN_BOARD_REV > 0xA0)
#define RAVEN_IDE_W  4
#else
#define RAVEN_IDE_W  2
#endif

struct IDE
{
    UWORD data;             /* 0   ATA & ATAPI: data transfer */
#if (RAVEN_IDE_W == 4)
    UBYTE filler00[2];
#endif    
    UBYTE features;         /* 2   ATA & ATAPI: Read: error / Write: features */
    UBYTE filler02[RAVEN_IDE_W-1];
    UBYTE sector_count;     /* 4   ATAPI: Read: ATAPI Interrupt Reason Register / Write: unused */
    UBYTE filler04[RAVEN_IDE_W-1];
    UBYTE sector_number;    /* 6  ATAPI: reserved */
    UBYTE filler06[RAVEN_IDE_W-1];
    UBYTE cylinder_low;     /* 8  ATAPI: Byte Count Register (bits 0-7) */
    UBYTE filler08[RAVEN_IDE_W-1];
    UBYTE cylinder_high;    /* 10 ATAPI: Byte Count Register (bits 8-15) */
    UBYTE filler10[RAVEN_IDE_W-1];
    UBYTE head;             /* 12 ATAPI: Drive select */
    UBYTE filler12[RAVEN_IDE_W-1];
    UBYTE command;          /* 14 ATA & ATAPI: Read: status / Write: ATA command */
    UBYTE filler14[RAVEN_IDE_W-1];
#if (RAVEN_IDE_W == 2)
    UBYTE filler16[RAVEN_IDE_W*8];
#endif    
    UBYTE filler32[RAVEN_IDE_W*6];
    UBYTE control;          /* 32 + 12: ATA & ATAPI: Read: alternate status / Write: device control */
    UBYTE filler44[RAVEN_IDE_W-1];
    UBYTE address;          /* 32 + 14: unused */
    UBYTE filler46[RAVEN_IDE_W-1];
#if (RAVEN_IDE_W == 2)
    UBYTE filler48[RAVEN_IDE_W*8];
#endif    
};

#define ide_interface           ((volatile struct IDE *)0xfff00000)

void raven_screen_init(void);
void raven_kbd_init(void);
void raven_init_keyboard_interrupt(void);

LONG  raven_ikbd_bcostat(void);
LONG  raven_ikbd_bconstat(void);
void  raven_ikbd_writeb(UBYTE b);
UBYTE raven_ikbd_readb(void);

LONG  raven_midi_bcostat(void);
LONG  raven_midi_bconstat(void);
void  raven_midi_writeb(UBYTE b);
UBYTE raven_midi_readb(void);

const UBYTE* raven_physbase(void);

#if CONF_WITH_NVRAM
UBYTE raven_nvram_readb(int index);
void raven_nvram_writeb(int index, UBYTE value);
void raven_nvram_detect(void);
#endif

#if RAVEN_DEBUG_PRINT
void raven_com1_write_byte(UBYTE b);
#endif

#endif /* __RAVEN__ASM__ */

#endif /* MACHINE_RAVEN */

#endif /* RAVEN_H */
