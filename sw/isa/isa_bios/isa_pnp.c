/*
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 1996, Sujal M. Patel
 * Copyright (c) 2024, Anders Granlund
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* 
 * Portions of this file is based on FreeBSD
 * https://github.com/NetApp/freebsd
 */

/*
#define DEBUG
*/



#if defined(__GNUC__)
#ifdef DEBUG
#define dbg_printf(...)    printf(__VA_ARGS__);
#else
#define dbg_printf(...)    { }
#endif
#endif


#include "isa_pnp.h"
#include "isa_probe.h"
#include "stdio.h"
#include "string.h"
#include "mint/osbind.h"

/*---------------------------------------------------------*/
uint16_t pnp_rdport;
#define pnp_wraddr      0x279
#define pnp_wrdata      0xa79

pnp_conf_t* pnpconfs;
#define PNP_MAX_CONFIGS 16
static pnp_conf_t* pnp_getconfs(uint16_t csn, uint16_t ldn) {
    uint32_t cidx = (csn & 0x0f) - 1;
    uint32_t offs = (cidx * ISA_MAX_CARD_DEVS * PNP_MAX_CONFIGS) + (ldn * PNP_MAX_CONFIGS);
    return &pnpconfs[offs];
}


/*---------------------------------------------------------*/
#define REG_SET_RDPORT              0x00
#define REG_SERIAL_ISOLATION        0x01
#define REG_CONFIG_CONTROL          0x02
#define CONFIG_CONTROL_RESET        0x01
#define CONFIG_CONTROL_WAIT_KEY     0x02
#define CONFIG_CONTROL_RESET_CSN    0x04
#define REG_WAKE_CSN                0x03
#define REG_RESOURCE_DATA           0x04
#define REG_STATUS                  0x05
#define REG_SELECT_CSN              0x06
#define REG_SELECT_LDN              0x07
#define REG_CARD_RESERVED           0x08
#define REG_VENDOR_DEFINED          0x20
#define REG_ACTIVATE                0x30
#define REG_IO_RANGE_CHECK          0x31
#define RANGE_CHECK_RD55            0x01
#define RANGE_CHECK_EN              0x02
#define REG_MEMBASE_HI(x)           (0x40 + 8*(x))
#define REG_MEMBASE_LO(x)           (0x41 + 8*(x))
#define REG_MEMCONTROL(x)           (0x42 + 8*(x))
#define MEMCONTROL_LIMIT            0x01
#define MEMCONTROL_16BIT            0x02
#define REG_MEMRANGE_HI(x)          (0x43 + 8*(x))
#define REG_MEMRANGE_LO(x)          (0x44 + 8*(x))
#define REG_IOBASE_HI(x)            (0x60 + 2*(x))
#define REG_IOBASE_LO(x)            (0x61 + 2*(x))
#define REG_IRQLEVEL(x)             (0x70 + 2*(x))
#define REG_IRQTYPE(x)              (0x71 + 2*(x))
#define REG_DMACHN(x)               (0x74 + 1*(x))


/*---------------------------------------------------------*/

static uint8_t bitmask_to_int(uint16_t mask) {
    int i;
    for (i=0; i<16; i++) {
        if (mask & (1 << i)) {
            return i;
        }
    }
    return 0;
}

/*---------------------------------------------------------*/

static bool pnp_IdToStrWithIdx(char* id, uint32_t* uid, uint32_t* idx) {
    uint32_t iid = 0; uint32_t iix = 0; bool valid = false;
    if (id) {
        uint32_t len = strlen(id);
        if (len == 7) {
            iix = 0;
            iid = StrToId(id);
            valid = true;
        } else if ((len == 9) && (id[len-2] == ':')) {
            iid = StrToId(id);
            iix = id[len-1] - '0';
            valid = (iix <= ISA_MAX_CARD_DEVS) ? true : false;
        }
    }
    if (uid) *uid = valid ? iid : 0;
    if (idx) *idx = valid ? iix : 0;
    return valid;
}

static isa_card_t* pnp_find_card(char* id, bool only_enabled) {
    uint32_t uid, uix;
    if (pnp_IdToStrWithIdx(id, &uid, &uix)) {
        uint32_t i, cnt;
        for (i=0, cnt=0; i<ISA_MAX_CARDS; i++) {
            if (isa.cards[i].vendor == uid) {
                if (cnt == uix) {
                    return ((isa.cards[i].flags & ISA_FLG_ENABLED) || !only_enabled) ? &isa.cards[i] : 0;
                }
                cnt++;
            }
        }
    }
    return 0;
}

