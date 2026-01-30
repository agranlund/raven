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

bool fdc_init(void);
void fdc_update(void);

bool fdc_changed(void);
bool fdc_getbpb(fdc_bpb_t* bpb);

bool fdc_read_lba(uint8_t* buf, uint8_t count, uint32_t lba);
bool fdc_write_lba(uint8_t* buf, uint8_t count, uint32_t lba);

bool fdc_read_chs(uint8_t* buf, uint8_t count, uint8_t c, uint8_t h, uint8_t s);
bool fdc_write_chs(uint8_t* buf, uint8_t count, uint8_t c, uint8_t h, uint8_t s);


#endif /* _FDC_H_ */
