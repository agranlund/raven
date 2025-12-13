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
#define RAVEN_PADDR_ROM         0x40000000UL
#define RAVEN_PADDR_MFP2        0xa0000a00UL
#define RAVEN_UART1_BASE        0x20000000UL
#define RAVEN_UART2_BASE        0x20000020UL

#ifndef __RAVEN__ASM__

BOOL raven_can_shutdown(void);
void raven_shutdown(void);

UBYTE raven_bootflags(void);
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
void raven_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez);

#if CONF_WITH_NVRAM
UBYTE raven_nvram_readb(int index);
void raven_nvram_writeb(int index, UBYTE value);
void raven_nvram_detect(void);
#endif

void raven_rs232_init(void);
void raven_rs232_write_byte(UBYTE b);

#endif /* __RAVEN__ASM__ */

#endif /* MACHINE_RAVEN */

#endif /* RAVEN_H */