static isa_device_t* pnp_find_dev(char* card_id, char* dev_id, bool only_enabled) {
    isa_card_t* card = pnp_find_card(card_id, only_enabled);
    if (card) {
        uint32_t uid, uix;
        if (pnp_IdToStrWithIdx(dev_id, &uid, &uix)) {
            uint32_t i, j, cnt;
            for (i=0, cnt=0; i<card->numdevices; i++) {
                bool match = false;
                for (j=0; j<ISA_MAX_DEV_IDS && !match; j++) {
                    if (card->devices[i].id[j] && (card->devices[i].id[j] == uid)) {
                        match = true;
                    }
                }
                if (match) {
                    return ((card->devices[i].flags & ISA_FLG_ENABLED) || !only_enabled) ? &card->devices[i] : 0;
                }
                cnt++;
            }
        }
    }
    return 0;
}


/*---------------------------------------------------------*/
static void pnp_inf_create_cards(void) {
    char* strc[8]; int strs = 3;
    const char* offs = 0;
    do {
        offs = GetInfCommand("card.", offs, strc, &strs);
        if (offs && (strs == 1)) {
             isa_card_t* card = &isa.cards[isa.numcards];
             isa.numcards++;
             card->flags = ISA_FLG_ENABLED;
             card->csn = isa.numcards;
             card->vendor = StrToId(strc[0]);
             strncpy(card->name, offs, 63);
        }
    } while(offs);
}

static void pnp_inf_modify_cards(void) {
    char* strc[8]; int strs = 2;
    const char* offs = 0;
    do {
        offs = GetInfCommand("card.", offs, strc, &strs);
        if (offs && (strs == 2)) {
            isa_card_t* card = pnp_find_card(strc[0], false);
            if (!card) {
                continue;
            }
            if (strcmp(strc[1], "name") == 0) {
                strncpy(card->name, offs, ISA_MAX_NAME-1);
            } else if (strcmp(strc[1], "enable") == 0) {
                if (StrToInt(offs) > 0)
                    card->flags |= ISA_FLG_ENABLED;
                else
                    card->flags &= ~ISA_FLG_ENABLED;
            } else if (strcmp(strc[2], "hidden") == 0) {
                /* todo */
            }
        }
    } while(offs);
}

static void pnp_inf_modify_devices(void) {
    char* strc[8]; int strs = 3;
    const char* offs = 0;
    do {
        offs = GetInfCommand("dev.", offs, strc, &strs);
        if (offs && (strs == 2)) {
            /* add */
            isa_card_t* card = pnp_find_card(strc[0], false);
            if (!card) {
                continue;
            } else if (card->numdevices >= ISA_MAX_CARD_DEVS) {
                continue;
            } else {
                isa_device_t* dev = &card->devices[card->numdevices];
                dev->csn = card->csn;
                dev->ldn = card->numdevices;
                dev->flags = card->flags;
                dev->id[0] = StrToId(strc[1]);
                strncpy(dev->name, offs, ISA_MAX_NAME-1);
                card->numdevices++;
            }
        }
        else if (offs && (strs == 3)) {
            isa_device_t* dev = pnp_find_dev(strc[0], strc[1], false);
            if (dev)
            {
                pnp_conf_t* conf = pnp_getconfs(dev->csn, dev->ldn);
                if (conf) {
                    char* cmd = strc[2]; int idx = 0; int len = strlen(cmd);
                    if (len > 0) {
                        if ((cmd[len-1]>='0') && (cmd[len-1]<='9')) {
                            idx = cmd[len-1] - '0';
                            cmd[len-1] = 0;
                        }
                        if (strcmp(cmd, "io") == 0) {
                            if (idx < ISA_MAX_DEV_PORT) {
                                conf->nio = idx < conf->nio ? conf->nio : idx + 1;
                                conf->flags |= ISA_FLG_ENABLED;
                                conf->iorange[idx].base_min = StrToHex(offs);
                                conf->iorange[idx].base_max = conf->iorange[idx].base_min;
                            }
                        }
                        else if (strcmp(cmd, "mem") == 0) {
                            if (idx < ISA_MAX_DEV_MEM) {
                                conf->nmem = idx < conf->nmem ? conf->nmem : idx + 1;
                                conf->flags |= ISA_FLG_ENABLED;
                                conf->memrange[idx].base_min = StrToHex(offs);
                                conf->memrange[idx].base_max = conf->memrange[idx].base_min;
                            }
                        }
                        else if (strcmp(cmd, "irq") == 0) {
                            if (idx < ISA_MAX_DEV_IRQ) {
                                uint32_t val = (uint32_t)StrToInt(offs);
                                uint32_t mask = 0x00010000UL;       /* high-true edge sensitive */
                                if (val > 0) {
                                    if (val > 15) {
                                        continue;
                                    }
                                    mask |= (1 << val);
                                }
                                conf->irqmask[idx] = mask;
                            }
                        }
                        else if (strcmp(cmd, "dma") == 0) {
                            if (idx < ISA_MAX_DEV_DMA) {
                                uint16_t val = (uint16_t)StrToInt(offs);
                                uint16_t mask = 0x0000;
                                if (val > 0) {
                                    if (val > 15) {
                                        continue;
                                    }
                                    mask |= (1 << val);
                                }
                                conf->dmamask[idx] = mask;
                            }
                        }
                        else if (strcmp(cmd, "name") == 0) {
                            strncpy(dev->name, offs, ISA_MAX_NAME - 1);
                        }
                        else if (strcmp(cmd, "id") == 0) {
                            if (idx < ISA_MAX_DEV_IDS) {
                                dev->id[idx] = StrToId(offs);
                            }
                        }
                        else if (strcmp(cmd, "enable") == 0) {
                            if (StrToInt(offs) > 0)
                                dev->flags |= ISA_FLG_ENABLED;
                            else
                                dev->flags &= ~ISA_FLG_ENABLED;
                        }
                        else if(strcmp(cmd, "conf") == 0) {
                            int num = StrToInt(offs);
                            if ((num > 0) && (num < PNP_MAX_CONFIGS)) {
                                memcpy(&conf[0], &conf[num], sizeof(pnp_conf_t));
                            }
                        }
                    }
                }
            }
        }
    } while(offs);    
}

