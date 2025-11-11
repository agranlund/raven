/*
 * raven.c - Raven specific functions
 *
 * Copyright (C) 2013-2024 The EmuTOS development team
 *
 * Authors:
 *  Anders Granlund
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 * 
 * 
 * todo:
 *  - call raven bios functions for hardware access where applicable.
 *
 */



/* #define ENABLE_KDEBUG */

#ifdef MACHINE_RAVEN

#include "emutos.h"
#include "raven.h"
#include "vectors.h"
#include "asm.h"
#include "tosvars.h"
#include "bios.h"
#include "blkdev.h"
#include "processor.h"
#include "biosext.h"            /* for cache control routines */
#include "gemerror.h"
#include "ikbd.h"               /* for call_mousevec() */
#include "serport.h"
#include "screen.h"
#include "nova.h"
#include "mfp.h"
#include "delay.h"
#include "asm.h"
#include "string.h"
#include "disk.h"
#include "biosmem.h"
#include "bootparams.h"
#include "machine.h"
#include "has.h"
#include "lineavars.h"
#include "../bdos/bdosstub.h"

#include "raven.h"
extern void raven_int_ikbd(void);
extern void raven_int_vbl(void);

#define REGB(x,y) *((volatile UBYTE*)((x)+(y)))
#define REGW(x,y) *((volatile UWORD*)((x)+(y)))
#define REGL(x,y) *((volatile ULONG*)((x)+(y)))


#define RAVEN_BIOS_BASEPTR      0x40000000UL
#define RAVEN_BIOS_RTCREAD      0x40
#define RAVEN_BIOS_RTCWRITE     0x44
#define RAVEN_BIOS_VGADDR       0x8C


/*-----------------------------------------------------------------------------------------
 * Screen
 *---------------------------------------------------------------------------------------*/

#define BOOT_SCREEN_PLANES  1
#define BOOT_SCREEN_WIDTH   640
#define BOOT_SCREEN_HEIGHT  480
#define BOOT_SCREEN_BPL     (BOOT_SCREEN_WIDTH / 8)

typedef ULONG(*raven_screenaddr_func)(void);
static ULONG raven_screen_addr;

const UBYTE *raven_physbase(void) {
    return (const UBYTE*)raven_screen_addr;
}

void raven_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez) {
    *planes = BOOT_SCREEN_PLANES;
    *hz_rez = BOOT_SCREEN_WIDTH;
    *vt_rez = BOOT_SCREEN_HEIGHT;
}

void raven_screen_init(void) {
    ULONG rv = *((ULONG*)RAVEN_BIOS_BASEPTR);
    raven_screenaddr_func f = *((raven_screenaddr_func*)(rv + RAVEN_BIOS_VGADDR));
    raven_screen_addr = f();
    v_bas_ad = (UBYTE*)raven_screen_addr;
    sshiftmod = ST_HIGH;
    v_planes = BOOT_SCREEN_PLANES;
    V_REZ_HZ = BOOT_SCREEN_WIDTH;
    V_REZ_VT = BOOT_SCREEN_HEIGHT;
    BYTES_LIN = v_lin_wr = (BOOT_SCREEN_WIDTH / 8);
    VEC_VBL = raven_int_vbl;
}


/*-----------------------------------------------------------------------------------------
 * Keyboard
 *---------------------------------------------------------------------------------------*/

LONG raven_ikbd_bcostat(void)       { return (REGB(RAVEN_UART1_BASE, 0x14) & (1 << 5)) ? -1 : 0; }
void raven_ikbd_writeb(UBYTE b)     { REGB(RAVEN_UART1_BASE, 0x00) = b; }
LONG raven_ikbd_bconstat(void)      { return (REGB(RAVEN_UART1_BASE, 0x14) & (1 << 0)) ? -1 : 0; }
UBYTE raven_ikbd_readb(void)        { return REGB(RAVEN_UART1_BASE, 0x00); }

void raven_init_keyboard_interrupt(void) {
    REGL(0, 0x74) = (ULONG) raven_int_ikbd;
    REGB(RAVEN_UART1_BASE, 0x08) |= 0x01;   /* RX fifo enabled */
    REGB(RAVEN_UART1_BASE, 0x04)  = 0x01;   /* RHR interrupts enabled */
}

void raven_kbd_init(void) {
}


