/*
 * disk.h - disk routines
 *
 * Copyright (C) 2001-2024 The EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef DISK_H
#define DISK_H

/* defines */

#define NUMFLOPPIES     2   /* max number of floppies supported */

#define DEVICES_PER_BUS     8

#define UNITSNUM            (NUMFLOPPIES+(DEVICES_PER_BUS*(MAX_BUS+1))+CONF_WITH_ROMDISK)

#define GET_BUS(major)          ((major)/DEVICES_PER_BUS)
#define IS_ACSI_DEVICE(major)   (GET_BUS(major) == ACSI_BUS)
#define IS_SCSI_DEVICE(major)   (GET_BUS(major) == SCSI_BUS)
#define IS_IDE_DEVICE(major)    (GET_BUS(major) == IDE_BUS)
#define IS_SDMMC_DEVICE(major)  (GET_BUS(major) == SDMMC_BUS)
#define IS_ROMDISK_DEVICE(major) (GET_BUS(major) == (MAX_BUS+1))

#define GET_UNITNUM(bus,dev)    (NUMFLOPPIES+(DEVICES_PER_BUS*(bus))+dev)

/*
 * commands used for internal xxx_ioctl() calls
 */
#define GET_DISKINFO        20  /* get disk info for specified drive: */
                                /* arg -> array of two ULONGS:        */
                                /*   [0] capacity (in sectors)        */
                                /*   [1] sector size (in bytes)       */
#define GET_DISKNAME        21  /* get name of specified drive:       */
                                /* arg -> return data (max 40 chars)  */
#define GET_MEDIACHANGE     30  /* return status as per Mediach() call*/
                                /* arg is NULL                        */
#define CHECK_DEVICE        40  /* determine if device exists         */
                                /* (not necessarily a hard disk)      */

#if CONF_WITH_ULTRASATAN_CLOCK
#define ULTRASATAN_GET_FIRMWARE_VERSION 60
#define ULTRASATAN_GET_CLOCK 61
#define ULTRASATAN_SET_CLOCK 62
#endif /* CONF_WITH_ULTRASATAN_CLOCK */

/* read/write flags */
#define RW_READ             0
#define RW_WRITE            1
/* bit masks */
#define RW_RW               1
#define RW_NOMEDIACH        2
#define RW_NORETRIES        4
#define RW_NOTRANSLATE      8
/* EmuTOS extension: Rwabs without byteswap on IDE */
#define RW_NOBYTESWAP     128

/*
 *  return codes
 */

#define DEVREADY        -1L             /*  device ready                */
#define DEVNOTREADY     0L              /*  device not ready            */
#define MEDIANOCHANGE   0L              /*  media def has not changed   */
#define MEDIAMAYCHANGE  1L              /*  media may have changed      */
#define MEDIACHANGE     2L              /*  media def has changed       */

/* physical unit (floppy/harddisk) identifier */
struct _unit
{
    UBYTE   valid;          /* unit valid */
#if CONF_WITH_IDE
    UBYTE   byteswap;       /* unit is byteswapped */
#endif
    ULONG   size;           /* number of physical sectors */
    WORD    psshift;        /* shift left amount to convert sectors to bytes */
    LONG    last_access;    /* used in mediach only */
    LONG    drivemap;       /* bitmap of logical devices on this physical unit */
    UBYTE   features;       /* see below */
#define UNIT_REMOVABLE  0x80    /* unit uses removable media */
#define UNIT_NATFEATS   0x40    /* unit is managed by NatFeats */
    UBYTE   status;         /* see below */
#define UNIT_CHANGED    0x01    /* 0 => physical media has not changed */
};
typedef struct _unit UNIT;

extern UNIT units[];

/* physical disk functions */

#if CONF_WITH_XHDI
LONG disk_inquire(UWORD unit, ULONG *blocksize, ULONG *deviceflags, char *productname, UWORD stringlen);
#endif

LONG disk_get_capacity(UWORD unit, ULONG *blocks, ULONG *blocksize);
LONG disk_rw(UWORD unit, UWORD rw, ULONG sector, UWORD count, UBYTE *buf);

/* xbios functions */

LONG DMAread(LONG sector, WORD count, UBYTE *buf, WORD major);
LONG DMAwrite(LONG sector, WORD count, const UBYTE *buf, WORD major);

/* partition detection */

void disk_init_all(void);
LONG disk_mediach(UWORD unit);
void disk_rescan(UWORD unit);

#if CONF_WITH_ROMDISK
void romdisk_init(WORD dev, LONG *devices_available);
LONG romdisk_ioctl(WORD dev, UWORD ctrl, void *arg);
LONG romdisk_rw(WORD rw, LONG sector, WORD count, UBYTE *buf, WORD dev);
#endif

#endif /* DISK_H */
