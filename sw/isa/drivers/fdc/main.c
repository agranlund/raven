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

#define DEBUG_DEV_TEMP 1

#if DEBUG
#include <stdarg.h>
#include "raven.h"
void dprintf(char* s, ...) {
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

/*-------------------------------------------------------------------------------
    XBIOS functions
-------------------------------------------------------------------------------*/

typedef struct {void *buf; int32_t filler; int16_t devno; int16_t sectno; int16_t trackno; int16_t sideno; int16_t count; } xb_floprw_args;
int16_t xb_floprd(xb_floprw_args* args) {
    /*
    The function returns 0 if the operation was successful or a non-zero error-code if not.
    */
    return -1;
}

int16_t xb_flopwr(xb_floprw_args* args) {
    /*
    The function returns 0 if the operation was successful or a non-zero error-code if not.
    */
   return -1;
}

int16_t xb_flopver(xb_floprw_args* args) {
    /*
    The function returns the value 0 if the list stored in the parameter buf is valid, or a non-zero value otherwise.
    Note: After the call one finds in the parameter buf a NULL-terminated list of 16-bit words containing
    the numbers of the defective sectors. So the function does not compare sectors with a block of memory;
    instead it always reads the sectors into the same buffer. This only verifies that the sectors can be read correctly,
    or if read errors occur during reading.
    */

    return -1;
}

typedef struct {void *buf; int32_t filler; int16_t devno; int16_t spt; int16_t trackno; int16_t sideno; int16_t interlv; int32_t magic; int16_t virgin; } xb_flopfmt_args;
int16_t xb_flopfmt(xb_flopfmt_args* args) {
    /*
    The function returns 0 when no error has occurred in formatting the track.
    Otherwise a NULL-terminated list of the faulty sectors will be written as a int16_t array into the buffer buf.
    */    
    return 0;
}

typedef struct {int16_t devno; int16_t newrate; } xb_floprate_args;
int16_t xb_floprate(xb_floprate_args* args) {
    /*
    -1	=	Do not alter seek rate
     0	=	Seek rate of 6ms
     1	=	Seek rate of 12ms
     2	=	Seek rate of 2ms
     3	=	Seek rate of 3ms
    */

    int16_t oldrate = *((volatile int16_t*)0x440);
    int16_t newrate = oldrate;
    /* todo: set fdc seekrate */
    switch (args->newrate) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
    }
    *((int16_t*)0x440) = newrate;
    return oldrate;
}

/*-------------------------------------------------------------------------------
    BIOS functions
-------------------------------------------------------------------------------*/

typedef struct {int16_t rwflag; void* buf; int16_t cnt; int16_t recnr; int16_t dev; int32_t lrecno; } hdv_rw_args;
int32_t hdv_rw(hdv_rw_args* args) {

    uint32_t lba = (args->recnr < 0) ? args->lrecno : args->recnr;
    #if DEBUG
    dprintf("hdv_rw %02x %d, %ld, %d\n", args->rwflag, args->dev, lba, args->cnt);
    #endif

    if (args->rwflag & 1) {
        if (fdc_write_lba(args->buf, args->cnt, lba)) {
            return 0;
        }
    } else {
        if (fdc_read_lba(args->buf, args->cnt, lba)) {
            return 0;
        }
    }
    return -1;
}