/*-----------------------------------------------------------------------------------------
 * MIDI
 *---------------------------------------------------------------------------------------*/

LONG raven_midi_bcostat(void)   { return (((MFP*)(RAVEN_PADDR_MFP2))->tsr & 0x80) ? -1 : 0; }
void raven_midi_writeb(UBYTE b) { ((MFP*)(RAVEN_PADDR_MFP2))->udr = (char)b; }
LONG raven_midi_bconstat(void)  { return (((MFP*)(RAVEN_PADDR_MFP2))->rsr & 0x80) ? -1 : 0; }
UBYTE raven_midi_readb(void)    { return ((MFP*)(RAVEN_PADDR_MFP2))->udr; }


/*-----------------------------------------------------------------------------------------
 * High speed uart
 * Atari compatible uart is already handled by EmuTOS's MFP driver
 *---------------------------------------------------------------------------------------*/

#define RS232_BUFSIZE    256
#define RS232_BUFMASK    ((RS232_BUFSIZE) - 1)
#define RS232_POLLING    (RAVEN_DEBUG_PRINT || 1)

static EXT_IOREC rs232_iorec;
static UBYTE rs232_ibuf[RS232_BUFSIZE];
static UBYTE rs232_obuf[RS232_BUFSIZE];

static const unsigned long rs232_freq = 24000000UL;

static const EXT_IOREC rs232_iorec_init = {
    { NULL, RS232_BUFSIZE, 0, 0, RS232_BUFSIZE/4, 3*RS232_BUFSIZE/4 },
    { NULL, RS232_BUFSIZE, 0, 0, RS232_BUFSIZE/4, 3*RS232_BUFSIZE/4 },
    B19200, FLOW_CTRL_NONE, 0x88, 0xff, 0xea };

static const unsigned long rs232_bauds[16] =
{
    /*  0: 19200 */      19200, /* same as TOS */
    /*  1:  9600 */       9600, /* same as TOS */
    /*  2:  4800 */       4800, /* same as TOS */
    /*  3:  3600 */       3600, /* same as TOS */
    /*  4:  2400 */       2400, /* same as TOS */
    /*  5:  2000 */    1000000,
    /*  6:  1800 */     750000,
    /*  7:  1200 */     500000,
    /*  8:   600 */     921600,
    /*  9:   300 */     460800,
    /* 10:   200 */     230400,
    /* 11:   150 */     115200, /* same as RSVE */
    /* 12:   134 */      57600, /* same as RSVE */
    /* 13:   110 */      38400, /* same as RSVE */
    /* 14:    75 */      28800,
    /* 15:    50 */      31250  /* midi, beacuse why not? */
};

static WORD rs232_baud_to_idx(unsigned long baud) {
    WORD idx = 0;
    long best = 0x0fffffffL;
    for (WORD i=0; i<16; i++) {
        long diff = (long) (baud > rs232_bauds[i]) ? (baud - rs232_bauds[i]) : (rs232_bauds[i] - baud);
        if (diff < best) { idx = i; best = diff; }
    }
    return idx;
}

static void rs232_set_baud(unsigned long baud) {
    unsigned long regs = (((rs232_freq / baud) << 4) & 0x00ffff00UL);  /* mm.ll.00 */
    regs |= (((((rs232_freq << 1) / baud) + 1) >> 1) & 0x0000000fUL);  /* mm.ll.dd */
    unsigned char lcr = REGB(RAVEN_UART2_BASE, 0x0C);
    REGB(RAVEN_UART2_BASE, 0x0C) = 0xBF;            // switch to enhanced regs
    REGB(RAVEN_UART2_BASE, 0x08) |= 0x10;           // unlock dld
    REGB(RAVEN_UART2_BASE, 0x0C) = 0x80;            // switch to baud regs
    REGB(RAVEN_UART2_BASE, 0x00) = (unsigned char)(((regs >>  8) & 0xff));
    REGB(RAVEN_UART2_BASE, 0x04) = (unsigned char)(((regs >> 16) & 0xff));
    REGB(RAVEN_UART2_BASE, 0x08) = (unsigned char)(((regs >>  0) & 0x0f) );//| (REGB(RAVEN_UART2_BASE, 0x08) & 0xf0));
    REGB(RAVEN_UART2_BASE, 0x0C) = 0xBF;            // switch to enhanced regs
    REGB(RAVEN_UART2_BASE, 0x08) &= ~0x10;          // lock dld
    REGB(RAVEN_UART2_BASE, 0x0C) = lcr;             // switch to normal regs
}

