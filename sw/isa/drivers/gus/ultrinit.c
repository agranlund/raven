
//--------------------------------------------------------------
// ultrinit.c
// Gravis Ultrasound initialization
//
//  Simple card init. Many things missing.
//  Perhaps better to port and extend the MS-DOS versions of
//  ultrinit.exe & iwinit.exe
//
// todo:
//  detect card from isa_bios
//  some way of manually specifying card port+irq+dma; env variable?
//  rom detect and setup
//  reset/default a lot of missing stuff
//  test and make work on GF1 cards
//
//  Should this program install a _GUS cookie with
//  port settings and card info?
//
//  MS-DOS use environment variable through autoexec.bat
//  which every program (including ultrinit.exe) reads
//
//--------------------------------------------------------------
#include "stdio.h"
#include "string.h"
#include "ext.h"
#include "mint/basepage.h"
#include "mint/cookie.h"
#include "../../isa.h"

typedef struct
{
    unsigned int total;
    unsigned int size[4];
    unsigned short cfg;
} t_gus_ram;

typedef struct
{
    unsigned short port;
    unsigned char  irq[2];
    unsigned char  dma[2];

    unsigned char   iw;         // gf1 or interwave
    unsigned int    banksize[4];

    unsigned int    p2xr;
    unsigned int    p3xr;
    unsigned int    pcodar;
    unsigned int    pcdrar;
    unsigned int    p201ar;
    unsigned int    p388ar;

    t_gus_ram       ram_gf1;
    t_gus_ram       ram_amd;
} t_gus;

static t_gus gusdev;
isa_t* isa;


// --------------------------------------------------------------------
static void gusWriteSynthRegB(unsigned char reg, unsigned char data) {
    isa->outp(gusdev.p3xr + 3, reg);
    isa->outp(gusdev.p3xr + 5, data);
}

static void gusWriteSynthRegW(unsigned char reg, unsigned short data) {
    isa->outp(gusdev.p3xr + 3, reg);
    isa->outpw(gusdev.p3xr + 4, data);
}

static void gusWriteSynthRam(unsigned int addr, unsigned char data) {
    isa->outp( gusdev.p3xr + 3, 0x43);
    isa->outpw(gusdev.p3xr + 4, (addr & 0xffff));
    isa->outp( gusdev.p3xr + 3, 0x44);
    isa->outpw(gusdev.p3xr + 5, (addr >> 16) & 0xff);
    isa->outp( gusdev.p3xr + 7, data);
}

static unsigned char gusReadSynthRegB(unsigned char reg) {
    isa->outp(gusdev.p3xr + 3, reg);
    return isa->inp( gusdev.p3xr + 5);
}

static unsigned char gusReadSynthRegW(unsigned char reg) {
    isa->outp(gusdev.p3xr + 3, reg);
    return isa->inpw( gusdev.p3xr + 4);
}

static unsigned char gusReadSynthRam(unsigned int addr) {
    isa->outp( gusdev.p3xr + 3, 0x43);
    isa->outpw(gusdev.p3xr + 4, (addr & 0xffff));
    isa->outp( gusdev.p3xr + 3, 0x44);
    isa->outpw(gusdev.p3xr + 5, (addr >> 16) & 0xff);
    return isa->inp(gusdev.p3xr + 7);
}

static unsigned char gusSetEnhancedMode(unsigned char enable) {
    if (enable) {
        gusWriteSynthRegB(0x19, gusReadSynthRegB(0x99) | 0x01);
        return 0x01 & gusReadSynthRegB(0x99);
    }
    gusWriteSynthRegB(0x19, gusReadSynthRegB(0x99) & ~0x01);
    return 0;
}

// --------------------------------------------------------------------

