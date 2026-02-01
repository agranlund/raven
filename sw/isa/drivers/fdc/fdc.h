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
#ifndef _FDC_H_
#define _FDC_H_

#include <stdbool.h>
#include <stdint.h>
#include <atarierr.h>

#ifndef DEBUG
#define DEBUG           0
#endif
#ifndef READONLY
#define READONLY        1
#endif

#ifndef DEBUG_PRINT
#define DEBUG_PRINT     (DEBUG && 1)
#endif
#ifndef DEBUG_DEV
#define DEBUG_DEV       (DEBUG && 0)
#endif

typedef struct {
    uint16_t bps;
    uint8_t  spc;
    uint16_t ressec;
    uint8_t  nfats;
    uint16_t ndirs;
    uint16_t nsects;
    uint8_t  media;
    uint16_t spf;
    uint16_t spt;
    uint16_t nheads;
    uint16_t nhid;
} fdc_bpb_t;

int16_t fdc_init(void);
void fdc_update(void);

int16_t fdc_changed(void);
int16_t fdc_getbpb(fdc_bpb_t* bpb);
int16_t fdc_seekrate(int16_t rate);

int16_t fdc_read_lba(uint8_t* buf, uint8_t count, uint32_t lba);
int16_t fdc_write_lba(uint8_t* buf, uint8_t count, uint32_t lba);

int16_t fdc_read_chs(uint8_t* buf, uint8_t count, uint8_t c, uint8_t h, uint8_t s);
int16_t fdc_write_chs(uint8_t* buf, uint8_t count, uint8_t c, uint8_t h, uint8_t s);

#if DEBUG_PRINT
extern void dprintf(char* s, ...);
extern void ddump(uint8_t* buf, uint32_t cnt);
#endif


#endif /* _FDC_H_ */
