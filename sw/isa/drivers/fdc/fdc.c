/*-------------------------------------------------------------------------------
 * ISA floppy disk driver
 * (c)2026 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "sysutil.h"
#include "fdc.h"
#include "isa.h"

#define FDC_DIRECT_ISA_ACCESS   1


typedef struct {
    bool        valid;
    bool        changed;
    uint8_t     hpc;        /* heads per cylinds */
    uint8_t     spt;        /* sectors per track */
    uint8_t     tps;        /* tracks per side   */
} disk_t;

typedef struct {
    uint16_t    spinrate;
    uint8_t     steprate;
    uint8_t     loadtime;
    uint8_t     unloadtime;
    uint8_t     cylinder;
} drive_t;

typedef struct {
    uint16_t    port;
    uint8_t     flags;
    uint8_t     ver;
    uint8_t     dor;
    drive_t     drive;
    disk_t      disk;
} fdc_t;

static uint8_t secbuf[512];
static fdc_t fdc_instance;
static fdc_t* fdc;
static isa_t* bus;

#define FDC_FLAG_AUTOSEEK       (1<<4)

#define MS_TO_TICKS(x)          ((x)/5)
#define DELAY_RESET             MS_TO_TICKS(100)
#define DELAY_MOTOR_ON          MS_TO_TICKS(1000)
#define TIMEOUT_PIO             MS_TO_TICKS(500)
#define TIMEOUT_INT             MS_TO_TICKS(5000)
#define TIMEOUT_RW              MS_TO_TICKS(5000)
#define TIMEOUT_MOTOR           MS_TO_TICKS(3000)

#define TIMEOUT_RW_NOINTS       10000000UL      /* number of isa bus reads */


#define FDC_REG_DOR             0x2
#define FDC_REG_MSR             0x4
#define FDC_REG_FIFO            0x5
#define FDC_REG_CTRL            0x7

#define FDC_DOR_RESET           (1<<2)
#define FDC_DOR_DMA             (1<<3)
#define FDC_DOR_MOTOR0          (1<<4)
#define FDC_DOR_MOTOR1          (1<<5)
#define FDC_DOR_MOTOR2          (1<<6)
#define FDC_DOR_MOTOR3          (1<<7)

#define FDC_MSR_POS0            (1<<0)
#define FDC_MSR_POS1            (1<<1)
#define FDC_MSR_POS2            (1<<2)
#define FDC_MSR_POS3            (1<<3)
#define FDC_MSR_BUSY            (1<<4)
#define FDC_MSR_DMA             (1<<5)
#define FDC_MSR_DATAIO          (1<<6)
#define FDC_MSR_DATAREG         (1<<7)

#define FDC_CMD_SPECIFY         0x03
#define FDC_CMD_STAT            0x04
#define FDC_CMD_WRITE_SECTOR    0xc5
#define FDC_CMD_READ_SECTOR     0x46
#define FDC_CMD_CALIBRATE       0x07
#define FDC_CMD_CHECK_IRQ       0x08
#define FDC_CMD_READ_ID         0x4a
#define FDC_CMD_FORMAT_TRACK    0x4d
#define FDC_CMD_SEEK            0x0f
#define FDC_CMD_VERSION         0x10
#define FDC_CMD_CONFIGURE       0x13
#define FDC_CMD_LOCK            0x94

typedef struct {
    uint8_t  hpc;
    uint8_t  tps;
    uint8_t  spt;
    uint16_t spinrate;
} flopinfo_t;

#define FDC_RATE_500   0        /* HD */
#define FDC_RATE_300   1
#define FDC_RATE_250   2        /* DD */
#define FDC_RATE_1000  3        /* ED */

/* default floppy specs. real stuff is read from bpb after density is detected */
flopinfo_t flopinfos[] = {
    {2, 80, 18,  500 }, /* 1.44 MB, 80 tracks, double sided, high density   */
    {2, 80,  9,  250 }, /*  720 KB, 80 tracks, double sided, double density */
    {0,  0,  0, 0 },
};

int16_t controller_enable(void);
void controller_disable(void);
int16_t motor_on(void);
void motor_off(void);

/*-------------------------------------------------------------------------------
    utility functions
*------------------------------------------------------------------------------*/

static uint16_t get_le16(uint8_t* buf, uint32_t offs) {
    uint16_t le = *((uint16_t*)&buf[offs]);
    return ((le>>8)|(le<<8));
}

static uint32_t timer_get(void) {
    return *((volatile uint32_t*)0x4ba);
}

static uint32_t timer_elapsed(uint32_t start) {
    return timer_get() - start;
}

static void timer_delay(uint32_t ticks) {
    uint32_t start = timer_get();
    ticks = ticks ? ticks : 2;
    while (timer_elapsed(start) < ticks);
}