/*---------------------------------------------------------*/

static void pnp_initiation_key(void)
{
    int i; int32_t key = 0x6a;
    isa.bus.outp(pnp_wraddr, 0);
    isa.bus.outp(pnp_wraddr, 0);
    isa.bus.outp(pnp_wraddr, key);
    for (i=1; i<32; i++) {
        key = (key >> 1) | (((key ^ (key >> 1)) << 7) & 0xff);
        isa.bus.outp(pnp_wraddr, key);
    }
}

static void pnp_regwr(uint8_t addr, uint8_t data) {
    isa.bus.outp(pnp_wraddr, addr);
    isa.bus.outp(pnp_wrdata, data);
}

static bool pnp_waitstatus(void) {
    int i;
    isa.bus.outp(pnp_wraddr, REG_STATUS);
    for (i = 0; i<100; i++) {
        if (isa.bus.inp(pnp_rdport) & 0x1) {
            return true;
        }
        delayus(10);
    }
    return false;
}

static bool pnp_resrd(uint8_t* buf, uint32_t count) {
    int i; uint32_t result = 0;
    for (i=0; i<count; i++) {
        uint8_t data;
        if (!pnp_waitstatus()) {
            break;
        }
        isa.bus.outp(pnp_wraddr, REG_RESOURCE_DATA);
        data = isa.bus.inp(pnp_rdport);
        if (buf) {
            buf[i] = data;
        }
        result++;
    }
    return (result == count) ? true : false;
}


static bool pnp_validate_conf(pnp_conf_t* conf) {
    /* must have valid flag set */
    if (!(conf->flags & ISA_FLG_ENABLED))
        return false;
    /* must support ISA high=true edge triggered interrupts */
    if ((conf->nirq > 0) && ((conf->irqmask[0] & (1UL << 16)) == 0))
        return false;
    if ((conf->nirq > 1) && ((conf->irqmask[1] & (1UL << 16)) == 0))
        return false;
    /* must have at least some resource */
    if ((conf->nio + conf->nmem + conf->nirq + conf->ndma) == 0)
        return false;
    return true;
}

static uint16_t pnp_validate_confs(isa_card_t* card) {
    int i, j; uint16_t valid_devs = 0;
    for (i=0; i<card->numdevices; i++) {
        pnp_conf_t* devconfs = pnp_getconfs(card->devices[i].csn, card->devices[i].ldn);

        /* count valid configs */
        uint16_t num_configs = 0;
        for (j=0; j<PNP_MAX_CONFIGS; j++) {
            num_configs += (devconfs[j].flags & ISA_FLG_ENABLED) ? 1 : 0;
        }

        /* arrange config list */
        if (num_configs > 1) {
            pnp_conf_t* dst = &devconfs[0];
            memset(dst, 0, sizeof(pnp_conf_t));
            for (j=1; j<PNP_MAX_CONFIGS; j++) {
                pnp_conf_t* src = &devconfs[j];
                if (pnp_validate_conf(src)) {
                    memcpy(dst, src, sizeof(pnp_conf_t));
                    memset(src, 0, sizeof(pnp_conf_t));
                    dst++;
                }
            }
        }

        /* we need at least one config for the device to be considered valid */
        if (pnp_validate_conf(&devconfs[0])) {
            valid_devs++;
        } else {
            devconfs[0].flags &= ~ISA_FLG_ENABLED;
        }
    }
    return valid_devs;
}

