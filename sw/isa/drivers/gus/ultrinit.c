
/* --------------------------------------------------------------
 * ultrinit.c
 * Gravis Ultrasound initialization
 *
 *  Simple card init. Many things missing.
 *  Perhaps better to port and extend the MS-DOS versions of
 *  ultrinit.exe & iwinit.exe
 *
 * todo:
 *  detect card from isa_bios
 *  some way of manually specifying card port+irq+dma; env variable?
 *  rom detect and setup
 *  reset/default a lot of missing stuff
 *  test and make work on GF1 cards
 *
 *  Should this program install a _GUS cookie with
 *  port settings and card info?
 *
 *  MS-DOS use environment variable through autoexec.bat
 *  which every program (including ultrinit.exe) reads
 *
 *--------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "ext.h"
#include "mint/basepage.h"
#include "mint/cookie.h"
#include "mint/osbind.h"
#define ISA_EXCLUDE_LIB
#include "isa.h"

#if 0
#define DEBUG
#endif

typedef struct
{
    uint32_t total;
    uint32_t size[4];
    uint16_t cfg;
} t_gus_ram;

typedef struct
{
    uint16_t port;
    uint8_t  irq[2];
    uint8_t  dma[2];

    uint8_t   iw;           /* gf1 or interwave */
    uint32_t  banksize[4];

    uint32_t  p2xr;
    uint32_t  p3xr;
    uint32_t  pcodar;
    uint32_t  pcdrar;
    uint32_t  p201ar;
    uint32_t  p388ar;

    t_gus_ram ram_gf1;
    t_gus_ram ram_amd;
} t_gus;

static t_gus gusdev;
isa_t* isa;


/* -------------------------------------------------------------------- */
static void delayms(uint32_t ms)
{
	uint32_t cycles = ms / 5;
	uint32_t start  = *((volatile uint32_t*)0x4ba);
    while (1) {
        uint32_t now = *((volatile uint32_t*)0x4ba);
        if (now < start) { start = now; }
        if ((now - start) < cycles) { break; }
    }
}

/* -------------------------------------------------------------------- */
static void gusWriteSynthRegB(uint8_t reg, uint8_t data) {
    isa->outp(gusdev.p3xr + 3, reg);
    isa->outp(gusdev.p3xr + 5, data);
}

static void gusWriteSynthRegW(uint8_t reg, uint16_t data) {
    isa->outp( gusdev.p3xr + 3, reg);
    isa->outpw(gusdev.p3xr + 4, data);
}

static void gusWriteSynthRam(uint32_t addr, uint8_t data) {
    uint8_t  a8  = (addr >> 16) & 0xff;
    uint16_t a16 = ((addr & 0xff00UL)>>8) | ((addr & 0x00ffUL)<<8);
    isa->outp( gusdev.p3xr + 3, 0x43);
    isa->outpw(gusdev.p3xr + 4, a16);
    isa->outp( gusdev.p3xr + 3, 0x44);
    isa->outp( gusdev.p3xr + 5, a8);
    isa->outp( gusdev.p3xr + 7, data);
}

static uint8_t gusReadSynthRegB(uint8_t reg) {
    isa->outp(gusdev.p3xr + 3, reg);
    return isa->inp( gusdev.p3xr + 5);
}

static uint8_t gusReadSynthRegW(uint8_t reg) {
    isa->outp(gusdev.p3xr + 3, reg);
    return isa->inpw( gusdev.p3xr + 4);
}

static uint8_t gusReadSynthRam(uint32_t addr) {
    uint8_t  a8  = (addr >> 16) & 0xff;
    uint16_t a16 = ((addr & 0xff00UL)>>8) | ((addr & 0x00ffUL)<<8);
    isa->outp( gusdev.p3xr + 3, 0x43);
    isa->outpw(gusdev.p3xr + 4, a16);
    isa->outp( gusdev.p3xr + 3, 0x44);
    isa->outp( gusdev.p3xr + 5, a8);
    return isa->inp(gusdev.p3xr + 7);
}

static uint8_t gusSetEnhancedMode(uint8_t enable) {
    if (enable) {
        gusWriteSynthRegB(0x19, gusReadSynthRegB(0x99) | 0x01);
        return 0x01 & gusReadSynthRegB(0x99);
    }
    gusWriteSynthRegB(0x19, gusReadSynthRegB(0x99) & ~0x01);
    return 0;
}

