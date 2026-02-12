/*-------------------------------------------------------------------------------
 * rvsnd : isa ultrasound driver
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
#include "driver.h"
#include "isa/isa.h"
#include "gus.h"

/* -------------------------------------------------------------------- */
static uint8_t p3xr_rd(uint8_t reg) {
    bus->outp(gus.p3xr + 3, reg);
    return bus->inp(gus.p3xr + 5);
}

static uint8_t p3xr_rdw(uint8_t reg) {
    bus->outp(gus.p3xr + 3, reg);
    return bus->inpw(gus.p3xr + 4);
}

static void p3xr_wr(uint8_t reg, uint8_t data) {
    bus->outp(gus.p3xr + 3, reg);
    bus->outp(gus.p3xr + 5, data);
}

static void p3xr_wrw(uint8_t reg, uint16_t data) {
    bus->outp( gus.p3xr + 3, reg);
    bus->outpw(gus.p3xr + 4, data);
}

static uint8_t ram_rd(uint32_t addr) {
    uint8_t  a8  = (addr >> 16) & 0xff;
    uint16_t a16 = ((addr & 0xff00UL)>>8) | ((addr & 0x00ffUL)<<8);
    bus->outp( gus.p3xr + 3, 0x43);
    bus->outpw(gus.p3xr + 4, a16);
    bus->outp( gus.p3xr + 3, 0x44);
    bus->outp( gus.p3xr + 5, a8);
    return bus->inp(gus.p3xr + 7);
}

static void ram_wr(uint32_t addr, uint8_t data) {
    uint8_t  a8  = (addr >> 16) & 0xff;
    uint16_t a16 = ((addr & 0xff00UL)>>8) | ((addr & 0x00ffUL)<<8);
    bus->outp( gus.p3xr + 3, 0x43);
    bus->outpw(gus.p3xr + 4, a16);
    bus->outp( gus.p3xr + 3, 0x44);
    bus->outp( gus.p3xr + 5, a8);
    bus->outp( gus.p3xr + 7, data);
}

/* -------------------------------------------------------------------- */
static uint8_t gus_mode(uint8_t enable) {
    if (enable) {
        p3xr_wr(0x19, p3xr_rd(0x99) | 0x01);
        return 0x01 & p3xr_rd(0x99);
    }
    p3xr_wr(0x19, p3xr_rd(0x99) & ~0x01);
    return 0;
}



/* -------------------------------------------------------------------- */
static uint32_t detect_ram_gf1(uint32_t max, uint32_t* bank_sizes, uint16_t* ram_cfg)
{
    uint32_t i,j;
    uint8_t oldmem[4];
    uint32_t total = 256 * 1024UL;
    (void)max;

    for (i=0; i<4; i++) {
        oldmem[i] = ram_rd(i*256*1024UL);
    }

    ram_wr(0,1);
    ram_wr(0,1);
    for (i=1; i<4; i++) {
        ram_wr(i*256*1024UL, 15+i);
        ram_wr(i*256*1024UL, 15+i);
        for (j=0; j<i; j++) {
            if (ram_rd(j*256*1024UL)!=(1+j)) { break; }
            if (ram_rd(j*256*1024UL)!=(1+j)) { break; }
        }

        if (j!=i) { break; }
        if (ram_rd(i*256*1024UL)!=(15+i)) { break; }
        if (ram_rd(i*256*1024UL)!=(15+i)) { break; }

        ram_wr(i*256*1024UL, 1+i);
        ram_wr(i*256*1024UL, 1+i);
        total+=256*1024UL;
    }

    for (i=0; i<4; i++) {
        ram_wr(i*256*1024UL, oldmem[i]);
        ram_wr(i*256*1024UL, oldmem[i]);
    }

    if (bank_sizes) {
        bank_sizes[0] = total;
        bank_sizes[1] = 0;
        bank_sizes[2] = 0;
        bank_sizes[3] = 0;
    }

    if (ram_cfg) {
        ram_cfg = 0;
    }

    return total;
}