static bool pnp_register_card(uint8_t csn, uint32_t card_vendor, uint32_t card_serial, uint8_t* resbuf, uint32_t reslen) {
    isa_device_t* dev = 0;
    isa_card_t* card = 0;
    pnp_conf_t* confs = 0;
    int16_t cfgnum = 0;
    uint8_t ldn = 0;
    int32_t len = 0;
    bool fail = false;

    if (isa.numcards >= ISA_MAX_CARDS)
        return false;

    card = &isa.cards[isa.numcards];

    memset(card, 0, sizeof(isa_card_t));
    sprintf(card->name, "%s:%08lx", IdToStr(card_vendor), card_serial);
    card->flags = ISA_FLG_ENABLED | ISA_FLG_PNP;
    card->vendor = card_vendor;
    card->serial = card_serial;
    card->csn = csn;

    confs = pnp_getconfs(csn, 0);
    len = (int32_t) reslen;
    while (!fail && (len > 0)) {
        uint8_t res_tag = resbuf[0];
        uint8_t* res_data = resbuf;
        uint16_t res_len = 0;
        uint16_t pkg_len = 0;

        if ((res_tag & 0x80) == 0) {
            res_len = (res_tag & 0x07);
            res_tag = ((res_tag >> 3) & 0x0f);
            res_data = &resbuf[1];
        } else if (len > 2) {
            res_len = (resbuf[1] + (resbuf[2] << 8));
            res_data = &resbuf[3];
        }

        pkg_len = (res_data - resbuf) + res_len;
        resbuf += pkg_len; len -= pkg_len;
        if ((pkg_len == 0) || (len < 0)) {
            fail = true;
            break;
        }

        switch (res_tag)
        {
            /*-------------------------------------------------------
             * version
             *-----------------------------------------------------*/
            case 0x01: 
            {
                card->pnp_version = res_data[0];
                card->card_version = res_data[1];
            } break;

            /*-------------------------------------------------------
             * logical device
             *-----------------------------------------------------*/
            case 0x02:
            {
                if (card->numdevices < ISA_MAX_CARD_DEVS) {
                    dev = &card->devices[card->numdevices];
                    dev->id[0] = swap32(*((uint32_t*)res_data));
                    dev->csn = card->csn;
                    dev->ldn = ldn;
                    dev->flags = card->flags;
                    sprintf(dev->name, "%s", IdToStr(dev->id[0]));
                    card->numdevices++;
                    cfgnum = 0;
                    confs = pnp_getconfs(csn, ldn);
                    confs[cfgnum].flags = card->flags;
                    ldn++;
                } else {
                    fail = true;
                }
            } break;

            /*-------------------------------------------------------
             * compatible device
             *-----------------------------------------------------*/
            case 0x03:
            {
                int i;
                for (i=1; i<ISA_MAX_DEV_IDS; i++) {
                    if (dev->id[i] == 0) {
                        dev->id[i] = swap32(*((uint32_t*)res_data));
                        break;
                    }
                }
            } break;

            /*-------------------------------------------------------
             * irq format
             *-----------------------------------------------------*/
            case 0x04:
            {
                uint32_t irqmask = swap16(*((uint16_t*)res_data));
                uint32_t irqflag = (res_len > 2) ? res_data[2] : 0x01; /* default: high true edge sensitive */
                if (confs[cfgnum].nirq < ISA_MAX_DEV_IRQ) {
                    #ifdef DEBUG
                    dbg_printf("irq fmt: %d %d %02x %04x\r\n", cfgnum, confs[cfgnum].nirq, irqflag, irqmask);
                    #endif
                    confs[cfgnum].irqmask[confs[cfgnum].nirq] = ((irqflag << 16) | irqmask);
                    confs[cfgnum].nirq++;
                }
            } break;

            /*-------------------------------------------------------
             * dma format
             *-----------------------------------------------------*/
            case 0x05:
            {
                uint16_t dmamask = res_data[0] + (res_data[1] << 8);
                if (confs[cfgnum].ndma < ISA_MAX_DEV_DMA) {
                    #ifdef DEBUG
                    dbg_printf("dma fmt: %d %d %04x\r\n", cfgnum, confs[cfgnum].ndma, dmamask);
                    #endif
                    confs[cfgnum].dmamask[confs[cfgnum].ndma] = dmamask;
                    confs[cfgnum].ndma++;
                }
            } break;

            /*-------------------------------------------------------
             * start dependant
             *-----------------------------------------------------*/
            case 0x06:
            {
                cfgnum++;
                memcpy((void*)&confs[cfgnum], (void*)&confs[0], sizeof(pnp_conf_t));
                if (res_len > 0) {
                    confs[cfgnum].flags &= 0xff00;
                    confs[cfgnum].flags |= res_data[0];
                }
            } break;

            /*-------------------------------------------------------
             * end dependant
             *-----------------------------------------------------*/
            case 0x07:
            {
                cfgnum = 0;
            } break;

            /*-------------------------------------------------------
             * io range
             *-----------------------------------------------------*/
            case 0x08:
            {
                if (confs[cfgnum].nio < ISA_MAX_DEV_PORT) {
                    pnp_desc_range_t* range = &confs[cfgnum].iorange[confs[cfgnum].nio];
                    range->base_min = swap16(*((uint16_t*)&res_data[1]));
                    range->base_max = swap16(*((uint16_t*)&res_data[3]));
                    range->align    = res_data[5];
                    range->length   = res_data[6];
                    #ifdef DEBUG
                    dbg_printf("io-range: %d %d %08lx-%08lx : %04x\r\n", cfgnum, confs[cfgnum].nio, range->base_min, range->base_max, range->length );
                    #endif
                    confs[cfgnum].nio++;
                }
            } break;

            /*-------------------------------------------------------
             * io fixed
             *-----------------------------------------------------*/
            case 0x09:
            {
                if (confs[cfgnum].nio < ISA_MAX_DEV_PORT) {
                    pnp_desc_range_t* range = &confs[cfgnum].iorange[confs[cfgnum].nio];
                    range->base_min = swap16(*((uint16_t*)&res_data[0]));
                    range->base_max = range->base_min;
                    range->align    = 1;
                    range->length   = res_data[2];
                    #ifdef DEBUG
                    dbg_printf("io-fixed: %d %d %08lx-%08lx : %04x\r\n", cfgnum, confs[cfgnum].nio, range->base_min, range->base_max, range->length );
                    #endif
                    confs[cfgnum].nio++;
                }
            } break;

            /*-------------------------------------------------------
             * vendor
             *-----------------------------------------------------*/
            case 0x0e:
            {
                /* vendor specific data */
            } break;

            /*-------------------------------------------------------
             * end
             *-----------------------------------------------------*/
            case 0x0f:
            {
                len = 0;
            } break;


            /*-------------------------------------------------------
             * mem range
             *-----------------------------------------------------*/
            case 0x81:
            {
                if (confs[cfgnum].nmem < ISA_MAX_DEV_MEM) {
                    pnp_desc_range_t* range = &confs[cfgnum].memrange[confs[cfgnum].nmem];
                    range->flags    = res_data[0];
                    range->base_min = swap16(*((uint16_t*)&res_data[1])) << 8;
                    range->base_max = swap16(*((uint16_t*)&res_data[3])) << 8;
                    range->align    = swap16(*((uint16_t*)&res_data[5])) << 8;
                    range->length   = swap16(*((uint16_t*)&res_data[7])) << 8;
                    #ifdef DEBUG
                    dbg_printf("mem-range: %d %d %08lx-%08lx : %04x\r\n", cfgnum, confs[cfgnum].nmem, range->base_min, range->base_max, range->length );
                    #endif
                    confs[cfgnum].nmem++;
                }
            } break;

            /*-------------------------------------------------------
             * ansi identifier
             *-----------------------------------------------------*/
            case 0x82:
            {
                char* namebuf = dev ? dev->name : card->name;
                if (res_len >= ISA_MAX_NAME) {
                    res_len = ISA_MAX_NAME - 1;
                }
                memcpy(namebuf, res_data, res_len);
                while (namebuf[res_len-1] == ' ') {
                    res_len--;
                }
                namebuf[res_len] = 0;
            } break;

            /*-------------------------------------------------------
             * unicode identifier
             *-----------------------------------------------------*/
            case 0x83:
            {
                /* unused */
            } break;

            /*-------------------------------------------------------
             * vendor
             *-----------------------------------------------------*/
            case 0x84:
            {
                /* vendor specific data */
            } break;

            /*-------------------------------------------------------
             * mem32-range
             *-----------------------------------------------------*/
            case 0x85:
            {
                if (confs[cfgnum].nmem < ISA_MAX_DEV_MEM) {
                    pnp_desc_range_t* range = &confs[cfgnum].memrange[confs[cfgnum].nmem];
                    range->flags    = res_data[0];
                    range->base_min = swap32(*((uint16_t*)&res_data[1]));
                    range->base_max = swap32(*((uint16_t*)&res_data[5]));
                    range->align    = swap32(*((uint16_t*)&res_data[9]));
                    range->length   = swap32(*((uint16_t*)&res_data[13]));
                    #ifdef DEBUG
                    dbg_printf("mem32-range: %d %d %08lx-%08lx : %04x\r\n", cfgnum, confs[cfgnum].nmem, range->base_min, range->base_max, range->length );
                    #endif
                    confs[cfgnum].nmem++;
                }
            } break;

            /*-------------------------------------------------------
             * mem32-fixed
             *-----------------------------------------------------*/
            case 0x86:
            {
                if (confs[cfgnum].nmem < ISA_MAX_DEV_MEM) {
                    pnp_desc_range_t* range = &confs[cfgnum].memrange[confs[cfgnum].nmem];
                    range->flags    = res_data[0];
                    range->base_min = swap32(*((uint16_t*)&res_data[1]));
                    range->base_max = range->base_min;
                    range->align    = 1;
                    range->length   = swap32(*((uint16_t*)&res_data[5]));
                    #ifdef DEBUG
                    dbg_printf("mem32-fixed: %d %d %08lx-%08lx : %04x\r\n", cfgnum, confs[cfgnum].nmem, range->base_min, range->base_max, range->length );
                    #endif
                    confs[cfgnum].nmem++;
                }
            } break;

            /*-------------------------------------------------------
             * default : ignore
             *-----------------------------------------------------*/
            default:
            {
            } break;
        }
    }

    /* clean up configs list */
    if (!fail) {
        fail = (pnp_validate_confs(card) == 0) ? true : false;
    }        
    return fail ? false : true;
}

