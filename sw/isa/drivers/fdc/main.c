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
#include <tos.h>
#include "sysutil.h"
#include "fdc.h"

#if DEBUG_PRINT
#include <stdarg.h>
#include "raven.h"
void dprint(char* s, ...) {
    static char dbgstr[512];
    va_list args;
    char* buf = dbgstr;
    uint16_t sr = DisableInterrupts();
    va_start(args, s);
    vsprintf(buf, s, args);
    va_end(args); 
    while(*buf != 0) {
        while((*((uint8_t*)(RV_PADDR_UART2 + 0x14)) & (1 << 5)) == 0);
        *((char*)(RV_PADDR_UART2 + 0x00)) = *buf++;
    }
    RestoreInterrupts(sr);
}

void ddump(uint8_t* buf, uint32_t cnt) {
    int i,j;
    for (i=0; i<(cnt/16); i++) {
        dprintf(("%04x ", (i*16)));
        for (j=0; j<16; j++) {
            dprintf(("%02x ", buf[(i*16)+j]));
        }
        for (j=0; j<16; j++) {
            char c = buf[(i*16)+j];
            if ((c < 32) || (c > 126)) { c = '.'; }
            dprintf(("%c", c));
        }
        dprintf(("\n"));
    }
}
#endif


extern uint32_t xbios_old;
extern uint32_t xbios_new;
int16_t floppy_devno;

extern uint32_t hdv_old_rw;
extern uint32_t hdv_old_getbpb;
extern uint32_t hdv_old_mediach;

extern uint32_t hdv_new_rw;
extern uint32_t hdv_new_getbpb;
extern uint32_t hdv_new_mediach;


/******************************************************************************
***                                                                         ***
***                                 BIOS                                    ***
***                                                                         ***
******************************************************************************/


/*-------------------------------------------------------------------------------
    hdv_rw
-------------------------------------------------------------------------------*/
typedef struct {int16_t rwflag; void* buf; int16_t cnt; int16_t recnr; int16_t dev; int32_t lrecno; } hdv_rw_args;
int32_t hdv_rw(hdv_rw_args* args) {
    int16_t err;
    uint32_t lba = (args->recnr < 0) ? args->lrecno : args->recnr;
    dprintf(("hdv_rw %02x %d, %ld, %d\n", args->rwflag, args->dev, lba, args->cnt));

    if (args->rwflag & 1) {
#if READONLY
        err = EWRPRO;
#else
        err = fdc_write_lba(args->buf, args->cnt, lba);
#endif        
    } else {
        err = fdc_read_lba(args->buf, args->cnt, lba);
    }
    return (int32_t)err;
}

/*-------------------------------------------------------------------------------
    hdv_getbpb
-------------------------------------------------------------------------------*/
int32_t hdv_getbpb(int16_t dev) {
    static BPB tos_bpb;
    fdc_bpb_t fdc_bpb;
    uint32_t temp;
    (void)dev;

    if (fdc_getbpb(&fdc_bpb) != E_OK) {
        return 0;
    }

    /* bytes per sector */
    tos_bpb.recsiz = fdc_bpb.bps;

    /* sectors per cluster */
    tos_bpb.clsiz = fdc_bpb.spc;

    /* bytes per cluster */
    tos_bpb.clsizb = tos_bpb.recsiz * tos_bpb.clsiz;

    /* directory length */
    if (tos_bpb.recsiz != 0) {
        temp = 32UL * fdc_bpb.ndirs;
        temp += (tos_bpb.recsiz - 1);
        temp /= tos_bpb.recsiz;
        tos_bpb.rdlen = (uint16_t)temp;
    } else {
        tos_bpb.rdlen = 0;
    }

    /* length of fat in sectors */
    tos_bpb.fsiz = fdc_bpb.spf;

    /* start of 2nd fat */
    tos_bpb.fatrec = fdc_bpb.ressec;
    if (tos_bpb.fatrec == 0) {
        tos_bpb.fatrec = 1;
    }
    if (fdc_bpb.nfats >= 2) {
        tos_bpb.fatrec += tos_bpb.fsiz;
    }

    /* first free sector */
    tos_bpb.datrec = tos_bpb.fatrec + tos_bpb.fsiz + tos_bpb.rdlen;
   
    /* total number of cluster */
    if (fdc_bpb.nsects >= tos_bpb.datrec) {
        tos_bpb.numcl = (fdc_bpb.nsects - tos_bpb.datrec) / fdc_bpb.spc;
    } else {
        tos_bpb.numcl = 0;
    }

    /* media flags */
    tos_bpb.bflags = 0;     /* assume 2xFAT12 */
    if (fdc_bpb.nfats < 2) {
        tos_bpb.bflags |= (1 << 1); /* single fat */
    }
    if (tos_bpb.numcl > 4084) {
        tos_bpb.bflags |= (1 << 0); /* fat16 */
    }

    return (int32_t)&tos_bpb;
}