static uint32_t detect_ram_amd(uint32_t max, uint32_t* bank_sizes, uint16_t* ram_cfg)
{
    uint32_t bank;
    uint32_t stepsize = 16 * 1024UL;
    uint32_t max_bank_size = 4 * 1024 * 1024UL;
    uint32_t bsize[4] = { 0, 0, 0, 0 };
    uint32_t total = 0;

    for (bank=0; bank<4; bank++) {
        uint32_t pos;
        uint32_t bank_addr = (bank<<22);
        ram_wr(bank_addr, (0xAA+bank));
        for (pos = 0; pos < max_bank_size; pos += stepsize) {
            if (total < max) {
                uint32_t r0,r1, test_addr, test_val;
                test_addr = bank_addr + (pos + stepsize - 1);
                test_val = bank + 0x55;
                ram_wr(test_addr, test_val);
                r0 = ram_rd(bank_addr);
                r1 = ram_rd(test_addr);
                if ((r0 != (0xAA+bank)) || (r1 != test_val)) {
                    pos = max_bank_size;
                } else {
                    bsize[bank] = pos + stepsize;
                    total += stepsize;
                }
            } else {
                bank = 4; pos = max_bank_size;
            }
        }
    }

    if (bank_sizes) {
        bank_sizes[0] = bsize[0];
        bank_sizes[1] = bsize[1];
        bank_sizes[2] = bsize[2];
        bank_sizes[3] = bsize[3];
    }

    if (ram_cfg) {
        uint32_t ram_ctrl = (bsize[3] >> 18);
        ram_ctrl = (ram_ctrl<<8) | (bsize[2]>>18);
        ram_ctrl = (ram_ctrl<<8) | (bsize[1]>>18);
        ram_ctrl = (ram_ctrl<<8) | (bsize[0]>>18);
        if      (ram_ctrl == 0x00000001UL) *ram_cfg = 0x0;
        else if (ram_ctrl == 0x00000101UL) *ram_cfg = 0x1;
        else if (ram_ctrl == 0x00010101UL) *ram_cfg = 0x1;
        else if (ram_ctrl == 0x01010101UL) *ram_cfg = 0x2;
        else if (ram_ctrl == 0x00000401UL) *ram_cfg = 0x3;
        else if (ram_ctrl == 0x00010401UL) *ram_cfg = 0x4;
        else if (ram_ctrl == 0x00040401UL) *ram_cfg = 0x4;
        else if (ram_ctrl == 0x01040401UL) *ram_cfg = 0x4;
        else if (ram_ctrl == 0x04040401UL) *ram_cfg = 0x4;
        else if (ram_ctrl == 0x00040101UL) *ram_cfg = 0x5;
        else if (ram_ctrl == 0x01040101UL) *ram_cfg = 0x6;
        else if (ram_ctrl == 0x04040101UL) *ram_cfg = 0x6;
        else if (ram_ctrl == 0x00000004UL) *ram_cfg = 0x7;
        else if (ram_ctrl == 0x00000104UL) *ram_cfg = 0x8;
        else if (ram_ctrl == 0x00000404UL) *ram_cfg = 0x8;
        else if (ram_ctrl == 0x00010404UL) *ram_cfg = 0x9;
        else if (ram_ctrl == 0x00040404UL) *ram_cfg = 0x9;
        else if (ram_ctrl == 0x01040404UL) *ram_cfg = 0x9;
        else if (ram_ctrl == 0x04040404UL) *ram_cfg = 0x9;
        else if (ram_ctrl == 0x00000010UL) *ram_cfg = 0xA;
        else if (ram_ctrl == 0x00000110UL) *ram_cfg = 0xB;
        else if (ram_ctrl == 0x00000410UL) *ram_cfg = 0xB;
        else if (ram_ctrl == 0x00001010UL) *ram_cfg = 0xB;
        else                               *ram_cfg = 0xC;
    }
    return total;
}

/* -------------------------------------------------------------------- */
static rvmixctrl_t mixer_g1_ctrls[] = {
    { 0,        0,      0, 0                    }
};
static void mixer_gf1_set(uint16_t idx, uint16_t data) {
    (void)idx; (void)data;
}