static unsigned long rs232_get_baud(void) {
    unsigned long regs = 0;
    unsigned char lcr = REGB(RAVEN_UART2_BASE, 0x0C);
    REGB(RAVEN_UART2_BASE, 0x0C) = 0xBF;            // switch to enhanced regs
    REGB(RAVEN_UART2_BASE, 0x08) |= 0x10;           // unlock dld
    REGB(RAVEN_UART2_BASE, 0x0C) = 0x80;            // switch to baud regs
    regs |= ((((unsigned long)REGB(RAVEN_UART2_BASE, 0x00)) & 0xff) <<  4);
    regs |= ((((unsigned long)REGB(RAVEN_UART2_BASE, 0x04)) & 0xff) << 12);
    regs |= ((((unsigned long)REGB(RAVEN_UART2_BASE, 0x08)) & 0x0f) <<  0);
    REGB(RAVEN_UART2_BASE, 0x0C) = 0xBF;            // switch to enhanced regs
    REGB(RAVEN_UART2_BASE, 0x08) &= ~0x10;          // lock dld
    REGB(RAVEN_UART2_BASE, 0x0C) = lcr;             // switch to normal regs
    unsigned long baud = (rs232_freq / regs);
    return baud;
}

static LONG  rs232_rxrdy(void)   { return ((REGB(RAVEN_UART2_BASE, 0x14) & (1 << 0)) == 0) ? 0 : -1; }
static LONG  rs232_txrdy(void)   { return ((REGB(RAVEN_UART2_BASE, 0x14) & (1 << 5)) == 0) ? 0 : -1; }
static UBYTE rs232_rx(void)      { return REGB(RAVEN_UART2_BASE, 0x00); }
static void  rs232_tx(UBYTE b)   { REGB(RAVEN_UART2_BASE, 0x00) = b; }

#if RS232_POLLING
#define rs232_bconstat  rs232_rxrdy
#define rs232_bcostat   rs232_txrdy
#else
static LONG rs232_bconstat(void) { return (rs232_iorec.in.head == rs232_iorec.in.tail) ? 0 : -1; }
static LONG rs232_bcostat(void)  { return (rs232_iorec.out.head == ((rs232_iorec.out.tail + 1) & RS232_BUFMASK)) ? 0 : -1; }
#endif

static LONG rs232_bconin(void) {
#if RS232_POLLING
    while (!rs232_bconstat()) { }
    return (LONG)rs232_rx();
#else    
    WORD old_sr;
    LONG value;
    while (!rs232_bconstat()) { }
    old_sr = set_sr(0x2700);
    rs232_iorec.in.head = (rs232_iorec.in.head + 1) & RS232_BUFMASK;
    value = (LONG) rs232_iorec.in.buf[rs232_iorec.in.head];
    set_sr(old_sr);
    return value;
#endif    
}

static LONG rs232_bconout(WORD dev, WORD b) {
#if RS232_POLLING
    while(!rs232_bcostat()) { }
    rs232_tx((UBYTE)b);
#else
    WORD old_sr;
    while(!rs232_bcostat()) { }
    old_sr = set_sr(0x2700);
    if ((rs232_iorec.out.head == rs232_iorec.out.tail) && rs232_txrdy()) {
        rs232_tx((UBYTE)b);
    } else {
        rs232_iorec.out.buf[rs232_iorec.out.tail] = (UBYTE)b;
        rs232_iorec.out.tail = (rs232_iorec.out.tail + 1) & RS232_BUFMASK;
    }
    set_sr(old_sr);
#endif    
    return 1;
}

#if RAVEN_DEBUG_PRINT
void raven_rs232_write_byte(UBYTE b) { rs232_bconout(7, b); }
#endif /* RAVEN_DEBUG_PRINT */