/* -------------------------------------------------------------------- */

static uint32_t gusDetectRamGF1(uint32_t max, uint32_t* bank_sizes, uint16_t* ram_cfg)
{
    uint32_t i,j;
    uint8_t oldmem[4];
    uint32_t total = 0;
    (void)max;

    for (i=0; i<4; i++) {
        oldmem[i]=gusReadSynthRam(i*256*1024UL);
    }

    total = 256 * 1024UL;
    gusWriteSynthRam(0,1);
    gusWriteSynthRam(0,1);
    for (i=1; i<4; i++) {
        gusWriteSynthRam(i*256*1024UL, 15+i);
        gusWriteSynthRam(i*256*1024UL, 15+i);
        for (j=0; j<i; j++) {
            if (gusReadSynthRam(j*256*1024UL)!=(1+j))
                break;
            if (gusReadSynthRam(j*256*1024UL)!=(1+j))
                break;
        }

        if (j!=i) {
            break;
        }
        if (gusReadSynthRam(i*256*1024UL)!=(15+i)) {
            break;
        }
        if (gusReadSynthRam(i*256*1024UL)!=(15+i)) {
            break;
        }

        gusWriteSynthRam(i*256*1024UL, 1+i);
        gusWriteSynthRam(i*256*1024UL, 1+i);
        total+=256*1024UL;
    }

    for (i=0; i<4; i++) {
        gusWriteSynthRam(i*256*1024UL, oldmem[i]);
        gusWriteSynthRam(i*256*1024UL, oldmem[i]);
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

static uint32_t gusDetectRamAMD(uint32_t max, uint32_t* bank_sizes, uint16_t* ram_cfg)
{
    uint32_t bank;
    uint32_t stepsize = 16 * 1024UL;
    uint32_t max_bank_size = 4 * 1024 * 1024UL;
    uint32_t total = 0;
    uint32_t bsize[4];
    bsize[0] = bsize[1] = bsize[2] = bsize[3] = 0;

    #ifdef DEBUG
    printf("detectAMD: %08lx\n", max);
    #endif

    for (bank=0; bank<4; bank++) {
        uint32_t pos;
        uint32_t bank_addr = (bank<<22);
        #ifdef DEBUG
        printf("bank_addr %ld : %08lx\n", bank, bank_addr);
        #endif
        gusWriteSynthRam(bank_addr, (0xAA+bank));
        for (pos = 0; pos < max_bank_size; pos += stepsize) {
            if (total < max) {
                uint32_t r0,r1, test_addr, test_val;
                test_addr = bank_addr + (pos + stepsize - 1);
                test_val = bank + 0x55;
                gusWriteSynthRam(test_addr, test_val);
                r0 = gusReadSynthRam(bank_addr);
                r1 = gusReadSynthRam(test_addr);
                if ((r0 != (0xAA+bank)) || (r1 != test_val)) {
                    pos = max_bank_size;
                } else
                {
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
        #ifdef DEBUG
        printf("bs0 %08lx\n", bank_sizes[0]);
        printf("bs1 %08lx\n", bank_sizes[1]);
        printf("bs2 %08lx\n", bank_sizes[2]);
        printf("bs3 %08lx\n", bank_sizes[3]);
        #endif
    }

    if (ram_cfg) {
        uint32_t ram_ctrl = (bsize[3] >> 18);
        ram_ctrl = (ram_ctrl<<8) | (bsize[2]>>18);
        ram_ctrl = (ram_ctrl<<8) | (bsize[1]>>18);
        ram_ctrl = (ram_ctrl<<8) | (bsize[0]>>18);
        #ifdef DEBUG
        printf("ram_ctrl : %08lx\n", ram_ctrl);
        #endif
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
        #ifdef DEBUG
        printf("ram_cfg : %04x\n", *ram_cfg);
        #endif
    }
    #ifdef DEBUG
    printf("total = %08lx\n", total);
    #endif
    return total;
}

static uint32_t gusDetectRam(uint32_t max, uint32_t* bank_sizes, uint16_t* ram_cfg) {
    return (gusdev.iw) ? gusDetectRamAMD(max, bank_sizes, ram_cfg) : gusDetectRamGF1(max, bank_sizes, ram_cfg);
}

static int gusDetect(uint16_t port)
{
    uint8_t verify;

    /* setup ports */
    gusdev.port = port;
    gusdev.p2xr = port;
    gusdev.p3xr = gusdev.port + 0x100;
    gusdev.pcodar = gusdev.port + 0x10C;

    #ifdef DEBUG
    printf("try detect at: %04lx, %04lx, %04lx\r\n", gusdev.p2xr, gusdev.p3xr, gusdev.pcodar);
    #endif

    /* reset */
    gusWriteSynthRegB(0x4C, 0x00);
    delayms(20);
    gusWriteSynthRegB(0x4C, 0x01);
    delayms(20);

    /* try reading and writing synth ram */
    gusWriteSynthRam(0x000000UL, 0xAA);
    gusWriteSynthRam(0x000100UL, 0x55);

    verify = gusReadSynthRam(0x000000UL);
    if (verify != 0xAA) {
        #ifdef DEBUG
        printf("fail. readback = %02x\r\n", verify);
        printf("000000 = %02x\r\n", gusReadSynthRam(0x000000UL));
        printf("000001 = %02x\r\n", gusReadSynthRam(0x000001UL));
        printf("000100 = %02x\r\n", gusReadSynthRam(0x000100UL));
        printf("000101 = %02x\r\n", gusReadSynthRam(0x000101UL));
        #endif
        return 0;
    }

    /* gf1 or amd chip? */
    gusdev.iw = gusSetEnhancedMode(1);

    /* gf1 dram settings */
    gusSetEnhancedMode(0);
    gusWriteSynthRegB(0x53, gusReadSynthRegB(0x53) & 0xFD);     /* select dram access */
    gusWriteSynthRegW(0x52, 0x0000);                            /* rom/ram config */
    gusdev.ram_gf1.total = gusDetectRam(16 * 1024 * 1024UL, gusdev.ram_gf1.size, &gusdev.ram_gf1.cfg);
    gusWriteSynthRegW(0x52, gusReadSynthRegB(0x52) | gusdev.ram_gf1.cfg);

    #ifdef DEBUG
    printf("gf1 ram = %ld\r\n", gusdev.ram_gf1.total);
    #endif

    /* amd dram config */
    if (gusSetEnhancedMode(1)) {
        gusWriteSynthRegW(0x52, (gusReadSynthRegW(0x52) & 0xfff0) | 0x000c);
        gusdev.ram_amd.total = gusDetectRam(16 * 1024 * 1024UL, gusdev.ram_amd.size, &gusdev.ram_amd.cfg);
        gusWriteSynthRegW(0x52, (gusReadSynthRegW(0x52) & 0xfff0) | gusdev.ram_amd.cfg);
    }

    #ifdef DEBUG
    printf("amd ram = %lu\r\n", gusdev.ram_amd.total);
    #endif

    /* todo: rom bank settings */

    /* amd mixer settings */
    if (gusSetEnhancedMode(1)) {

        uint8_t b = isa->inp(gusdev.pcodar+0) & 0xe0;

        isa->outp(gusdev.pcodar+0, b | 0x0a);       /* external control reg */
        isa->outp(gusdev.pcodar+1, 0x02);           /* global irq enable */

        /* select mode 3 */
        isa->outp(gusdev.pcodar+0, b | 0x0c);        /* mode */
        isa->outp(gusdev.pcodar+1, 0x6c);            /* 3 */
#if 0
        isa->outp(gusdev.pcodar+0, b | 0x08);        /* playback format */
        isa->outp(gusdev.pcodar+1, 0x5b);            /* pio, 1chn, record di, playback en */
#endif
        isa->outp(gusdev.pcodar+0, b | 0x09);       /* conf1 */
        isa->outp(gusdev.pcodar+1, 0xc5);           /* pio, 1chn, record di, playback en */
#if 0
        isa->outp(gusdev.pcodar+0, b | 0x10);       /* conf2 */
        isa->outp(gusdev.pcodar+1, 0x80);           /* full scale voltage select */
#endif
        isa->outp(gusdev.pcodar+0, b | 0x11);       /* conf3 */
        isa->outp(gusdev.pcodar+1, 0x02);           /* enable synth */

        isa->outp(gusdev.pcodar+0, b | 0x00);       /* adc source left */
        isa->outp(gusdev.pcodar+1, 0x40);           /* aux1 */
        isa->outp(gusdev.pcodar+0, b | 0x01);       /* adc source right */
        isa->outp(gusdev.pcodar+1, 0x40);           /* aux1 */

        isa->outp(gusdev.pcodar+0, b | 0x02);       /* choose aux1 or synth left */
        isa->outp(gusdev.pcodar+1, 0x0f);           /* synth */
        isa->outp(gusdev.pcodar+0, b | 0x03);       /* choose aux1 or synth right */
        isa->outp(gusdev.pcodar+1, 0x0f);           /* synth */

        isa->outp(gusdev.pcodar+0, b | 0x04);       /* aux2 left gain */
        isa->outp(gusdev.pcodar+1, 0x0c);
        isa->outp(gusdev.pcodar+0, b | 0x05);       /* aux2 right gain */
        isa->outp(gusdev.pcodar+1, 0x0c);

        isa->outp(gusdev.pcodar+0, b | 0x06);       /* dac left gain */
        isa->outp(gusdev.pcodar+1, 0x80);
        isa->outp(gusdev.pcodar+0, b | 0x07);       /* dac right gain */
        isa->outp(gusdev.pcodar+1, 0x80);

        isa->outp(gusdev.pcodar+0, b | 0x12);       /* line-in left gain */
        isa->outp(gusdev.pcodar+1, 0x0c);
        isa->outp(gusdev.pcodar+0, b | 0x13);       /* line-in right gain */
        isa->outp(gusdev.pcodar+1, 0x0c);

        isa->outp(gusdev.pcodar+0, b | 0x16);       /* mic-in left gain */
        isa->outp(gusdev.pcodar+1, 0x10);
        isa->outp(gusdev.pcodar+0, b | 0x17);       /* mic-in right gain */
        isa->outp(gusdev.pcodar+1, 0x10);

        isa->outp(gusdev.pcodar+0, b | 0x19);       /* line-out left attenuation */
        isa->outp(gusdev.pcodar+1, 0x20);
        isa->outp(gusdev.pcodar+0, b | 0x1b);       /* line-out right attenuation */
        isa->outp(gusdev.pcodar+1, 0x20);

        gusWriteSynthRegB(0x59, 0x00);              /* compatibility mode */
    }

    gusWriteSynthRegB(0x4c, 0x07);                  /* synth irq enable, dac enable */
    isa->outp(gusdev.p2xr+0, 0x08);                 /* irq enable, mic disable, line-out enable, line-in enable */

    return 1;
}

static int gusAutoDetect(void)
{
    uint16_t port;
    const uint16_t port_st = 0x200;
    const uint16_t port_en = 0x280;

    /* ask isa_bios */
    isa_dev_t* dev = isa->find_dev("GRV0000", 0);
    if (dev) {
        #ifdef DEBUG
        printf("found in pnp registry\r\n");
        #endif
        return gusDetect(dev->port[0]);
    }

    /* poke at the bus like a caveman */
    #ifdef DEBUG
    printf("probing bus range: %04x-%04x\r\n", port_st, port_en);
    #endif
    for (port = port_st; port < port_en; port += 0x10) {
        if (gusDetect(port)) {
            return 1;
        }
    }
    return 0;
}

/* -------------------------------------------------------------------- */

int32_t super_main(void) {
    uint32_t ram;

    if (Getcookie(C__ISA, (long*)&isa) != C_FOUND) {
        #ifdef DEBUG
        printf("fail: no isa cookie\r\n");
        #endif        
        return 0;
    }

    if (!gusAutoDetect()) {
        #ifdef DEBUG
        printf("fail: card not detected\r\n");
        #endif
        return 0;
    }

    ram = gusdev.iw ? gusdev.ram_amd.total : gusdev.ram_gf1.total;
    printf("Gravis Ultrasound (%s) %lu%s at port 0x%03x\r\n",
        gusdev.iw ? "AMD" : "GF1",
        ram >= (1024 * 1024UL) ? ram / (1024 * 1024UL) : ram / 1024,
        ram >= (1024 * 1024UL) ? "MB" : "KB",
        gusdev.port);

    /* default to GF1 mode */
#if 0
    gusWriteSynthRegB(0x19, gusReadSynthRegB(0x99) & ~0x01);
    gusWriteSynthRegW(0x52, gusReadSynthRegB(0x52) | gusdev.ram_gf1.cfg);
#endif

    return 1;
}

int32_t main() {
    if (!Supexec(super_main)) {
        return -1;
    }
    /* ExitTsr(); */
    return 0;
}