void lba_to_chs(uint32_t lba, uint8_t* cyl, uint8_t* head, uint8_t* sector) {
    uint16_t hpc = fdc->disk.hpc;
    uint16_t spt = fdc->disk.spt;
    *cyl     = (lba / (spt * hpc));
    *head    = (lba / spt) % hpc;
    *sector  = (lba % spt) + 1;
}

uint32_t chs_to_lba(uint8_t cyl, uint8_t head, uint8_t sector) {
    uint16_t hpc = fdc->disk.hpc;
    uint16_t spt = fdc->disk.spt;
    uint32_t lba = cyl;
    lba *= hpc;
    lba += head;
    lba *= spt;
    lba += (sector - 1);
    return lba;
}

/*-------------------------------------------------------------------------------
    register level functions
*------------------------------------------------------------------------------*/

static uint8_t reg_read(uint16_t reg) {
    return inp(fdc->port + reg);
}

static void reg_write(uint16_t reg, uint8_t data) {
    outp(fdc->port + reg, data);
}

static void dor_write(uint8_t data) {
    reg_write(FDC_REG_DOR, data);
    fdc->dor = data;
}

static uint8_t dor_read(void) {
    return fdc->dor;
}

static int16_t wait_msr(uint8_t mask, uint8_t val, uint16_t timeout) {
    uint32_t start = timer_get();
    while(timer_elapsed(start) < (uint32_t)timeout) {
        uint8_t msr = reg_read(FDC_REG_MSR);
        if ((msr & mask) == val) {
            return E_OK;
        }
    }
    return EDRVNR;
}

static int data_write(uint8_t* buf, uint8_t num) {
    int16_t ret = 0;
    uint8_t msr;
    while (1) {
        /* wait for fifo ready */
        if (wait_msr(0x80, 0x80, TIMEOUT_PIO) != E_OK) {
            return EDRVNR;
        }
        /* verify controller expects us to write */
        msr = reg_read(FDC_REG_MSR);
        if (msr & 0x40) {
            return EUNDEV;
        }
        /* write to fifo */
        reg_write(FDC_REG_FIFO, buf[ret++]);
        if (ret >= num) {
            return ret;
        }
    }
}

static int16_t data_read(uint8_t* buf, int16_t num) {
    int16_t cnt = 0;
    uint8_t msr;

    /* make sure we are in result phase */
    if (wait_msr(0xD0, 0xD0, TIMEOUT_PIO) != E_OK) {
        return EDRVNR;
    }

    /* read from fifo */    
    for (; num >= 0; num--) {
        /* wait for fifo ready */
        if (wait_msr(0x80, 0x80, TIMEOUT_PIO) != E_OK) {
            return EDRVNR;
        }
        /* get data or finish */
        msr = reg_read(FDC_REG_MSR);
        if (msr & 0x40) {
            buf[cnt++] = reg_read(FDC_REG_FIFO);
        } else {
            return cnt;
        }
    }
    return EUNDEV;
}


/*-------------------------------------------------------------------------------
    mid-level floppy commands
*------------------------------------------------------------------------------*/

static int16_t sense_int(uint8_t* st0, uint8_t* cyl) {
    uint32_t start;
    uint8_t buf[8];

    #if DEBUG_PRINT
    dprintf("sense_int\n");
    #endif

    start = timer_get();
    while (timer_elapsed(start) < TIMEOUT_INT) {
        int16_t num;
        buf[0] = FDC_CMD_CHECK_IRQ;
        if (data_write(buf, 1) != 1) {
            return EUNDEV;
        }
        num = data_read(buf, 2);
        if (num == 2) {
            /* interrupt triggered */
            if (st0) { *st0 = buf[0]; }
            if (cyl) { *cyl = buf[1]; }
            return E_OK;
        } else if (num == 1) {
            /* still waiting */
            /* todo: check the actual ST0 code */
            timer_delay(10);
            continue;
        } else {
            return EUNDEV;
        }
    }
    return EDRVNR;
}

/*-------------------------------------------------------------------------------
    mid-level floppy commands
*------------------------------------------------------------------------------*/