static ULONG rs232_rsconf(WORD baud, WORD ctrl, WORD ucr, WORD rsr, WORD tsr, WORD scr) {
    ULONG old, lcr, mcr;

    if (baud == -2) {
        return (ULONG) rs232_baud_to_idx(rs232_get_baud());
    }

    /*
     * retrieve old ucr/rsr/tsr/scr
     * according to the TT030 TOS Release notes, for non-MFP hardware, we must
     * return 0 for rsr and scr, and the only valid bit in the tsr is bit 3.
     */
    lcr = (ULONG)REGB(RAVEN_UART2_BASE, 0x0C);
    mcr = (ULONG)REGB(RAVEN_UART2_BASE, 0x10);

    old = 0;
    old |= ((lcr & (1UL<<6)) << 5);       /* break */
    old |= ((lcr & (1UL<<3)) << 23);      /* partity enable */
    old |= ((lcr & (1UL<<4)) << 21);      /* partity format */
    old |= ((3-(lcr & (3UL<<0))) << 29);  /* word length */
    if ((lcr & 3) == 0) {
        old |= (lcr & (1UL<<2)) ? (2UL << 27) : (1UL << 27);  /* 1.5 or 1 stop bits */
    } else {
        old |= (lcr & (1UL<<2)) ? (3UL << 27) : (1UL << 27);  /* 2 or 1 stop bits */
    }

    if ((ctrl >= MIN_FLOW_CTRL) && (ctrl <= MAX_FLOW_CTRL))
    {
        UBYTE efr; 
        REGB(RAVEN_UART2_BASE, 0x0C) = 0xBF;            /* select enhanced registers */
        efr = REGB(RAVEN_UART2_BASE, 0x08) & 0x30;
        REGB(RAVEN_UART2_BASE, 0x08) = efr;             /* disable all flow control */
        REGB(RAVEN_UART2_BASE, 0x0C) = lcr;             /* select normal registers */
        mcr &= ~0x02;                                   /* clear rts */

        if (ctrl & (FLOW_CTRL_SOFT | FLOW_CTRL_HARD))
        {
#if 0
            REGB(RAVEN_UART2_BASE, 0x0C) = 0xBF;        /* LCR: select enhanced registers */
            REGB(RAVEN_UART2_BASE, 0x08) |= 0x10;       /* EFR: unlock  MCR[7:5] */
            REGB(RAVEN_UART2_BASE, 0x0C) = lcr;         /* LCR: select normal registers */
            REGB(RAVEN_UART2_BASE, 0x10) |= 0x40;       /* MCR: select TCR register */
            REGB(RAVEN_UART2_BASE, 0x18) = 0x8f;        /* TCR: resume: 0, halt 64 */
            REGB(RAVEN_UART2_BASE, 0x10) &= ~0x40;      /* MCR: deselect TCR register */
            REGB(RAVEN_UART2_BASE, 0x0C) = 0xBF;        /* LCR: select enhanced registers */
            REGB(RAVEN_UART2_BASE, 0x08) &= ~0x10;      /* EFR: lock  MCR[7:5] */
#endif
            REGB(RAVEN_UART2_BASE, 0x0C) = 0xBF;        /* LCR: select enhanced registers */
            if (ctrl & FLOW_CTRL_HARD) {
                efr |= (1 << 7);                        /* auto cts */
                efr |= (1 << 6);                        /* auto rts */
                mcr |= 0x02;                            /* rts signal */
            }
            if (ctrl & FLOW_CTRL_SOFT) {
                REGB(RAVEN_UART2_BASE, 0x10) = 0x11;    /* XON1:  CTRL-Q */
                REGB(RAVEN_UART2_BASE, 0x18) = 0x13;    /* XOFF1: CTRL-S */
                efr |= 0xA;                             /* tx: xon1/xoff1, rx: xon1/xoff1 */
            }
            REGB(RAVEN_UART2_BASE, 0x08) = efr;         /* EFR: apply flow control settings */
            REGB(RAVEN_UART2_BASE, 0x0C) = lcr;         /* LCR: select normal registers */
        }
        REGB(RAVEN_UART2_BASE, 0x10) = mcr;         /* apply modem control changes */
        rs232_iorec.flowctrl = ctrl;
    }

    /*
     * set baudrate from lookup table
     */
    if ((baud >= MIN_BAUDRATE_CODE ) && (baud <= MAX_BAUDRATE_CODE)) {
        unsigned long bval = rs232_bauds[baud];
        rs232_set_baud(bval);
        rs232_iorec.baudrate = baud;
    }

    /*
     * handle ucr
     */
    if (ucr >= 0) {
        lcr = lcr & 0xC0;
        lcr |= (3 - ((ucr>>5)&3));                  /* word length      */
        lcr |= ((((ucr>>3)&3) > 1) ? 0x04 : 0x00);  /* stop bits        */
        lcr |= ((ucr&(1<<2)) << 1);                 /* parity enable    */
        lcr |= ((ucr&(1<<1)) << 3);                 /* parity odd/even  */
        REGB(RAVEN_UART2_BASE, 0x0C) = lcr;
        rs232_iorec.ucr = ucr;
    }

    /*
     * handle tsr
     */
    if (tsr >= 0) {
        lcr = (lcr & 0xBF) | ((tsr&(1<<3))<<3);     /* break */
        REGB(RAVEN_UART2_BASE, 0x0C) = lcr;
    }

    return old;
}