int gusDetectRam(unsigned int max, unsigned int* bank_sizes, unsigned short* ram_cfg)
{
    unsigned int stepsize = 16 * 1024UL;
    unsigned int max_bank_size = 4 * 1024 * 1024UL;
    unsigned int total = 0;
    unsigned int bsize[4];
    bsize[0] = bsize[1] = bsize[2] = bsize[3] = 0;

    for (int bank=0; bank<4; bank++) {
        unsigned int bank_addr = (bank<<22);
        gusWriteSynthRam(bank_addr, (0xAA+bank));
        for (unsigned int pos = 0; pos < max_bank_size; pos += stepsize) {
            if (total >= max) {
                goto done;
            }
            unsigned int test_addr = bank_addr + (pos + stepsize - 1);
            unsigned char test_val = 0x55 + bank;
            gusWriteSynthRam(test_addr, test_val);
            unsigned int r0 = gusReadSynthRam(bank_addr);
            unsigned int r1 = gusReadSynthRam(test_addr);
            if ((r0 != (0xAA+bank)) || (r1 != test_val)) {
                break;
            }
            bsize[bank] = pos + stepsize;
            total += stepsize;
        }
    }

done:

    if (bank_sizes) {
        bank_sizes[0] = bsize[0];
        bank_sizes[1] = bsize[1];
        bank_sizes[2] = bsize[2];
        bank_sizes[3] = bsize[3];
    }

    if (ram_cfg) {
        unsigned int ram_ctrl = (bsize[3] >> 18);
        ram_ctrl = (ram_ctrl<<8) | (bsize[2]>>18);
        ram_ctrl = (ram_ctrl<<8) | (bsize[1]>>18);
        ram_ctrl = (ram_ctrl<<8) | (bsize[0]>>18);
        switch (ram_ctrl)
        {
            case 0x00000001: *ram_cfg = 0x0; break;
            case 0x00000101:
            case 0x00010101: *ram_cfg = 0x1; break;
            case 0x01010101: *ram_cfg = 0x2; break;
            case 0x00000401: *ram_cfg = 0x3; break;
            case 0x00010401:
            case 0x00040401:
            case 0x01040401:
            case 0x04040401: *ram_cfg = 0x4; break;
            case 0x00040101: *ram_cfg = 0x5; break;
            case 0x01040101:
            case 0x04040101: *ram_cfg = 0x6; break;
            case 0x00000004: *ram_cfg = 0x7; break;
            case 0x00000104:
            case 0x00000404: *ram_cfg = 0x8; break;
            case 0x00010404:
            case 0x00040404:
            case 0x01040404:
            case 0x04040404: *ram_cfg = 0x9; break;
            case 0x00000010: *ram_cfg = 0xA; break;
            case 0x00000110:
            case 0x00000410:
            case 0x00001010: *ram_cfg = 0xB; break;
            default:         *ram_cfg = 0xC; break;
        }
    }

    return total;
}