static uint16_t mixer_gf1_get(uint16_t idx) {
    return 0;
}

/* -------------------------------------------------------------------- */

/*
 hacks galore:
 gus attenuation goes from 0 to about twice the -db compared to soundblaster
 se we only using 4 of the 5 bits to end up with a roughly equivalent range.
 Better would be if we exposed db ranges instead of bitdepths.
*/

static rvmixctrl_t mixer_amd_ctrls[] = {
    { "Master", 0x0019, 4, RVMIX_XBIOS_MASTER   },  /* gus: line-out attenuation    */
    { "Voice",  0x0002, 4, RVMIX_XBIOS_PCM      },  /* gus: synth/aux1 gain         */
    { "Line",   0x0012, 4, RVMIX_XBIOS_LINE     },  /* gus: line-in gain            */
    { "Mic",    0x0016, 4, RVMIX_XBIOS_MIC      },  /* gus: mic gain                */
    { "Aux2",   0x0004, 4, RVMIX_XBIOS_AUX      },  /* gus: aux2 gain               */
    { 0,        0,      0, 0                    }
};

/*
    bit7 = mute
 0, 1    adc                     4bit    000x0000
 2, 3    aux1 / synth gain       5bit    1xx01000    gain    +12db to -34.5db
 4, 5    aux2 gain               5bit    1xx01000
 6, 7    dac                     6bit    1x000000    atten   0db to -94.5db
12,13    line-in                 5bit    1xx00000    gain    +12db to -34.5db
16,17    mic                     5bit    1x010000    gain    +12db to -34.5db
19,1B    line-out                5bit    1xx00000    atten   0db to -46.5db
1A       mono in/out             4bit    000x0000    atten   0db to -45db
*/

/* get rid of the +12db part to avoid risks of clipping synth playback */
#define GUS_GAIN_REDUCE (1 << 3)

static void mixer_amd_outp(uint8_t _r, uint8_t _d)  {
    uint8_t r1 = _r + 0;
    uint8_t r2 = _r + ((_r == 0x19) ? 2 : 1);
    uint8_t b = bus->inp(gus.pcod + 0) & 0xe0;
    bus->outp(gus.pcod+0, b | r1);  /* left  */
    bus->outp(gus.pcod+1, _d);
    bus->outp(gus.pcod+0, b | r2);  /* right */
    bus->outp(gus.pcod+1, _d);
}

static uint8_t mixer_amd_inp(uint8_t _r, uint8_t mask) {
    uint8_t vl, vr;
    uint8_t r1 = _r + 0;
    uint8_t r2 = _r + ((_r == 0x19) ? 2 : 1);
    uint8_t b = bus->inp(gus.pcod + 0) & 0xe0;
    bus->outp(gus.pcod+0, b | r1);  /* left  */
    vl = bus->inp(gus.pcod+1);
    bus->outp(gus.pcod+0, b | r2);  /* right */
    vr = bus->inp(gus.pcod+1);
    vl = ((vl & 0x80) ? mask : (vl & mask));
    vr = ((vr & 0x80) ? mask : (vr & mask));
    return ((vl + vr) >> 1);
}

static void mixer_amd_set(uint16_t idx, uint16_t data) {
    switch (idx) {
        case 0x0002: {
            uint8_t v = (0x0f - min(data, 0x0f));
            uint8_t m = (v == 0x0f) ? 0x80 : 0;
            v = min(0x1f, (v + GUS_GAIN_REDUCE));
            mixer_amd_outp(idx, (v | m));
        } break;

        default: {
            uint8_t v = (0x0f - min(data, 0x0f));
            uint8_t m = (v == 0x0f) ? 0x80 : 0;
            mixer_amd_outp(idx, (v | m));
        } break;
    }
}

