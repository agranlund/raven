/*-------------------------------------------------------------------------------
 * rvsnd : isa soundblaster driver
 * (c)2025 Anders Granlund
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

#ifndef _SBDRIVER_H_
#define _SBDRIVER_H_

#define SBTYPE_NONE         0
#define SBTYPE_SB1          1
#define SBTYPE_SB2          2
#define SBTYPE_SBPRO        3
#define SBTYPE_SB16         4
#define SBTYPE_ESS          5
#define SBTYPE_OPL3SA       6

typedef struct {
    volatile uint8_t* base;
    uint16_t irq;
    uint16_t port;
    uint16_t version;
    uint16_t type;
} sb_info_t;

typedef struct {
    volatile uint8_t* base;
    uint8_t port;
} wss_info_t;

typedef struct {
    volatile uint8_t* base;
    uint16_t port;
} opl3sa_info_t;

typedef struct {
    uint16_t version;
} ess_info_t;


extern isa_t*           bus;

extern sb_info_t        sb;
extern wss_info_t       wss;
extern opl3sa_info_t    sax;
extern ess_info_t       ess;

extern bool sb_detect(sb_info_t* out, uint16_t port);
extern bool wss_detect(wss_info_t* out, uint16_t port);
extern bool sax_detect(opl3sa_info_t* out, sb_info_t* sb, uint16_t port);
extern bool ess_detect(ess_info_t* out, sb_info_t* sb);

extern void sb_init_mixer(rvdev_mix_t* out);
extern void sax_init_mixer(rvdev_mix_t* out);
extern void ess_init_mixer(rvdev_mix_t* out, bool midivol_auxb);

static void sb_mixer_outp(uint8_t _r, uint8_t _d)  {sb.base[4] = _r; sb.base[5]  = _d; }
static uint8_t sb_mixer_inp(uint8_t _r) { sb.base[4]  = _r; return sb.base[5]; }

extern void ct1345_mixer_set(uint16_t idx, uint16_t data);
extern uint16_t ct1345_mixer_get(uint16_t idx);


#endif /* _SBDRIVER_H_ */