static int16_t specify(uint16_t spinrate, uint8_t steprate, uint8_t loadtime, uint8_t unloadtime) {
    uint8_t buf[3];
    uint8_t srt, hut, hlt;

    spinrate = spinrate ? spinrate : fdc->drive.spinrate;
    steprate = steprate ? steprate : fdc->drive.steprate;
    loadtime = loadtime ? loadtime : fdc->drive.loadtime;
    unloadtime = unloadtime ? unloadtime : fdc->drive.unloadtime;

    #if DEBUG_PRINT
    dprintf("specify %d %d %d %d\n", spinrate, steprate, loadtime, unloadtime);
    #endif

    switch (spinrate) {
        case 1000:
            reg_write(FDC_REG_CTRL, FDC_RATE_1000);
            srt = (steprate > 8) ? 0 : (16 - (steprate * 2));
            hut = unloadtime / 8;
            hlt = loadtime * 2;
            break;
        case 250:
            reg_write(FDC_REG_CTRL, FDC_RATE_250);
            srt = (steprate > 32) ? 0 : ((32 - steprate) / 2);
            hut = unloadtime / 32;
            hlt = loadtime / 4;
            break;
        case 500:
            reg_write(FDC_REG_CTRL, FDC_RATE_500);
            srt = (steprate > 16) ? 0 : (16 - steprate);
            hut = unloadtime / 16;
            hlt = loadtime / 2;
            break;
        default:
            return EBADRQ;
    }

    srt = (srt > 15) ? 15 : srt;
    hlt = (hlt > 15) ? 15 : hlt;
    hut = (hut > 127) ? 127 : hut;

    buf[0] = FDC_CMD_SPECIFY;
    buf[1] = ((srt & 0xf) << 4) | (hut & 0xf);
    buf[2] = (hlt << 1) | 1;   /* bit0 = dma off */

    if (data_write(buf, 3) == 3) {
        fdc->drive.spinrate = spinrate;
        fdc->drive.steprate = steprate;
        fdc->drive.loadtime = loadtime;
        fdc->drive.unloadtime = unloadtime;
        return E_OK;
    }

    #if DEBUG_PRINT
    dprintf(" specify err\n");
    #endif
    return EUNDEV;
}

static int16_t calibrate(void) {
    int i; uint8_t buf[2];
    buf[0] = FDC_CMD_CALIBRATE;
    buf[1] = 0; /* drive number */

    #if DEBUG_PRINT
    dprintf("calibrate\n");
    #endif

    /* may need multiple retries to reach cylinder 0 */
    for (i=0; i<10; i++) {
        uint8_t st0, cyl;
        if (data_write(buf, 2) != 2) {
            return EUNDEV;
        }
        if (wait_msr(0x80, 0x80, TIMEOUT_INT) != E_OK) {
            return EUNDEV;
        }
        if (sense_int(&st0, &cyl) != E_OK) {
            return EUNDEV;
        }
        fdc->drive.cylinder = cyl;
        if (cyl == 0) {
            return E_OK;
        }
    }
    return EUNDEV;
}