static uint16_t mixer_amd_get(uint16_t idx) {
    switch (idx) {
        case 0x0002: {
            uint8_t v = mixer_amd_inp(idx, 0x1f);
            v = (v > GUS_GAIN_REDUCE) ? (v - GUS_GAIN_REDUCE) : 0;
            return (0x0f - min(0x0f, v));
        }
        default: {
            return (0x0f - min(0x0f, mixer_amd_inp(idx, 0x0f)));
        }
    }
}


bool gus_create_mixer(rvdev_mix_t* out) {
    if (gus.amd) {
        out->ctrls = mixer_amd_ctrls;
        out->set = mixer_amd_set;
        out->get = mixer_amd_get;
    }
    return (out->ctrls) ? true : false;
}

void gus_init(void) {


    /* setup mixer */
    if (gus_mode(1)) {

        uint8_t b = bus->inp(gus.pcod+0) & 0xe0;

        bus->outp(gus.pcod+0, b | 0x0a);        /* external control reg */
        bus->outp(gus.pcod+1, 0x02);            /* global irq enable */

        /* select mode 3 */
        bus->outp(gus.pcod+0, b | 0x0c);        /* mode */
        bus->outp(gus.pcod+1, 0x6c);            /* 3 */
#if 0
        bus->outp(gus.pcod+0, b | 0x08);        /* playback format */
        bus->outp(gus.pcod+1, 0x5b);            /* pio, 1chn, record di, playback en */
#endif
        bus->outp(gus.pcod+0, b | 0x09);        /* conf1 */
        bus->outp(gus.pcod+1, 0xc5);            /* pio, 1chn, record di, playback en */
#if 0
        bus->outp(gus.pcod+0, b | 0x10);        /* conf2 */
        bus->outp(gus.pcod+1, 0x80);            /* full scale voltage select */
#endif
        bus->outp(gus.pcod+0, b | 0x11);        /* conf3 */
        bus->outp(gus.pcod+1, 0x02);            /* select synth for aux1 */

        bus->outp(gus.pcod+0, b | 0x00);        /* adc left source + gain */
        bus->outp(gus.pcod+1, 0x9f);            /* mic */
        bus->outp(gus.pcod+0, b | 0x01);        /* adc right source + gain */
        bus->outp(gus.pcod+1, 0x9f);            /* mic */

        bus->outp(gus.pcod+0, b | 0x02);        /* synth/aux1 left gain */
        bus->outp(gus.pcod+1, 0x10);
        bus->outp(gus.pcod+0, b | 0x03);        /* synth/aux1 right gain */
        bus->outp(gus.pcod+1, 0x10);

        bus->outp(gus.pcod+0, b | 0x04);        /* aux2 left gain */
        bus->outp(gus.pcod+1, 0x10);
        bus->outp(gus.pcod+0, b | 0x05);        /* aux2 right gain */
        bus->outp(gus.pcod+1, 0x10);

        bus->outp(gus.pcod+0, b | 0x06);        /* dac left gain */
        bus->outp(gus.pcod+1, 0x9f);
        bus->outp(gus.pcod+0, b | 0x07);        /* dac right gain */
        bus->outp(gus.pcod+1, 0x9f);

        bus->outp(gus.pcod+0, b | 0x12);        /* line-in left gain */
        bus->outp(gus.pcod+1, 0x10);
        bus->outp(gus.pcod+0, b | 0x13);        /* line-in right gain */
        bus->outp(gus.pcod+1, 0x10);

        bus->outp(gus.pcod+0, b | 0x16);        /* mic-in left gain */
        bus->outp(gus.pcod+1, 0x10);
        bus->outp(gus.pcod+0, b | 0x17);        /* mic-in right gain */
        bus->outp(gus.pcod+1, 0x10);

        bus->outp(gus.pcod+0, b | 0x19);        /* line-out left attenuation */
        bus->outp(gus.pcod+1, 0x00);
        bus->outp(gus.pcod+0, b | 0x1b);        /* line-out right attenuation */
        bus->outp(gus.pcod+1, 0x00);

        bus->outp(gus.pcod+0, b | 0x1a);        /* mono input and output control */
        bus->outp(gus.pcod+1, 0xc0);            /* out: mute, in: mute */

        p3xr_wr(0x59, 0x00);                    /* compatibility mode */
    }
    p3xr_wr(0x4c, 0x07);                        /* synth irq enable, dac enable */
    bus->outp(gus.p2xr+0, 0x0c);                /* irq enable, mic enable, line-out enable, line-in enable */
}