/*-------------------------------------------------------------------------------
    hdv_mediach
-------------------------------------------------------------------------------*/
int32_t hdv_mediach(int16_t dev) {
    (void)dev;
    return (fdc_changed() == E_OK) ? 0 : 2;
}




/******************************************************************************
***                                                                         ***
***                                 XBIOS                                   ***
***                                                                         ***
******************************************************************************/


/*-------------------------------------------------------------------------------
    floprd
-------------------------------------------------------------------------------*/
typedef struct {void *buf; int32_t filler; int16_t devno; int16_t sectno; int16_t trackno; int16_t sideno; int16_t count; } xb_floprw_args;
int16_t xb_floprd(xb_floprw_args* args) {
    return fdc_read_chs(args->buf, args->count, args->trackno, args->sideno, args->sectno);
}

/*-------------------------------------------------------------------------------
    flopwr
-------------------------------------------------------------------------------*/
int16_t xb_flopwr(xb_floprw_args* args) {
#if READONLY
    (void)args;
   return EWRPRO;
#else
   return fdc_write_chs(args->buf, args->count, args->trackno, args->sideno, args->sectno);
#endif
}

/*-------------------------------------------------------------------------------
    flopver
-------------------------------------------------------------------------------*/
int16_t xb_flopver(xb_floprw_args* args) {
    /*
    The function returns the value 0 if the list stored in the parameter buf is valid, or a non-zero value otherwise.
    Note: After the call one finds in the parameter buf a NULL-terminated list of 16-bit words containing
    the numbers of the defective sectors. So the function does not compare sectors with a block of memory;
    instead it always reads the sectors into the same buffer. This only verifies that the sectors can be read correctly,
    or if read errors occur during reading.
    */
    int16_t sec = args->sectno;
    uint16_t* seclist = (uint16_t*)args->buf;
    uint8_t* rdbuf = ((uint8_t*)args->buf) + 512UL;
    for (; sec < (args->sectno + args->count); sec++) {
        int16_t err = fdc_read_chs(rdbuf, 1, args->trackno, args->sideno, sec);
        if (err != E_OK) {
            *seclist++ = (sec ? sec : -1);
        }
    }
    *seclist = 0;
    return E_OK;
}

