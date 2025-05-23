/*
 * machine.h - declarations about the machine type and capabilities
 *
 * Copyright (C) 2001-2024 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef MACHINE_H
#define MACHINE_H

#include "memory.h"

/*
 * hardware registers (and some bit definitions)
 */
#define FALCON_BUS_CTL      0xffff8007UL
#define FALCON_HHT          0xffff8282UL
#define TT_PALETTE_REGS     ((volatile UWORD *)0xffff8400UL)
#define BLITTER_CONFIG1     0xffff8a3cUL
#define SCC_BASE            0xffff8c80UL
#define SYS_INT_MASK        0xffff8e01UL
#define SYS_INT_VSYNC           (1<<4)
#define SYS_INT_HSYNC           (1<<2)
#define SCU_GPR1            0xffff8e09UL
#define VME_INT_MASK        0xffff8e0dUL
#define VME_INT_MFP             (1<<6)
#define VME_INT_SCC             (1<<5)
#define DIP_SWITCHES        0xffff9200UL
#define MONSTER_REG         0xfffffe00UL

/*
 * standard bus numbers
 */
#define ACSI_BUS            0
#define SCSI_BUS            1
#define IDE_BUS             2
#define SDMMC_BUS           3

#if CONF_WITH_SDMMC
# define MAX_BUS            SDMMC_BUS
#elif CONF_WITH_IDE
# define MAX_BUS            IDE_BUS
#elif CONF_WITH_SCSI
# define MAX_BUS            SCSI_BUS
#else
# define MAX_BUS            ACSI_BUS
#endif

#if CONF_WITH_ROMDISK
# define ROMDISK_BUS        MAX_BUS+1
#endif

extern ULONG detected_busses;

/*
 * some useful cookies.
 */

extern ULONG cookie_mch;
extern ULONG cookie_vdo;
extern ULONG cookie_snd;
#if CONF_WITH_FDC
extern ULONG cookie_fdc;
#endif
#if CONF_WITH_DIP_SWITCHES
extern ULONG cookie_swi;
#endif
extern ULONG cookie_akp;
extern ULONG cookie_idt;

/*
 * these are != 0 if the feature is present
 */

/* Convenience macro to test if first boot. See MEMINIT_BIT_FIRST_BOOT. */
#define FIRST_BOOT (meminit_flags & MEMINIT_FIRST_BOOT)

/*
 * functions
 */

/* address bus width */
#if CONF_WITH_ADVANCED_CPU
BOOL detect_32bit_address_bus(void);
#endif

/* XHDI vector table */
#if CONF_WITH_XHDI
long xhdi_vec(UWORD opcode, ...);   /* In bios/natfeat.S */
#endif

/* detect the available hardware */
void machine_detect(void);

/* perform machine-specific initialisation */
void machine_init(void);

/* fill the cookie jar */
void fill_cookie_jar(void);

/* print the name of the machine */
const char * machine_name(void);

#define IS_STRAM_POINTER(p) ((UBYTE *)(p) < phystop)

#endif /* MACHINE_H */