int32_t hdv_getbpb(int16_t dev) {
    static BPB tos_bpb;
    fdc_bpb_t fdc_bpb;
    uint32_t temp;
    (void)dev;

    if (!fdc_getbpb(&fdc_bpb)) {
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

int32_t hdv_mediach(int16_t dev) {
    (void)dev;
    return fdc_changed() ? 2 : 0;
}



/*-------------------------------------------------------------------------------
    driver
-------------------------------------------------------------------------------*/

bool Createcookie(uint32_t id, uint32_t value)
{
    /* find free slot */
    int32_t cookies_size = 0;
	int32_t cookies_used = 0;
    int32_t cookies_avail = 0;
	uint32_t* jar = (uint32_t*) *((uint32_t*)0x5a0);
	uint32_t* c = jar;
	while (1) {
		cookies_used++;
		if (c[0] == id) {
			c[1] = value;
			return true;
		}
		else if (c[0] == 0) {
            cookies_size = c[1];
			break;
		}
		c += 2;
	}

    /* grow jar when necessary */
    cookies_avail = cookies_size - cookies_used;
	if (cookies_avail <= 0) {
        uint32_t* newjar;
        int32_t oldsize = (2*4*(cookies_size + 0));
        int32_t newsize = (2*4*(cookies_size + 8));
        cookies_size += 8;
        newjar = (uint32_t*)Mxalloc(newsize, 3);
        memcpy(newjar, jar, oldsize);
        *((uint32_t*)0x5a0) = (uint32_t)newjar;
        jar = newjar;
	}

	/* install cookie */
	jar[(cookies_used<<1)-2] = id;
	jar[(cookies_used<<1)-1] = value;
	jar[(cookies_used<<1)+0] = 0;
	jar[(cookies_used<<1)+1] = cookies_size;
	return true;    
}

void CreateVblHandler(uint32_t func) {
    uint32_t* pfnew = 0;
    uint32_t* pfold = *((uint32_t**)0x456);
    uint16_t max = *((uint16_t*)0x454);
    uint16_t i;
    for (i=0; i<max; i++) {
        if (pfold[i] == 0) {
            pfold[i] = func;
            return;
        }
    }
    pfnew = (uint32_t*)Mxalloc(4L * max * 2, 3);
    memset(pfnew, 0, 4L * max * 2);
    memcpy(pfnew, pfold, 4L * max);
    pfnew[max] = func;
    *((uint32_t**)0x456) = pfnew;
}

bool driver_init(void) {
    int16_t seekrate = 0;
    int16_t nflops = *((int16_t*)0x4a6);
    uint32_t drvbits = *((uint32_t*)0x4c2);

#if DEBUG_DEV_TEMP
    floppy_devno = 0;
    drvbits |= (1 << floppy_devno);
    nflops = 1;
#else
    if ((drvbits & 1) == 0) {
        floppy_devno = 0;
    } else if ((drvbits & 2) == 0) {
        floppy_devno = 1;
    } else {
        return -1;
    }
    drvbits |= (1 << floppy_devno);
    nflops++;
#endif

    /* install variables */
    *((uint32_t*)0x4c2) = drvbits;
    *((int16_t*)0x4a6) = nflops;
    *((int16_t*)0x440) = seekrate;

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
    Createcookie(0x5F464443UL, 0x01464443UL); /* '_FDC' = 0x01,'FDC' (1.44MB support) */

    /* install vbl handler */
    CreateVblHandler((uint32_t)fdc_update);

    return true;
}



long super_main(int args, char* argv[]) {
    (void)args; (void)argv;

    if (!fdc_init()) {
        return -1;
    }

    if (!driver_init()) {
        return -1;
    }

    return 0;
}

int main(int args, char* argv[]) {
    int ret = (int)Supmain(args, argv, super_main);
    if (ret == 0) {
        Ptermres(_PgmSize, 0);
    }
    return ret;
}





/*
    cookie:
        FDC (1.44MB support)

    xbios:
        Flopfmt
        Floprate
        Floprd
        Flopver
        Flopwr
        Protobt

    bios:
        Drvmap
        Getbpb
        Mediach
        Rwabs


    variables:
        _dskbufp     LONG 0x4c6
            Pointer to a 1024-byte buffer for reading and writing to floppy disks or hard drives (e.g. at boot-attempts).
            The pointer is also used by the VDI.

        _drvbits     LONG 0x4c2
            Bit-table for the mounted drives of the BIOS. Valid are:        
            Bit-0 = Drive A
            Bit-1 = Drive B

        _frclock     LONG 0x466
            Similar to _vbclock, with the difference that the count is not halted by vblsem.
 
        _vbclock     LONG 0x462
            Number of vertical blanks processed since the last reset.            

        _fverify     WORD 0x444
            Determines whether the BIOS should perform a Verify via Rwabs when writing to floppy disks, or not. Valid are:
            0 = No Verify
            Normally the Verify is switched on.

        _nflops      WORD 0x4a6
            Number of mounted floppy disk drives.


        flock        WORD 0x43e
            If there is a non-zero value here, then you must not access the DMA chip.
            So DMA device drivers must first inquire whether the DMA chip has been blocked and set flock themselves when they start work.

        hdv_bpb      LONG 0x472
            Vector to routine that establishes the BPB of a BIOS drive. The device number is passed on the stack (4(sp)).            

        hdv_init     LONG 0x46a
            Vector to the initialisation routines for the floppy disk drives.
            It is read out before reading the boot sectors, and hence can be altered only by reset-resident programs or ROM-modules.
            The tasks include:
                Initialisation of the diskette drives (_nflops is set accordingly).
                Transfer of seekrate to the internal variables of the BIOS.            

        hdv_mediach  LONG 0x47e
            Vector to routine for establishing the media-change status of a BIOS drive.
            The BIOS device number is passed on the stack (4(sp)).            

        hdv_rw       LONG 0x476
            Vector to the routine for reading and writing of blocks to BIOS drives.
            The same parameters are passed on the stack as for Rwabs (starting with 4(sp); rwflag).        


        seekrate     WORD 0x440
            Seek rate for the two floppy drives. Valid are:
            0 =  6 ms
            1 = 12 ms
            2 =  2 ms
            3 =  3 ms
            The variable is read out straight after system start by the BIOS, and ignored afterwards.
            For altering the seek rate that is used actually, one has to use the XBIOS function Floprate.        




*/