/*-------------------------------------------------------------------------------
    flopfmt
-------------------------------------------------------------------------------*/
typedef struct {void *buf; int32_t filler; int16_t devno; int16_t spt; int16_t trackno; int16_t sideno; int16_t interlv; int32_t magic; int16_t virgin; } xb_flopfmt_args;
int16_t xb_flopfmt(xb_flopfmt_args* args) {
    /*
    The function returns 0 when no error has occurred in formatting the track.
    Otherwise a NULL-terminated list of the faulty sectors will be written as a int16_t array into the buffer buf.
    */
#if (READONLY || DISABLE_FORMAT)
    (void)args;
    return EWRPRO;
#else   
    int16_t err, i;
    int16_t interlv;
    int16_t interlv_offs;
    int16_t interlv_used;
    fdc_fmt_t* data = (fdc_fmt_t*)args->buf;
    uint16_t* skew = 0;

    if (args->magic != 0x87654321UL) {
        return EBADSF;
    }

    interlv = args->interlv;
    if (args->interlv >= 0) {
        interlv %= args->spt;
        interlv = (interlv == 0) ? 1 : interlv;
        interlv_offs = -interlv;
        interlv_used = 0;
    } else {
        skew = (uint16_t*)args->filler;
    }

    for (i=0; i<args->spt; i++) {
        data[i].cyl = args->trackno;
        data[i].head = args->sideno;
        data[i].size = 2; /* 512 bytes per sector */
        if (skew) {
            data[i].record = *skew++;
        } else {
            interlv_offs = ((interlv_offs + interlv) % args->spt);
            while (interlv_used & (1L << interlv_offs)) {
                interlv_offs = ((interlv_offs + 1) % args->spt);
            }
            data[i].record = (interlv_offs + 1);
            interlv_used |= (1L << interlv_offs);
        }
    }

    for (i=0; i<args->spt; i++) {
        dprintf(("%d: %d %d %d %d\n", i, data[i].cyl, data[i].head, data[i].record, data[i].size));
    }

    /* format track */
    dprintf((" format %d %d %d\n", args->spt, args->trackno, args->sideno));
    err = fdc_format(data, args->spt, args->trackno, args->sideno);
    dprintf((" format err = %d\n", err));
    if (err) {
        return err;
    }

    /* verify track */
/*
    dprintf(("verify\n"));
    err = Flopver(args->buf, 0L, args->devno, 1, args->trackno, args->sideno, args->spt);
    dprintf(("verify err = %d\n", err));
    if (err) {
        return err;
    } else if (*((int16_t*)args->buf) != 0) {
        return EBADSF;
    }
*/        
    return E_OK;
#endif    
}

/*-------------------------------------------------------------------------------
    floprate
-------------------------------------------------------------------------------*/
typedef struct {int16_t devno; int16_t newrate; } xb_floprate_args;
int16_t xb_floprate(xb_floprate_args* args) {
    int16_t ret = 0;
    switch (args->newrate) {
        case -4:
            /* return drive type */
            ret = -1;   /* 0 == DD, -1 = HD */
            break;
        case -3:
            /* set HD drive type */
            ret = 0;
            break;
        case -2:
            /* set DD drive type */
            ret = 0;
            break;
        case -1:
            /* current rate */
            ret = fdc_seekrate(0);
            break;
        case 0:
            ret = fdc_seekrate(6);
            break;
        case 1:
            ret = fdc_seekrate(12);
            break;
        case 2:
            ret = fdc_seekrate(2);
            break;
        case 3:
            ret = fdc_seekrate(3);
            break;
    }

    /* convert actual rate to enum */
    if ((args->newrate >= -1) && (args->newrate <= 3)) {
        if (ret >= 12)      { ret = 1; }
        else if (ret >= 6)  { ret = 0; }
        else if (ret >= 3)  { ret = 3; }
        else                { ret = 2; }
    }

    return ret;
}




/******************************************************************************
***                                                                         ***
***                               DRIVER                                    ***
***                                                                         ***
******************************************************************************/


int16_t Createcookie(uint32_t id, uint32_t value)
{
    /* find jar slot */
    int32_t cookies_size = 0;
	int32_t cookies_used = 0;
    int32_t cookies_avail = 0;
	uint32_t* jar = (uint32_t*) *((uint32_t*)0x5a0);
	uint32_t* c = jar;
	while (1) {
		cookies_used++;
		if (c[0] == id) {
			c[1] = value;
			return E_OK;
		}
		else if (c[0] == 0) {
            cookies_size = c[1];
			break;
		}
		c += 2;
	}

    /* grow jar */
    cookies_avail = cookies_size - cookies_used;
	if (cookies_avail <= 0) {
        uint32_t* newjar;
        int32_t oldsize = (2*4*(cookies_size + 0));
        int32_t newsize = (2*4*(cookies_size + 8));
        cookies_size += 8;
        newjar = (uint32_t*)Mxalloc(newsize, 3);
        if (!newjar) {
            return ENSMEM;
        }
        memcpy(newjar, jar, oldsize);
        *((uint32_t*)0x5a0) = (uint32_t)newjar;
        jar = newjar;
	}

	/* install cookie */
	jar[(cookies_used<<1)-2] = id;
	jar[(cookies_used<<1)-1] = value;
	jar[(cookies_used<<1)+0] = 0;
	jar[(cookies_used<<1)+1] = cookies_size;
    return E_OK;
}