static int pnp_detect(void)
{
    int csn, i;
    
    /* temporary resource buffer */
    uint32_t memsize_res = (16 * 1024UL);
    uint8_t* resbuf = (uint8_t*) Malloc(memsize_res);
    if (!resbuf) {
        return 0;
    }
    memset(resbuf, 0, memsize_res);

    pnp_initiation_key();
    pnp_regwr(REG_CONFIG_CONTROL, CONFIG_CONTROL_RESET_CSN);
    pnp_regwr(REG_WAKE_CSN, 0);
    pnp_regwr(REG_SET_RDPORT, pnp_rdport >> 2);
    for (csn = (isa.numcards+1); csn <= ISA_MAX_CARDS; csn++ ) {
        uint32_t card_vendor = 0; uint32_t card_serial = 0;
        isa.bus.outp(pnp_wraddr, REG_SERIAL_ISOLATION);
        delayus(1000);

        /* get vendor and device id */
        {
            int i;
            bool id_valid = false;
            uint8_t data[9];
            int32_t crc = 0x6a;

            memset(data, 0, sizeof(data));
            isa.bus.outp(pnp_wraddr, REG_SERIAL_ISOLATION);
            delayus(1000);

            for (i=0; i<72; i++) {
                int bit = (isa.bus.inp(pnp_rdport) == 0x55) ? 1 : 0;
                delayus(250);
                bit = ((isa.bus.inp(pnp_rdport) == 0xAA) && bit) ? 1 : 0;
                delayus(250);
                id_valid = id_valid || bit;
                if (i < 64) {
                    crc = (crc >> 1) | (((crc ^ (crc >> 1) ^ bit) << 7) & 0xff);
                }
                data[i / 8] = (data[i / 8] >> 1) | (bit ? 0x80 : 0);
            }

            id_valid = id_valid && (data[8] == crc);
            if (!id_valid) {
                break;
            } else {
                uint32_t* d = (uint32_t*)data;
                card_vendor = swap32(d[0]); 
                card_serial = swap32(d[1]);
            }
        }

        /* read card resources */
        {
            bool done = false; bool ok = true; uint32_t size = 0;
            pnp_regwr(REG_SELECT_CSN, csn);
            while (!done && ok) {
                ok = pnp_resrd(&resbuf[size], 1); size++;
                if (ok) {
                    uint8_t res_tag = resbuf[size-1];
                    if ((res_tag & 0x80) == 0) {
                        /* small resource */
                        uint8_t res_len = (res_tag & 0x07);
                        uint8_t res_num = ((res_tag >> 3) & 0xf);
                        ok = pnp_resrd(&resbuf[size], res_len);
                        size += res_len;
                        if (ok && (res_num == 0xf)) {
                            done = true;
                        }
                    }
                    else {
                        /* large resource */
                        /*uint8_t res_num = (res_tag & 0x7f);*/
                        ok = pnp_resrd(&resbuf[size], 2);
                        size += 2;
                        if (ok) {
                            uint16_t res_len = resbuf[size-2] + (resbuf[size-1] << 8);
                            ok = pnp_resrd(&resbuf[size], res_len);
                            size += res_len;
                        }
                    }
                }
            }

            /* create device */
            if (ok && pnp_register_card(csn, card_vendor, card_serial, resbuf, size)) {
                isa.numcards++;
            }

            pnp_regwr(REG_WAKE_CSN, 0);
        }
    }

    pnp_regwr(REG_CONFIG_CONTROL, CONFIG_CONTROL_WAIT_KEY);
    Mfree(resbuf);
    return isa.numcards;
}