static int8_t read_id(uint8_t head, uint8_t* c, uint8_t* h, uint8_t* s, uint8_t* n) {
    uint8_t buf[8];
    buf[0] = FDC_CMD_READ_ID;
    buf[1] = head;

    #if DEBUG_PRINT
    dprintf("read_id\n");
    #endif

    if (data_write(buf, 2) != 2) { return EUNDEV; }
    if (data_read(buf, 7) != 7) { return EUNDEV; }

    #if DEBUG_PRINT
    dprintf(" st=%02x %02x %02x c=%d h=%d s=%d n=%d\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
    #endif

    if ((buf[0] & 0xC0) != 0) {
        return EMEDIA;
    }

    fdc->drive.cylinder = buf[3];

    if (c) { *c = buf[3]; }
    if (h) { *h = buf[4]; }
    if (s) { *s = buf[5]; }
    if (n) { *n = buf[6]; }
    return buf[5];
}

static int16_t seek(uint8_t cyl) {
    uint8_t buf[3];
    buf[0] = FDC_CMD_SEEK;
    buf[1] = 0;
    buf[2] = cyl;

    #if DEBUG_PRINT
    dprintf("seek %d\n", cyl);
    #endif

    if (data_write(buf, 3) != 3) {
        return E_SEEK;
    }
    if (wait_msr(0x80, 0x80, TIMEOUT_INT) != E_OK) {
        return E_SEEK;
    }
    if (sense_int(&buf[0], &buf[1]) != E_OK) {
        return E_SEEK;
    }
    fdc->drive.cylinder = buf[1];
    return E_OK;
}

static int16_t readwrite(bool read, uint8_t* ptr, uint8_t count, uint8_t cyl, uint8_t head, uint8_t sector) {
    uint8_t buf[9];
    uint8_t end = sector + count - 1;
    uint8_t gap = (fdc->drive.spinrate >= 500) ? 0x1B : 0x2A;

    buf[0] = read ? FDC_CMD_READ_SECTOR : FDC_CMD_WRITE_SECTOR;
    buf[1] = (head << 2) | 0;   /* drive number */
    buf[2] = cyl;               /* track */
    buf[3] = head;              /* side */
    buf[4] = sector;            /* first sector */
    buf[5] = 2;                 /* 512 bytes per sector */
    buf[6] = end;               /* last sector */
    buf[7] = gap;               /* gap length */
    buf[8] = 0xff;              /* unused with 512 bytes per sector */

    #if DEBUG_PRINT
    dprintf("readwrite %d %08lx %d %d %d %d %02x\n", read, (uint32_t)ptr, count, cyl, head, sector, buf[7]);
    #endif

    if (!(fdc->flags & FDC_FLAG_AUTOSEEK)) {
        if (seek(cyl) != E_OK) {
            #if DEBUG_PRINT
            dprintf("seek fail\n");
            #endif
            return E_SEEK;
        }
    }

    /* this stuff is timing critical and interrupts needs to be disabled during pio transfer */
    {
        volatile register uint8_t* p_msr  = (volatile uint8_t*) (bus->iobase + fdc->port + FDC_REG_MSR);
        volatile register uint8_t* p_fifo = (volatile uint8_t*) (bus->iobase + fdc->port + FDC_REG_FIFO);
        register uint8_t* p = ptr;

        register int16_t remain = (512 * count) - 1;
        register uint32_t to = TIMEOUT_RW_NOINTS;
        register uint32_t tm = to;
        register uint16_t sr;

        if (read)
        {
            /* command phase */
            if (data_write(buf, 9) != 9) {
                #if DEBUG_PRINT
                dprintf("sector read cmd error, msr = %02x\n", *p_msr);
                #endif
                return EUNDEV;
            }

            /* keep interupts enabled while waiting for very first byte */
            if (wait_msr(0xF0, 0xF0, TIMEOUT_RW) != E_OK) {
                #if DEBUG_PRINT
                dprintf("sector read wait error, msr = %02x\n", *p_msr);
                #endif
                return EUNDEV;
            }

            /* disable interrupts for remainder of the transfer */
            *p++ = *p_fifo;
            sr = DisableInterrupts();
            while (remain) {
                while (tm) {
                    if (((*p_msr) & 0xF0) == 0xF0) {
                        *p++ = *p_fifo;
                        goto ok1;
                    }
                    tm--;
                }
                RestoreInterrupts(sr);
                #if DEBUG_PRINT
                dprintf("sector read exec error, msr = %02x, remain = %d\n", *p_msr, remain);
                #endif
                return EUNDEV;
            ok1:
                tm = to;
                remain--;
            }
            RestoreInterrupts(sr);
        }
        else
        {
            /* command phase */
            if (data_write(buf, 9) != 9) {
                #if DEBUG_PRINT
                dprintf("sector write cmd error, msr = %02x\n", *p_msr);
                #endif
                return EUNDEV;
            }
            if (wait_msr(0xF0, 0xB0, TIMEOUT_RW) != E_OK) {
                #if DEBUG_PRINT
                dprintf("sector write wait error, msr = %02x\n", *p_msr);
                #endif
                return EUNDEV;
            }
            /* execute phase */
            *p_fifo = *p++;
            DisableInterrupts();
            while (remain) {
                while (tm) {
                    if (((*p_msr) & 0xF0) == 0xB0) {
                        *p_fifo = *p++;
                        goto ok2;
                    }
                    tm--;
                }
                RestoreInterrupts(sr);
                #if DEBUG_PRINT
                dprintf("sector write exec error, msr = %02x, remain = %d\n", *p_msr, remain);
                #endif
                return EUNDEV;
            ok2:
                tm = to;
                remain--;
            }
        }
        RestoreInterrupts(sr);
    }

    /* result phase */
    if (data_read(buf, 7) != 7) {
        #if DEBUG_PRINT
        dprintf(" err (result)\n");
        #endif
        return EUNDEV;
    }

    /* todo: more result verifications */
    #if DEBUG_PRINT
    dprintf(" res = %02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
    #endif

    if (buf[0] & (1 << 4)) {    /* drive fault */
        #if DEBUG_PRINT
        dprintf("err: %02x %02x %02x\n", buf[0], buf[1], buf[2]);
        #endif
        return EUNDEV;
    }

    if (buf[1] != 0x80) {           /* end in polling mode, no errors */
        #if DEBUG_PRINT
        dprintf("err: %02x %02x %02x\n", buf[0], buf[1], buf[2]);
        #endif
#if 0                
        if (buf[1] & (1 << 4)) {    /* err: dma overrun */
            #if DEBUG_PRINT
            dprintf(" dma overrun\n");
            #endif
            controller_disable();
            controller_enable();
            motor_on();
            continue;
        }
#endif                
        return EUNDEV;
    }

    return E_OK;
}

static int16_t format(fdc_fmt_t* fmt, uint8_t count, uint8_t cyl, uint8_t head) {
    uint8_t buf[9];
    uint8_t gap = (count >= 13) ? 0x1B : 0x2A;

    buf[0] = FDC_CMD_FORMAT_TRACK;
    buf[1] = (head << 2) | 0;   /* drive number */
    buf[2] = 2;                 /* 512 bytes per sector */
    buf[3] = count;             /* sectors per track */
    buf[4] = gap;               /* gap length */
    buf[5] = 0xE5;              /* filler byte */
    buf[6] = 0x00;              /* reserved */
    buf[7] = 0x00;              /* reserved */
    buf[8] = 0x00;              /* reserved */


    #if DEBUG_PRINT
    dprintf(" cmd = %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
    dprintf("format %d %d %d\n", count, cyl, head);
    #endif
    if (!(fdc->flags & FDC_FLAG_AUTOSEEK)) {
        if (seek(cyl) != E_OK) {
            #if DEBUG_PRINT
            dprintf("seek fail\n");
            #endif
            return E_SEEK;
        }
    }

    /* this stuff is timing critical and interrupts needs to be disabled during pio transfer */
    {
        volatile register uint8_t* p_msr  = (volatile uint8_t*) (bus->iobase + fdc->port + FDC_REG_MSR);
        volatile register uint8_t* p_fifo = (volatile uint8_t*) (bus->iobase + fdc->port + FDC_REG_FIFO);
        register uint8_t* p = (uint8_t*)fmt;

        register int16_t remain = count;
        register uint32_t to = TIMEOUT_RW_NOINTS;
        register uint32_t tm = to;
        register uint16_t sr = 0;

        /* command phase */
        if (data_write(buf, 9) != 9) {
            #if DEBUG_PRINT
            dprintf("track write cmd error, msr = %02x\n", *p_msr);
            #endif
            return EUNDEV;
        }

        /* execute phase */
        sr = DisableInterrupts();
        while (remain) {
            while (tm) {
                if (((*p_msr) & 0xF0) == 0xB0) {
                    *p_fifo = *p++;
                    *p_fifo = *p++;
                    *p_fifo = *p++;
                    *p_fifo = *p++;
                    goto ok1;
                }
                tm--;
            }
            RestoreInterrupts(sr);
            #if DEBUG_PRINT
            dprintf("sector read exec error, msr = %02x, remain = %d\n", *p_msr, remain);
            #endif
            return EUNDEV;
        ok1:
            tm = to;
            remain--;
        }
        RestoreInterrupts(sr);
    }


    if (data_read(buf, 7) != 7) {
        #if DEBUG_PRINT
        dprintf(" err (result)\n");
        #endif
        return EUNDEV;
    }

    #if DEBUG_PRINT
    dprintf(" res = %02x %02x %02x %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
    #endif

    if ((buf[0] & 0xC0) != 0x00) {
        #if DEBUG_PRINT
        dprintf("err: %02x %02x %02x\n", buf[0], buf[1], buf[2]);
        #endif
        return EUNDEV;
    }

#if 0    
    if (buf[0] & (1 << 4)) {    /* drive fault */
        #if DEBUG_PRINT
        dprintf("err: %02x %02x %02x\n", buf[0], buf[1], buf[2]);
        #endif
        return EUNDEV;
    }

    if (buf[1] != 0x80) {           /* end in polling mode, no errors */
        #if DEBUG_PRINT
        dprintf("err: %02x %02x %02x\n", buf[0], buf[1], buf[2]);
        #endif
        return EUNDEV;
    }

    if (cnt != len) {
        #if DEBUG_PRINT
        dprintf(" cnt error %d / %d\n", cnt, len);
        #endif
        return EUNDEV;
    }
#endif
    return E_OK;
}


static int16_t read_bpb(fdc_bpb_t* bpb) {
    int16_t err;
    memset(bpb, 0, sizeof(fdc_bpb_t));
    err = readwrite(true, secbuf, 1, 0, 0, 1);
    if (err != E_OK) {
        return err;
    }
    bpb->bps = get_le16(secbuf, 0x0b);
    bpb->spc = secbuf[0x0d];
    bpb->ressec = get_le16(secbuf, 0x0e);
    bpb->nfats = secbuf[0x10];
    bpb->ndirs = get_le16(secbuf, 0x11);
    bpb->nsects = get_le16(secbuf, 0x13);
    bpb->media = secbuf[0x15];
    bpb->spf = get_le16(secbuf, 0x16);
    bpb->spt = get_le16(secbuf, 0x18);
    bpb->nheads = get_le16(secbuf, 0x1a);
    bpb->nhid = get_le16(secbuf, 0x1c);
    return E_OK;
}


/*-------------------------------------------------------------------------------
    mid-level controller functions
*------------------------------------------------------------------------------*/

int16_t motor_on(void) {
    int16_t err = controller_enable();
    if (err != E_OK) {
        return err;
    }
    if ((dor_read() & FDC_DOR_MOTOR0) == 0) {
        #if DEBUG_PRINT
        dprintf("motor on\n");
        #endif
        dor_write(dor_read() | FDC_DOR_MOTOR0);
        timer_delay(DELAY_MOTOR_ON);
    }
    return E_OK;
}

void motor_off(void) {
    dor_write(dor_read() & ~FDC_DOR_MOTOR0);
}

void controller_disable(void) {
    dor_write(0);
    fdc->drive.cylinder = -1;
    fdc->disk.valid = false;
}


static int16_t controller_configure(void) {
    uint8_t buf[8];
    #if DEBUG_PRINT
    dprintf("configure\n");
    #endif

    /* need version 0x90 for these commands */
    buf[0] = FDC_CMD_VERSION;
    if (data_write(buf, 1) != 1) { return EUNDEV; }
    if (data_read(buf, 1) != 1) { return EUNDEV; }
    if (buf[0] != 0x90) { return E_OK; }

    /* configure it */
    buf[0] = FDC_CMD_CONFIGURE;
    buf[1] = 0;                     /* reserved */
    buf[2] =    (IMPLIED_SEEK<<6) | /* implied seek disabled */
                (0<<5) |            /* fifo enable */
                (0<<4) |            /* drive polling on */
                8;                  /* fifo threshold */
    buf[3] = 0; /* precompensation */
    if (data_write(buf, 4) != 4) { return EUNDEV; }

    /* lock the settings */
    buf[0] = FDC_CMD_LOCK;
    if (data_write(buf, 1) != 1) { return EUNDEV; }
    if (data_read(buf, 1) != 1) { return EUNDEV; }
    if (buf[0] == 0x10) {
#if IMPLIED_SEEK
        #if DEBUG_PRINT
        dprintf("autoseek\n");
        #endif
        fdc->flags |= FDC_FLAG_AUTOSEEK;
#endif        
    }
    return E_OK;
}

int16_t controller_enable(void) {
    if ((dor_read() & FDC_DOR_RESET) == 0) {
        /* reset */
        dor_write(0);
        timer_delay(DELAY_RESET);
        /* enable in pio mode */
        dor_write(FDC_DOR_RESET);
        /* wait for ready */
        if (wait_msr(0xff, 0x80, 1000/5) != E_OK) {
            return EUNDEV;
        }
        /* clear internal interrupt status */
        if (sense_int(0, 0) != E_OK) { dor_write(0); return EUNDEV; }
        if (sense_int(0, 0) != E_OK) { dor_write(0); return EUNDEV; }
        if (sense_int(0, 0) != E_OK) { dor_write(0); return EUNDEV; }
        if (sense_int(0, 0) != E_OK) { dor_write(0); return EUNDEV; }
    }
    return E_OK;
}

int16_t controller_reset(void) {
    controller_disable();
    return controller_enable();
}

static int16_t controller_init(uint16_t port) {
    int16_t err;
    #if DEBUG_PRINT
    dprintf("fdc init\n");
    #endif
    memset((void*)fdc, 0, sizeof(fdc_t));
    fdc->port = port;
    fdc->drive.spinrate = 500;
    fdc->drive.steprate = 3;
    fdc->drive.loadtime = 16;
    fdc->drive.unloadtime = 240;
    fdc->drive.cylinder = -1;

    /* reset controller */
    err = controller_reset();
    if (err != E_OK) {
        return err;
    }

    /* configure */
    err = controller_configure();
    if (err != E_OK) {
        return controller_reset();
    }

    return E_OK;
}

static int16_t fdc_begin(void) {
    int16_t err;
    fdc_bpb_t bpb;
    bool mediachange = false;

    /* motor needs to be on when reading media change flag */
    err = motor_on();
    if (err != E_OK) {
        return err;
    }

    /* reinit media if drive door was opened */
    if ((dor_read() & FDC_DOR_RESET)) {
        if (reg_read(FDC_REG_CTRL) & 0x80) {
            mediachange = true;
        }
    }

    /* detect media geometry */
    if (!fdc->disk.valid || mediachange) {
        flopinfo_t* finfo = flopinfos;
        flopinfo_t* found = 0;

        /* density */
        for (; finfo->hpc; finfo++) {

            err = specify(finfo->spinrate, 0, 0, 0);
            if (err != E_OK) {
                #if DEBUG_PRINT
                dprintf("specify fail\n");
                #endif
                return err;
            }

            err = calibrate();
            if (err != E_OK) {
                #if DEBUG_PRINT
                dprintf("calibrate fail\n");
                #endif
                return err;
            }
            
            err = read_id(0, 0, 0, 0, 0);
            if (err >= 0) {
                found = finfo;
                break;
            } else if (err != EMEDIA) {
                #if DEBUG_PRINT
                dprintf("read_id err %d\n", err);
                #endif
                return err;
            }
        }
        if (!found) {
            #if DEBUG_PRINT
            dprintf("no media detected\n");
            #endif
            return EMEDIA;
        }

        /* asume default geometry until we know better */
        fdc->disk.hpc = found->hpc;
        fdc->disk.spt = found->spt;
        fdc->disk.tps = found->tps;
        fdc->disk.valid = true;
        #if DEBUG_PRINT
        dprintf("found: %d %d %d\n", fdc->disk.hpc, fdc->disk.spt, fdc->disk.tps);
        #endif

        /* clear disk changed flag */
        seek(79);
        seek(0);

        /* read bootsector and parse bpb */
        err = read_bpb(&bpb);
        if (err != E_OK) {
            return err;
        }

        /* todo: verify valid bpb */
        fdc->disk.hpc = bpb.nheads;
        fdc->disk.spt = bpb.spt;
        fdc->disk.tps = (bpb.nsects / bpb.spt) / bpb.nheads;
        #if DEBUG_PRINT
        dprintf("T:%d S:%d H:%d\n", fdc->disk.tps, fdc->disk.spt, fdc->disk.hpc);
        #endif
    }

    return E_OK;
}


/*-------------------------------------------------------------------------------

    high level functions

-------------------------------------------------------------------------------*/

#define FDC_RETRIES 2

static volatile int16_t fdc_access_lock;
static volatile uint32_t fdc_access_time;

static void fdc_lock(void) {
    uint16_t sr = DisableInterrupts();
    fdc_access_time = timer_get();
    fdc_access_lock++;
    RestoreInterrupts(sr);
}

static void fdc_unlock(void) {
    uint16_t sr = DisableInterrupts();
    fdc_access_time = timer_get();
    fdc_access_lock--;
    RestoreInterrupts(sr);
}

/*-------------------------------------------------------------------------------
    get or change seekrate
-------------------------------------------------------------------------------*/
int16_t fdc_seekrate(int16_t rate) {
    int16_t err = E_OK;
    int16_t old = fdc->drive.steprate;
    if (rate > 0) {
        err = motor_on();
        if (err != E_OK) {
            goto done;
        }
        err = specify(0, rate, 0, 0);
    }
done:
    if (err != E_OK) {
        controller_reset();
    }
    return old;
}


/*-------------------------------------------------------------------------------
    media change status
-------------------------------------------------------------------------------*/
int16_t fdc_changed() {
    int16_t err = E_OK;

    fdc_lock();

    /* no device check */
    if (!fdc->disk.valid) {
        err = EUNDEV;
        goto done;
    }
    if ((dor_read() & FDC_DOR_RESET) == 0) {
        err = EUNDEV;
        goto done;
    }

    /* motor must be on for register to be valid */
    err = motor_on();
    if (err != E_OK) {
        controller_reset();
        goto done;
    }

    /* read media change register */
    if (reg_read(FDC_REG_CTRL) & 0x80) {
        err = E_CHNG;
        goto done;
    }

done:
    fdc_unlock();
    return err;
}


/*-------------------------------------------------------------------------------
    retrieve bpb
-------------------------------------------------------------------------------*/
int16_t fdc_getbpb(fdc_bpb_t* bpb) {
    int16_t err, retry = 0;

    fdc_lock();
    err = fdc_begin();
    if (err != E_OK) {
        goto done;
    }
    for (retry = FDC_RETRIES; retry >= 0; retry--) {
        err = fdc_begin();
        if (err == E_OK) {
            err = read_bpb(bpb);
            if (err == E_OK) {
                goto done;
            }
        }
    }
done:
    if (err != E_OK) {
        controller_reset();
    }
    fdc_unlock();
    return err;
}


/*-------------------------------------------------------------------------------
    read/write
-------------------------------------------------------------------------------*/

static int16_t fdc_readwrite_sector(bool read, uint8_t* buf, uint8_t cyl, uint8_t head, uint8_t sec) {
    int16_t err, retry;
    for (retry = FDC_RETRIES; retry >= 0; retry--) {
        err = fdc_begin();
        if (err == E_OK) {
            err = readwrite(read, buf, 1, cyl, head, sec);
            if (err == E_OK) {
                return err;
            }
        }
        controller_reset();
    }
    return err;
}

static int16_t fdc_readwrite_lba(bool read, uint8_t* buf, uint8_t count, uint32_t lba) {
    int16_t err;

    fdc_lock();
    err = fdc_begin();
    if (err != E_OK) {
        goto done;
    }

    for (; count; count--) {
        uint8_t c,h,s;
        lba_to_chs(lba, &c, &h, &s);
        err = fdc_readwrite_sector(read, buf, c, h, s);
        if (err != E_OK) {
            goto done;
        }
        buf += 512;
        lba += 1;
    }

done:
    if (err != E_OK) {
        controller_reset();
    }
    fdc_unlock();
    return err;
}

static int16_t fdc_readwrite_chs(bool read, uint8_t* buf, uint8_t count, uint8_t c, uint8_t h, uint8_t s) {
    int16_t err;

    fdc_lock();
    err = fdc_begin();
    if (err != E_OK) {
        goto done;
    }

    for (; count; count--) {
        err = ESECNF;
        if (h >= fdc->disk.hpc) { goto done; }
        if (c >= fdc->disk.tps) { goto done; }
        if (s >  fdc->disk.spt) { goto done; }
        err = fdc_readwrite_sector(read, buf, c, h, s);
        if (err != E_OK) {
            goto done;
        }
        buf += 512;
        s += 1;
    }

done:
    if ((err != E_OK) && (err != ESECNF)) {
        controller_reset();
    }
    fdc_unlock();
    return err;
}

int16_t fdc_read_lba(uint8_t* buf, uint8_t count, uint32_t lba) { return fdc_readwrite_lba(true, buf, count, lba); }
int16_t fdc_write_lba(uint8_t* buf, uint8_t count, uint32_t lba) { return fdc_readwrite_lba(false, buf, count, lba); }
int16_t fdc_read_chs(uint8_t* buf, uint8_t count, uint8_t c, uint8_t h, uint8_t s) { return fdc_readwrite_chs(true, buf, count, c, h, s); }
int16_t fdc_write_chs(uint8_t* buf, uint8_t count, uint8_t c, uint8_t h, uint8_t s) { return fdc_readwrite_chs(false, buf, count, c, h, s); }


/*-------------------------------------------------------------------------------
    format
-------------------------------------------------------------------------------*/
int16_t fdc_format(fdc_fmt_t* fmt, uint8_t count, uint8_t c, uint8_t h) {
    uint16_t spinrate;
    int16_t err = E_OK;

    fdc_lock();

    spinrate = (count >= 13) ? 500 : 250;
    if (spinrate != fdc->drive.spinrate) {
        controller_disable();
    }

    if ((dor_read() & FDC_DOR_MOTOR0) == 0) {
        err = controller_enable();
        if (err != E_OK) {
            goto done;

        }
        err = motor_on();
        if (err != E_OK) {
            goto done;
        }

        err = specify(spinrate, 0, 0, 0);
        if (err != E_OK) {
            goto done;
        }
        err = calibrate();
        if (err != E_OK) {
            return err;
        }
        seek(79);
        seek(0);
    }


    err = format(fmt, count, c, h);
    if (err != E_OK) {
        goto done;
    }

    /* update disk geometry */
    if (c == 0) {
        fdc->disk.valid = true;
        fdc->disk.changed = false;
        fdc->drive.spinrate = spinrate;
        fdc->disk.spt = count;
        fdc->disk.hpc = (h + 1);
        fdc->disk.tps = (c + 1);
    } else {
        fdc->disk.tps = (c + 1);
    }

done:
    if (err != E_OK) {
        controller_reset();
    }
    fdc_unlock();
    return err;
}


/*-------------------------------------------------------------------------------
    periodic update
-------------------------------------------------------------------------------*/
void fdc_update(void) {
    uint16_t sr = DisableInterrupts();
    if (fdc_access_lock <= 0) {
        if ((dor_read() & FDC_DOR_MOTOR0)) {
            if (timer_elapsed(fdc_access_time) > TIMEOUT_MOTOR) {
                #if DEBUG_PRINT
                dprintf("motor off\n");
                #endif
                motor_off();
            }
        }
    }
    RestoreInterrupts(sr);
}


/*-------------------------------------------------------------------------------
    init
-------------------------------------------------------------------------------*/
int16_t fdc_init(void) {
    bus = isa_init();
    if (!bus) {
        #if DEBUG_PRINT
        dprintf("err: no isa bus\n");
        #endif
        return EUNDEV;
    }

    fdc = &fdc_instance;
    if (controller_init(0x3f0) != E_OK) {
        #if DEBUG_PRINT
        dprintf("err: no fdc controller\n");
        #endif
        motor_off();
        return EUNDEV;
    }

    motor_off();
    return E_OK;
}