int16_t CreateVblHandler(uint32_t func) {
    uint32_t* pfnew = 0;
    uint32_t* pfold = *((uint32_t**)0x456);
    uint16_t max = *((uint16_t*)0x454);
    uint16_t i;
    /* find free vblqueue entry */
    for (i=0; i<max; i++) {
        if (pfold[i] == 0) {
            pfold[i] = func;
            return E_OK;
        }
    }
    /* grow queue */
    pfnew = (uint32_t*)Mxalloc(4L * max * 2, 3);
    if (!pfnew) {
        return ENSMEM;
    }
    memset(pfnew, 0, 4L * max * 2);
    memcpy(pfnew, pfold, 4L * max);
    pfnew[max] = func;
    *((uint32_t**)0x456) = pfnew;
    return E_OK;
}


int16_t driver_init(void) {
    int16_t err;
    int16_t seekrate = 0;
    int16_t nflops = *((int16_t*)0x4a6);
    uint32_t drvbits = *((uint32_t*)0x4c2);

    /* find free drive letter */
#if DEBUG_DEV
    floppy_devno = 0;
    drvbits |= (1 << floppy_devno);
    nflops = 1;
#else
    if ((drvbits & 1) == 0) {
        floppy_devno = 0;
    } else if ((drvbits & 2) == 0) {
        floppy_devno = 1;
    } else {
        return EMOUNT;
    }
    drvbits |= (1 << floppy_devno);
    nflops++;
#endif

    /* install variables */
    *((uint32_t*)0x4c2) = drvbits;
    *((int16_t*)0x4a6) = nflops;

    /* there is only one seekrate variable in tos */
    /* assume we own it if we're the first floppy */
    if (floppy_devno == 0) {
        *((int16_t*)0x440) = seekrate;
    }

    /* install xbios */
    xbios_old = *((uint32_t*)0xb8);
    *((uint32_t*)0xb8) = (uint32_t)&xbios_new;

    /* intall bios */
    hdv_old_getbpb = *((uint32_t*)0x472);
    hdv_old_rw = *((uint32_t*)0x476);
    hdv_old_mediach = *((uint32_t*)0x47e);
    *((uint32_t*)0x472) = (uint32_t)&hdv_new_getbpb;
    *((uint32_t*)0x476) = (uint32_t)&hdv_new_rw;
    *((uint32_t*)0x47e) = (uint32_t)&hdv_new_mediach;

    /* install cookie */
    err = Createcookie(0x5F464443UL, 0x01464443UL); /* '_FDC' = 0x01,'FDC' (1.44MB support) */
    if (err != E_OK) {
        return err;
    }

    /* install vbl handler */
    err = CreateVblHandler((uint32_t)fdc_update);
    if (err != E_OK) {
        return err;
    }

    return err;
}


/*-------------------------------------------------------------------------------
    program
-------------------------------------------------------------------------------*/

long super_main(int args, char* argv[]) {
    long err; (void)args; (void)argv;

    err = fdc_init();
    if (err != E_OK) {
        return err;
    }

    err = driver_init();
    if (err != E_OK) {
        return err;
    }

    return (long)E_OK;
}


int main(int args, char* argv[]) {
    int err;

    
    Cconws("\33p ISA Floppy driver \33q\r\n");
    err = (int)Supmain(args, argv, super_main);

    switch(err) {
        case E_OK:
            {
                char tmp[] = "Drive: A\r\n\r\n";
                tmp[7] += floppy_devno;
                Cconws(tmp);
                Ptermres(_PgmSize, 0);
            }
            break;
        case EUNDEV:
            Cconws("Err: Device not found");
            break;
        case EMOUNT:
            Cconws("Err: No free drive letter");
            break;
        case ENSMEM:
            Cconws("Err: Out of memory");
            break;
        default:
            Cconws("Err: Unknown");
            break;
    }
    Cconws("\r\n\r\n");
    return err;
}