bool pnp_apply(isa_device_t* dev, pnp_conf_t* conf) {
    int i; bool activate = false;
    if ((dev->flags & ISA_FLG_PNP) == 0)
        return true;

    pnp_initiation_key();
    pnp_regwr(REG_WAKE_CSN, dev->csn);
    pnp_regwr(REG_SELECT_LDN, dev->ldn);

    activate = ((dev->flags & ISA_FLG_ENABLED) && (conf->flags & ISA_FLG_ENABLED)) ? true : false;

    for (i=0; i<ISA_MAX_DEV_MEM; i++) {
        if (activate && (i < conf->nmem)) {
            pnp_regwr(REG_MEMBASE_HI(i), (conf->memrange[i].base_min >> 16) & 0xff);
            pnp_regwr(REG_MEMBASE_LO(i), (conf->memrange[i].base_min >>  8) & 0xff);
            pnp_regwr(REG_MEMRANGE_HI(i), (conf->memrange[i].length >> 16) & 0xff);
            pnp_regwr(REG_MEMRANGE_LO(i), (conf->memrange[i].length >>  8) & 0xff);
        } else {
            pnp_regwr(REG_MEMBASE_HI(i), 0);
            pnp_regwr(REG_MEMBASE_LO(i), 0);
            pnp_regwr(REG_MEMRANGE_HI(i), 0);
            pnp_regwr(REG_MEMRANGE_LO(i), 0);
        }
    }

    for (i=0; i<ISA_MAX_DEV_PORT; i++) {
        if (activate && (i < conf->nio) && (conf->iorange[i].length)) {
            pnp_regwr(REG_IOBASE_HI(i), (conf->iorange[i].base_min >> 8) & 0xff);
            pnp_regwr(REG_IOBASE_LO(i), (conf->iorange[i].base_min >> 0) & 0xff);
        } else {
            pnp_regwr(REG_IOBASE_HI(i), 0);
            pnp_regwr(REG_IOBASE_LO(i), 0);
        }
    }

    for (i=0; i<ISA_MAX_DEV_IRQ; i++) {
        if (activate && (i < conf->nirq) && (conf->irqmask[i] != 0)) {
            pnp_regwr(REG_IRQLEVEL(i), bitmask_to_int(conf->irqmask[i]));
            pnp_regwr(REG_IRQTYPE(i), 2);   /* edge triggered, high=true */
        } else {
            pnp_regwr(REG_IRQLEVEL(i), 0);
            pnp_regwr(REG_IRQTYPE(i), 2);   /* edge triggered, high=true */
        }
    }

    for (i=0; i<ISA_MAX_DEV_DMA; i++) {
        if (activate && (i < conf->ndma) && (conf->dmamask[i] != 0)) {
            pnp_regwr(REG_DMACHN(i), bitmask_to_int(conf->dmamask[i]));
        } else {
            pnp_regwr(REG_DMACHN(i), 4);
        }
    }

    pnp_regwr(REG_ACTIVATE, activate ? 1 : 0);
    pnp_regwr(REG_CONFIG_CONTROL, CONFIG_CONTROL_WAIT_KEY);
    return activate;
}