void raven_rs232_init(void) {
    MAPTAB* maptab = &bconmap_root.maptab[bconmap_root.maptabsize];

    memcpy(&rs232_iorec, &rs232_iorec_init, sizeof(EXT_IOREC));
    rs232_iorec.in.buf = rs232_ibuf;
    rs232_iorec.out.buf = rs232_obuf;
    rs232_iorec.baudrate = rs232_baud_to_idx(rs232_get_baud());

    maptab->Bconstat = rs232_bconstat;
    maptab->Bconin   = rs232_bconin;
    maptab->Bcostat  = rs232_bcostat;
    maptab->Bconout  = rs232_bconout;
    maptab->Rsconf   = rs232_rsconf;
    maptab->Iorec    = &rs232_iorec;

    bconmap_root.maptabsize++;
}


/*-----------------------------------------------------------------------------------------
 * NVRAM
 *---------------------------------------------------------------------------------------*/

#if CONF_WITH_NVRAM

#define RAVEN_RTC_EMUL_START    14
#define RAVEN_RTC_REAL_START    8

typedef void(*raven_nvram_func)(ULONG,UBYTE*,ULONG);

static inline UBYTE int2bcd(UWORD a) { return (a % 10) + ((a / 10) << 4); }
static inline UWORD bcd2int(UBYTE a) { return (a & 15) + ((a >> 4) * 10); }

static inline UBYTE nvram_read_raw(int index)
{
    UBYTE value = 0;
    ULONG rv = *((ULONG*)RAVEN_BIOS_BASEPTR);
    WORD old_sr = set_sr(0x2700);
    raven_nvram_func f = *((raven_nvram_func*)(rv + RAVEN_BIOS_RTCREAD));
    f(index, &value, 1);
    set_sr(old_sr);
    return value;
}

static inline void nvram_write_raw(int index, UBYTE value)
{
    ULONG rv = *((ULONG*)RAVEN_BIOS_BASEPTR);
    WORD old_sr = set_sr(0x2700);
    raven_nvram_func f = *((raven_nvram_func*)(rv + RAVEN_BIOS_RTCWRITE));
    f(index, &value, 1);
    set_sr(old_sr);
}

UBYTE raven_nvram_readb(int index)
{
    switch (index)
    {
        case  0: return bcd2int(nvram_read_raw(0) & 0x7f);      // seconds
        case  1: return 0x00;
        case  2: return bcd2int(nvram_read_raw(1) & 0x7f);      // minutes
        case  3: return 0x00;
        case  4: return bcd2int(nvram_read_raw(2) & 0x3f);      // hours
        case  5: return 0x00;
        case  6: return 0x00;
        case  7: return bcd2int(nvram_read_raw(4) & 0x3f);      // date
        case  8: return bcd2int(nvram_read_raw(5) & 0x1f);      // month
        case  9: return bcd2int(nvram_read_raw(6) & 0xff);      // year
        case 10: return 0x00;
        case 11: return 0x00;
        case 12: return 0x00;
        case 13: return 0x80;
        default: return nvram_read_raw(index - RAVEN_RTC_EMUL_START + RAVEN_RTC_REAL_START);
    }
}