int gusDetect(unsigned short port)
{
    // setup ports
    gusdev.port = port;
    gusdev.p2xr = port;
    gusdev.p3xr = gusdev.port + 0x100;
    gusdev.pcodar = gusdev.port + 0x10C;

    // reset
    gusWriteSynthRegB(0x4C, 0x00);
    delay(20);
    gusWriteSynthRegB(0x4C, 0x01);
    delay(20);

    // try reading and writing synth ram
    gusWriteSynthRam(0x000000, 0xAA);
    gusWriteSynthRam(0x000100, 0x55);
    if (gusReadSynthRam(0x000000) != 0xAA) {
        return 0;
    }

    // gf1 or amd chip?
    gusdev.iw = gusSetEnhancedMode(1);

    // gf1 dram settings
    gusSetEnhancedMode(0);
    gusWriteSynthRegB(0x53, gusReadSynthRegB(0x53) & 0xFD);     // select dram access
    gusWriteSynthRegW(0x52, 0x0000);                            // rom/ram config
    gusdev.ram_gf1.total = gusDetectRam(16 * 1024 * 1024UL, gusdev.ram_gf1.size, &gusdev.ram_gf1.cfg);
    gusWriteSynthRegW(0x52, gusReadSynthRegB(0x52) | gusdev.ram_gf1.cfg);

    //printf("gf1 ram = %d\r\n", gusdev.ram_gf1.total);

    // amd dram config
    if (gusSetEnhancedMode(1)) {
        gusWriteSynthRegW(0x52, (gusReadSynthRegW(0x52) & 0xfff0) | 0x000c);
        gusdev.ram_amd.total = gusDetectRam(16 * 1024 * 1024UL, gusdev.ram_amd.size, &gusdev.ram_amd.cfg);
        gusWriteSynthRegW(0x52, (gusReadSynthRegW(0x52) & 0xfff0) | gusdev.ram_amd.cfg);
    }

    //printf("amd ram = %d\r\n", gusdev.ram_amd.total);

    // todo: rom bank settings

    // amd mixer settings
    if (gusSetEnhancedMode(1)) {

        unsigned char b = isa->inp(gusdev.pcodar+0) & 0xe0;

        isa->outp(gusdev.pcodar+0, b | 0x0a);       // external control reg
        isa->outp(gusdev.pcodar+1, 0x02);           // global irq enable

        // select mode 3
        isa->outp(gusdev.pcodar+0, b | 0x0c);        // mode
        isa->outp(gusdev.pcodar+1, 0x6c);            // 3

        //isa->outp(gusdev.pcodar+0, b | 0x08);        // playback format
        //isa->outp(gusdev.pcodar+1, 0x5b);            // pio, 1chn, record di, playback en

        isa->outp(gusdev.pcodar+0, b | 0x09);       // conf1
        isa->outp(gusdev.pcodar+1, 0xc5);           // pio, 1chn, record di, playback en

        //isa->outp(gusdev.pcodar+0, b | 0x10);       // conf2
        //isa->outp(gusdev.pcodar+1, 0x80);           // full scale voltage select

        isa->outp(gusdev.pcodar+0, b | 0x11);       // conf3
        isa->outp(gusdev.pcodar+1, 0x02);           // enable synth

        isa->outp(gusdev.pcodar+0, b | 0x00);       // adc source left
        isa->outp(gusdev.pcodar+1, 0x40);           // aux1
        isa->outp(gusdev.pcodar+0, b | 0x01);       // adc source right
        isa->outp(gusdev.pcodar+1, 0x40);           // aux1

        isa->outp(gusdev.pcodar+0, b | 0x02);       // choose aux1 or synth left
        isa->outp(gusdev.pcodar+1, 0x0f);           // synth
        isa->outp(gusdev.pcodar+0, b | 0x03);       // choose aux1 or synth right
        isa->outp(gusdev.pcodar+1, 0x0f);           // synth

        isa->outp(gusdev.pcodar+0, b | 0x04);       // aux2 left gain
        isa->outp(gusdev.pcodar+1, 0x08);           // 
        isa->outp(gusdev.pcodar+0, b | 0x05);       // aux2 right gain
        isa->outp(gusdev.pcodar+1, 0x08);           //

        isa->outp(gusdev.pcodar+0, b | 0x06);       // dac left gain
        isa->outp(gusdev.pcodar+1, 0x80);           // 
        isa->outp(gusdev.pcodar+0, b | 0x07);       // dac right gain
        isa->outp(gusdev.pcodar+1, 0x80);           // 

        isa->outp(gusdev.pcodar+0, b | 0x12);       // line-in left gain
        isa->outp(gusdev.pcodar+1, 0x10);           // 
        isa->outp(gusdev.pcodar+0, b | 0x13);       // line-in right gain
        isa->outp(gusdev.pcodar+1, 0x10);           // 

        isa->outp(gusdev.pcodar+0, b | 0x16);       // mic-in left gain
        isa->outp(gusdev.pcodar+1, 0x80);           //
        isa->outp(gusdev.pcodar+0, b | 0x17);       // mic-in right gain
        isa->outp(gusdev.pcodar+1, 0x80);           //

        isa->outp(gusdev.pcodar+0, b | 0x19);       // line-out left attenuation
        isa->outp(gusdev.pcodar+1, 0x20);           // 
        isa->outp(gusdev.pcodar+0, b | 0x1b);       // line-out right attenuation
        isa->outp(gusdev.pcodar+1, 0x20);           // 

        gusWriteSynthRegB(0x59, 0x00);              // compatibility mode
    }

    gusWriteSynthRegB(0x4c, 0x07);                  // synth irq enable, dac enable
    isa->outp(gusdev.p2xr+0, 0x08);                 // irq enable, mic disable, line-out enable, line-in enable
    return 1;
}

int gusAutoDetect()
{
    // ask isa_bios
    isa_dev_t* dev = isa->find_dev("GRV0000", 0);
    if (dev) {
        return gusDetect(dev->port[0]);
    }

    // poke at the bus like a caveman
    const unsigned short port_st = 0x200;
    const unsigned short port_en = 0x280;
    for (unsigned short port = port_st; port < port_en; port += 0x10) {
        if (gusDetect(port)) {
            return 1;
        }
    }
    return 0;
}

// --------------------------------------------------------------------

int super_main() {

    if (Getcookie(C__ISA, (long*)&isa) != C_FOUND) {
        return 0;
    }

    if (!gusAutoDetect()) {
        return 0;
    }

    unsigned int ram = gusdev.iw ? gusdev.ram_amd.total : gusdev.ram_gf1.total;

    printf("Gravis Ultrasound (%s) %lu%s at port 0x%03x\r\n",
        gusdev.iw ? "AMD" : "GF1",
        ram >= (1024 * 1024UL) ? ram / (1024 * 1024UL) : ram / 1024,
        ram >= (1024 * 1024UL) ? "MB" : "KB",
        gusdev.port);

    // default to GF1 mode
//    gusWriteSynthRegB(0x19, gusReadSynthRegB(0x99) & ~0x01);
//    gusWriteSynthRegW(0x52, gusReadSynthRegB(0x52) | gusdev.ram_gf1.cfg);
    return 1;
}

int main() {
    if (!Supexec(super_main)) {
        return -1;
    }
    //ExitTsr();
    return 0;
}