void pnp_activate(void) {
    int icard, idev, i;
    for (icard = 0; icard < isa.numcards; icard++) {
        bool has_displayed_card_info = false;
        isa_card_t* card = &isa.cards[icard];
        for (idev = 0; idev < card->numdevices; idev++) {
            isa_device_t* dev = &card->devices[idev];
            pnp_conf_t* devconfs = pnp_getconfs(dev->csn, dev->ldn);
            if (devconfs) {
                /* now set the config */
                /* todo: should be a configured conf */
                pnp_conf_t* conf = &devconfs[0];
                if (!(card->flags & ISA_FLG_ENABLED)) { conf->flags &= ~ISA_FLG_ENABLED; }
                if (!(dev->flags  & ISA_FLG_ENABLED)) { conf->flags &= ~ISA_FLG_ENABLED; }
                if ((card->flags & ISA_FLG_PNP) && (!pnp_apply(dev, conf))) {
                    continue;
                }

                if (conf->flags & ISA_FLG_ENABLED)
                {
                    /* create public device info */
                    isa_dev_t* isadev = &isa.bus.devs[isa.bus.numdevs];
                    memset(isadev, 0, sizeof(isa_dev_t));
                    isadev->id[0] = dev->id[0];
                    for (i=1; i<ISA_MAX_DEV_IDS && dev->id[i]; i++) { isadev->id[i] = dev->id[i]; }
                    for (i=0; i<conf->nio; i++)  { isadev->port[i] = conf->iorange[i].base_min; }
                    for (i=0; i<conf->nmem; i++) { isadev->mem[i] = conf->memrange[i].base_min; }
                    for (i=0; i<conf->nirq; i++) { isadev->irq[i] = bitmask_to_int(conf->irqmask[i]); }
                    for (i=0; i<conf->ndma; i++) { isadev->dma[i] = bitmask_to_int(conf->dmamask[i]); }
                    isa.bus.numdevs++;

                    /* and show some info */
                    if (!has_displayed_card_info) {
                        has_displayed_card_info = true;
                        printf("\r\n%s\r\n", card->name);
                    }
                    printf(" [%01x:%01x] %s\r\n", dev->csn, dev->ldn, dev->name);
                }
            }
        }
    }
}

void pnp_configure(void) {
}