void raven_nvram_writeb(int index, UBYTE value)
{
    switch (index)
    {
        case  0: nvram_write_raw(0, (nvram_read_raw(0) & 0x80) | (int2bcd(value) & 0x7f)); break;   // seconds
        case  1: break;
        case  2: nvram_write_raw(1, int2bcd(value) & 0x7f); break;                                  // minutes
        case  3: break;
        case  4: nvram_write_raw(2, int2bcd(value) & 0x3f); break;                                  // hours
        case  5: break;
        case  6: break;
        case  7: nvram_write_raw(4, int2bcd(value) & 0x3f); break;                                  // date
        case  8: nvram_write_raw(5, int2bcd(value) & 0x1f); break;                                  // month
        case  9: nvram_write_raw(6, int2bcd(value) & 0xff); break;                                  // year
        case 10: break;
        case 11: nvram_write_raw(0, (nvram_read_raw(0) & 0x7f) | (value & 0x80)); break;
        case 12: break;
        case 13: break;
        default: return nvram_write_raw(index - RAVEN_RTC_EMUL_START + RAVEN_RTC_REAL_START, value);
    }
}

void raven_nvram_detect(void)
{
    has_nvram = 1;
}

#endif /* CONF_WITH_NVRAM */

/*-----------------------------------------------------------------------------------------
 * boot flags
 *---------------------------------------------------------------------------------------*/
UBYTE raven_bootflags(void)
{
    return 0;
}

/*-----------------------------------------------------------------------------------------
 * ROM Disk
 *---------------------------------------------------------------------------------------*/

#if CONF_WITH_ROMDISK

static unsigned long ROMDISK_ADDR;
static unsigned long ROMDISK_SIZE;
#define ROMDISK_SECTOR(_x)  ((const UBYTE *)(ROMDISK_ADDR + ((_x) * SECTOR_SIZE)))

void romdisk_init(WORD dev, LONG *devices_available)
{
    UNIT * const u = &units[dev];

    /* look for an 'RDSK' entry in rom image */
    ROMDISK_ADDR = ROMDISK_SIZE = 0;
    unsigned long* toc = (unsigned long*) (RAVEN_PADDR_ROM + 0x400UL);
    if (toc[0] == 0x5F524F4DUL) {           /* '_ROM' */
        while(toc) {
            if (toc[0] == 0x5244534BUL) {   /* 'RDSK' */
                ROMDISK_ADDR = toc[1];
                ROMDISK_SIZE = toc[2];
                break;
            }
            toc += 4;
        }
    }

    if ((ROMDISK_ADDR == 0) || (ROMDISK_SIZE == 0)) {
        return;
    }

    /* look for FAT bootsector signature */
    if ((ROMDISK_SECTOR(0)[0x1fe] != 0x55) || (ROMDISK_SECTOR(0)[0x1ff] != 0xaa)) {
        KDEBUG(("romdisk: unexpected bootsector signature %02x,%02x\n",
                ROMDISK_SECTOR(0)[0x1fe], ROMDISK_SECTOR(0)[0x1ff]));
        return;
    }

    /* try adding this as though it were a partition - type must be something recognized */
    else if (add_partition(dev, devices_available, "GEM", 0, ROMDISK_SIZE)) {
        KDEBUG(("romdisk: add_partition failed\n"));
        return;
    }

    KDEBUG(("romdisk: attached unit %d\n", dev));

    u->valid = 1;
    u->size = ROMDISK_SIZE;
    u->psshift = get_shift(SECTOR_SIZE);
    u->last_access = 0;
    u->status = 0;
    u->features = 0;
}

LONG romdisk_ioctl(WORD dev, UWORD ctrl, void *arg)
{
    if (ROMDISK_SIZE) {
        switch (ctrl) {
        case GET_DISKINFO:
            {
                ULONG *info = (ULONG *)arg;
                info[0] = ROMDISK_SIZE;
                info[1] = SECTOR_SIZE;
                return E_OK;
            }

        case GET_DISKNAME:
            strcpy(arg, "romdisk");
            return E_OK;

        case GET_MEDIACHANGE:
            return MEDIANOCHANGE;
        }
    }
    return ERR;
}

LONG romdisk_rw(WORD rw, LONG sector, WORD count, UBYTE *buf, WORD dev)
{
    if ((sector + count) > ROMDISK_SIZE) {
        return ERR;
    }
    if ((rw & RW_RW) != RW_READ) {
        return EWRPRO;
    }

    memcpy(buf, ROMDISK_SECTOR(sector), SECTOR_SIZE * count);
    return E_OK;
}
#endif /* CONF_WITH_ROMDISK */


#endif /* MACHINE_RAVEN */