/* -------------------------------------------------------------------- */
bool gus_detect(uint16_t port) {
    uint8_t test1, test2;
    uint8_t  old3 = bus->inp(port + 0x103);
    uint16_t old4 = bus->inpw(port + 0x104);
    uint8_t  old7 = bus->inp(port + 0x107);

    /* detect by writing and reading back gus ram */
    bus->outp(port + 0x103, 0x43); bus->outpw(port + 0x104, 0x0000);
    bus->outp(port + 0x103, 0x44); bus->outpw(port + 0x104, 0x0000);
    bus->outp(port + 0x107, 0xAA);
    bus->outp(port + 0x103, 0x43); bus->outpw(port + 0x104, 0x0100);
    bus->outp(port + 0x103, 0x44); bus->outpw(port + 0x104, 0x0000);
    bus->outp(port + 0x107, 0x55);
    bus->outp(port + 0x103, 0x43); bus->outpw(port + 0x104, 0x0000);
    bus->outp(port + 0x103, 0x44); bus->outpw(port + 0x104, 0x0000);
    test1 = bus->inp(port + 0x107);
    bus->outp(port + 0x103, 0x43); bus->outpw(port + 0x104, 0x0100);
    bus->outp(port + 0x103, 0x44); bus->outpw(port + 0x104, 0x0000);
    test2 = bus->inp(port + 0x107);
    if ((test1 != 0xAA) || (test2 != 0x55)) {
        /* restore and exit */
        bus->outp(port + 0x107, old7);
        bus->outpw(port + 0x104, old4);
        bus->outp(port + 0x103, old3);
        return false;
    }

    /* set up the basics */
    memset((void*)&gus, 0, sizeof(gusinfo_t));
    gus.port = port;
    gus.p2xr = port;
    gus.p3xr = port + 0x100;
    gus.pcod = port + 0x10C;

    /* reset */
    p3xr_wr(0x4C, 0x00); delayus(20000);
    p3xr_wr(0x4C, 0x01); delayus(20000);

    /* detect gf1 ram bank sizes */
    gus_mode(0);
    p3xr_wr(0x53, p3xr_rd(0x53) & 0xFD);    /* select dram access */
    p3xr_wrw(0x52, 0x0000);                 /* rom/ram config */
    gus.ram_gf1.total = detect_ram_gf1(16*1024*1024UL, gus.ram_gf1.size, &gus.ram_gf1.cfg);
    p3xr_wrw(0x52, p3xr_rd(0x52) | gus.ram_gf1.cfg);

    /* detect amd ram bank sizes */
    if (gus_mode(1)) {
        gus.amd = 1;
        p3xr_wrw(0x52, (p3xr_rdw(0x52) & 0xfff0) | 0x000c);
        gus.ram_amd.total = detect_ram_amd(16*1024*1024UL, gus.ram_amd.size, &gus.ram_amd.cfg);
        p3xr_wrw(0x52, (p3xr_rdw(0x52) & 0xfff0) | gus.ram_amd.cfg);
    }

    dprintf(("gf1 ram: %ldKb : %ld,%ld,%ld,%ld\n", gus.ram_gf1.total / 1024, gus.ram_gf1.size[0] / 1024, gus.ram_gf1.size[1] / 1024, gus.ram_gf1.size[2] / 1024, gus.ram_gf1.size[3] / 1024));
    dprintf(("amd ram: %ldKb : %ld,%ld,%ld,%ld\n", gus.ram_amd.total / 1024, gus.ram_amd.size[0] / 1024, gus.ram_amd.size[1] / 1024, gus.ram_amd.size[2] / 1024, gus.ram_amd.size[3] / 1024));

    return true;
}