void pnp_log_devices(void) {
    int icard, idev, i, j;
    for (icard = 0; icard<isa.numcards; icard++) {
        isa_card_t* card = &isa.cards[icard];
        Log("\r\nCARD%d : %s : %s\r\n", icard, IdToStr(card->vendor), card->name);
        for (idev = 0; idev < card->numdevices; idev++) {
            isa_device_t* dev = &card->devices[idev];
            pnp_conf_t* confs = pnp_getconfs(dev->csn, dev->ldn);
            if (!confs) {
                continue;
            }

            Log(" DEV%d [%02x:%02x] : %s : %s\r\n", idev, dev->csn, dev->ldn, IdToStr(dev->id[0]), dev->name);
            for (i=1; i<ISA_MAX_DEV_IDS; i++) {
                if (dev->id[i]) {
                    Log("  COMP%d: %s\r\n", i-1, IdToStr(dev->id[i]));
                }
            }

            for (i=0; i<PNP_MAX_CONFIGS; i++) {
                pnp_conf_t* conf = &confs[i];
                uint32_t resources = conf->nio + conf->nmem + conf->nirq + conf->ndma;
                if (resources && (conf->flags & ISA_FLG_ENABLED)) {
                    Log("  CONF%d:\r\n", i);
                    for (j=0; j<conf->nio; j++)  { Log("    IO%d: %08lx-%08lx : %08lx\r\n", j, conf->iorange[j].base_min, conf->iorange[j].base_max, conf->iorange[j].length); }
                    for (j=0; j<conf->nmem; j++) { Log("   MEM%d: %08lx-%08lx : %08lx\r\n", j, conf->memrange[j].base_min, conf->memrange[j].base_max, conf->memrange[j].length); }
                    for (j=0; j<conf->nirq; j++) { Log("   IRQ%d: %08lx\r\n", j, conf->irqmask[j]); }
                    for (j=0; j<conf->ndma; j++) { Log("   DMA%d: %08lx\r\n", j, conf->dmamask[j]); }
                }
            }
        }
    }
}


int32_t pnp_init(void) {
    int i,j;
    uint32_t cfgint, memsize_cfg;
    int found  = 0;
    bool pnp_probe = true;
    bool pnp_disable = false;

    Log("\r\n");
    Log("--------------------------------------------------------------------------\r\n");
    Log("Resources\r\n");
    Log("--------------------------------------------------------------------------\r\n");

    /* allocate temporary autoconf memory */
    memsize_cfg = ISA_MAX_DEVS * PNP_MAX_CONFIGS * sizeof(pnp_conf_t);
    pnpconfs = (pnp_conf_t*) Malloc(memsize_cfg);
    if (!pnpconfs) {
        return 0;
    }
    memset((void*)pnpconfs, 0, memsize_cfg);

    /* settings */
    pnp_disable = (GetInfHex("isa.pnp.enable", &cfgint) && (cfgint == 0)) ? true : false;
    if (!pnp_disable) {
        pnp_rdport = (GetInfHex("isa.pnp.port", &cfgint) && (cfgint != 0)) ? cfgint : 0x203;
        pnp_probe = true;
    } else {
        pnp_rdport = 0;
        pnp_probe = false;
    }

    /* manually created cards */
    pnp_inf_create_cards();

    /* autodetect non-pnp cards */
    if (pnp_probe) {
        isa_probe();
    }

    /* detect pnp cards */
    if (pnp_rdport) {
        found = pnp_detect();
        pnp_log_devices();
    }

    /* apply manual cards settings */
    pnp_inf_modify_cards();

    /* apply manual device settings */
    pnp_inf_modify_devices();

    /* configure devices */
    pnp_configure();

    /* activate */
    if (pnp_rdport) {
        pnp_activate();
    }

    Log("\r\n");
    Log("--------------------------------------------------------------------------\r\n");
    Log("Configured\r\n");
    Log("--------------------------------------------------------------------------\r\n");
    for (i=0; i<isa.bus.numdevs; i++) {
        isa_dev_t* isadev = &isa.bus.devs[i];
        Log("%d - ID:  ", i); for (j=0; j<ISA_MAX_DEV_IDS && isadev->id[j]; j++) { Log ("%s ", IdToStr(isadev->id[j])); } Log("\r\n");
        if (isadev->port[0]) { Log("    IO:  "); for (j=0; j<ISA_MAX_DEV_PORT && isadev->port[j]; j++) { Log("%04x ", isadev->port[j]); } Log("\r\n"); }
        if (isadev->mem[0])  { Log("    MEM: "); for (j=0; j<ISA_MAX_DEV_MEM && isadev->mem[j];  j++)  { Log("%08lx ", isadev->mem[j]);  } Log("\r\n"); }
        if (isadev->irq[0])  { Log("    IRQ: "); for (j=0; j<ISA_MAX_DEV_IRQ && isadev->irq[j];  j++)  { Log("%d ",   isadev->irq[j]);  } Log("\r\n"); }
        if (isadev->dma[0])  { Log("    DMA: "); for (j=0; j<ISA_MAX_DEV_DMA && isadev->dma[j];  j++)  { Log("%d ",   isadev->dma[j]);  } Log("\r\n"); }
        Log("\r\n");
    }

    /* release temp memory and done */
    Mfree(pnpconfs);
    return found;
}

