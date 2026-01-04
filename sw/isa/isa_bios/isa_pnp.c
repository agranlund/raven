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

#include "isa_bios.h"
#include <stdio.h>
#include <string.h>

#define PNP_TEMP_RESBUF_SIZE (16 * 1024UL)


/*---------------------------------------------------------*/
#define PNP_REG_SET_RDPORT              0x00
#define PNP_REG_SERIAL_ISOLATION        0x01
#define PNP_REG_CONFIG_CONTROL          0x02
    #define PNP_CONFIG_CONTROL_RESET        0x01
    #define PNP_CONFIG_CONTROL_WAIT_KEY     0x02
    #define PNP_CONFIG_CONTROL_RESET_CSN    0x04
#define PNP_REG_WAKE_CSN                0x03
#define PNP_REG_RESOURCE_DATA           0x04
#define PNP_REG_STATUS                  0x05
#define PNP_REG_SELECT_CSN              0x06
#define PNP_REG_SELECT_LDN              0x07
#define PNP_REG_CARD_RESERVED           0x08
#define PNP_REG_VENDOR_DEFINED          0x20
#define PNP_REG_ACTIVATE                0x30
#define PNP_REG_RANGE_CHECK             0x31
#define PNP_REG_MEMBASE_HI(x)           (0x40 + 8*(x))
#define PNP_REG_MEMBASE_LO(x)           (0x41 + 8*(x))
#define PNP_REG_MEMCONTROL(x)           (0x42 + 8*(x))
#define PNP_REG_MEMRANGE_HI(x)          (0x43 + 8*(x))
#define PNP_REG_MEMRANGE_LO(x)          (0x44 + 8*(x))
#define PNP_REG_IOBASE_HI(x)            (0x60 + 2*(x))
#define PNP_REG_IOBASE_LO(x)            (0x61 + 2*(x))
#define PNP_REG_IRQLEVEL(x)             (0x70 + 2*(x))
#define PNP_REG_IRQTYPE(x)              (0x71 + 2*(x))
#define PNP_REG_DMACHN(x)               (0x74 + 1*(x))

#define PNP_REG_MEMBASE32_24(x)         (0x76 + 10*(x))
#define PNP_REG_MEMBASE32_16(x)         (0x77 + 10*(x))
#define PNP_REG_MEMBASE32_08(x)         (0x78 + 10*(x))
#define PNP_REG_MEMBASE32_00(x)         (0x79 + 10*(x))
#define PNP_REG_MEMCONTROL32(x)         (0x7A + 10*(x))
#define PNP_REG_MEMRANGE32_24(x)        (0x7B + 10*(x))
#define PNP_REG_MEMRANGE32_16(x)        (0x7C + 10*(x))
#define PNP_REG_MEMRANGE32_08(x)        (0x7D + 10*(x))
#define PNP_REG_MEMRANGE32_00(x)        (0x7E + 10*(x))


/*-----------------------------------------------------------------------------------
 * eisa id helpers
 *---------------------------------------------------------------------------------*/
 const char* pnp_id_to_string(uint32_t id) {
    static char str[8];
    static const char hextable[16] = "0123456789ABCDEF";
    str[0] = '@' + ((id >> 26) & 0x1f);
    str[1] = '@' + ((id >> 21) & 0x1f);
    str[2] = '@' + ((id >> 16) & 0x1f);
    str[3] = hextable[((id >> 12) & 0xf)];
    str[4] = hextable[((id >>  8) & 0xf)];
    str[5] = hextable[((id >>  4) & 0xf)];
    str[6] = hextable[((id >>  0) & 0xf)];
    str[7] = 0;
    return (const char*) str;
}

uint32_t pnp_string_to_id(const char* str) {
    uint32_t id = 0;
    if (str) {
        uint32_t bt;
        bt = (uint32_t) (str[0] - '@'); id |= (bt << 26);
        bt = (uint32_t) (str[1] - '@'); id |= (bt << 21);
        bt = (uint32_t) (str[2] - '@'); id |= (bt << 16);
        bt = (uint32_t) ((str[3] >= 'a') ? (10 + str[3] - 'a') : ((str[3] >= 'A') ? (10 + str[3] - 'A') : (str[3] - '0'))); id |= (bt << 12);
        bt = (uint32_t) ((str[4] >= 'a') ? (10 + str[4] - 'a') : ((str[4] >= 'A') ? (10 + str[4] - 'A') : (str[4] - '0'))); id |= (bt <<  8);
        bt = (uint32_t) ((str[5] >= 'a') ? (10 + str[5] - 'a') : ((str[5] >= 'A') ? (10 + str[5] - 'A') : (str[5] - '0'))); id |= (bt <<  4);
        bt = (uint32_t) ((str[6] >= 'a') ? (10 + str[6] - 'a') : ((str[6] >= 'A') ? (10 + str[6] - 'A') : (str[6] - '0'))); id |= (bt <<  0);
        return id;
    }
    return id;
}


/*-----------------------------------------------------------------------------------
 * device access and configuration
 *---------------------------------------------------------------------------------*/

pnp_card_t* pnp_get_card(uint32_t id) {
    pnp_card_t* card = isa.pnp.cards;
    if (id == 0) {
        return card;
    } else {
        while (card) {
            if (card->id == id) {
                return card;
            }
            card = (pnp_card_t*) card->next;
        }
    }
    return card;
}

pnp_device_t* pnp_card_get_device(pnp_card_t* card, uint32_t id) {
    pnp_device_t* dev = card->devices;
    if (id == 0) {
        return dev;
    } else {
        while (dev) {
            int i;
            for (i=0; (i<PNP_MAX_IDS) && (dev->ids[i] != 0); i++) {
                if (dev->ids[i] == id) {
                    return dev;
                }
            }
            dev = (pnp_device_t*) dev->next;
        }
    }
    return dev;
}


pnp_card_t* pnp_device_get_card(pnp_device_t* dev) {
    return (pnp_card_t*) dev->parent;
}

pnp_conf_t* pnp_device_get_conf(pnp_device_t* dev, uint8_t id) {
    pnp_conf_t* conf = dev->confs;
    while (conf) {
        if (conf->id == id) {
            return conf;
        }
        conf = (pnp_conf_t*) conf->next;
    }
    return dev->confs; /* fallback to first config */
}

pnp_setting_t* pnp_device_get_settings(pnp_device_t* dev) {
    return dev->settings;
}

/*-----------------------------------------------------------------------------------
 *
 * lowlevel
 * 
 *---------------------------------------------------------------------------------*/

#if defined(__GNUC__)
static void nop(void) { __asm__ volatile ( "nop\n\t" : : : ); }
#else
static void nop(void) 0x4E71;
#endif

static void delayus(uint32_t us) {
    static uint32_t calib = 0;
    if (calib == 0) {
        uint32_t tick_start = *((volatile uint32_t*)0x4ba);
        uint32_t tick_end = tick_start;
        do {
            nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
            nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
            tick_end = *((volatile uint32_t*)0x4ba);
            calib++;
        } while (((tick_end - tick_start) <= 50) && (calib < 1000000UL));
    } else {
        while (us) {
            uint32_t loops = (us > 1000) ? 1000 : us; us -= loops;
            loops = 1 + ((4 * calib * loops) / (1000 * 1000UL));
            for (; loops; loops--) {
                nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
                nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
            }
        }
    }
}

/*-----------------------------------------------------------------------------------
 * write pnp initiation key
 *---------------------------------------------------------------------------------*/
void pnp_write_key(void)
{
    int i; int32_t key = 0x6a;
    isa.bus.outp(isa.pnp.adport, 0);
    isa.bus.outp(isa.pnp.adport, 0);
    isa.bus.outp(isa.pnp.adport, key);
    for (i=1; i<32; i++) {
        key = (key >> 1) | (((key ^ (key >> 1)) << 7) & 0xff);
        isa.bus.outp(isa.pnp.adport, key);
    }
}

/*-----------------------------------------------------------------------------------
 * write pnp register
 *---------------------------------------------------------------------------------*/
void pnp_write_reg(uint8_t addr, uint8_t data) {
    isa.bus.outp(isa.pnp.adport, addr);
    isa.bus.outp(isa.pnp.wrport, data);
}

/*-----------------------------------------------------------------------------------
 * read pnp register
 *---------------------------------------------------------------------------------*/
uint8_t pnp_read_reg(uint8_t addr) {
    isa.bus.outp(isa.pnp.adport, addr);
    return isa.bus.inp(isa.pnp.rdport);
}

/*-----------------------------------------------------------------------------------
 * read pnp resource
 *---------------------------------------------------------------------------------*/
bool pnp_read_res(uint8_t* buf, uint32_t count) {
    int i,j; uint32_t result = 0;
    for (i=0; i<count; i++) {
        bool ready = false;
        uint8_t data = 0;

        /* wait for ready status */
        isa.bus.outp(isa.pnp.adport, PNP_REG_STATUS);
        for (j=0; (j<100) && !ready; j++) {
            ready = (isa.bus.inp(isa.pnp.rdport) & 0x01) ? true : false;
            delayus(100);
        }

        /* something went wrong */
        if (!ready) {
            break;
        }

        /* get data */
        isa.bus.outp(isa.pnp.adport, PNP_REG_RESOURCE_DATA);
        data = isa.bus.inp(isa.pnp.rdport);
        if (buf) {
            buf[i] = data;
        }
        result++;
    }
    return (result == count) ? true : false;
}

/*-----------------------------------------------------------------------------------
 * configuration
 *---------------------------------------------------------------------------------*/

 void pnp_enter_config_state(pnp_device_t* dev) {
    pnp_write_reg(PNP_REG_CONFIG_CONTROL, PNP_CONFIG_CONTROL_WAIT_KEY);                
    pnp_write_key();
    pnp_write_reg(PNP_REG_WAKE_CSN, dev->csn);
    pnp_write_reg(PNP_REG_SELECT_LDN, dev->ldn);
}

void pnp_leave_config_state(void) {
    pnp_write_reg(PNP_REG_CONFIG_CONTROL, PNP_CONFIG_CONTROL_WAIT_KEY);                
}

/*-----------------------------------------------------------------------------------
 * activate card
 *---------------------------------------------------------------------------------*/

bool pnp_device_configure(pnp_device_t* dev) {
    int i; bool activate;
    pnp_setting_t* setting;
    pnp_conf_t* conf;
    pnp_card_t* card;

    /* deactivated until proven otherwise */
    dev->flags &= ~ISA_FLG_ACTIVE;

    /* skip non-pnp devices */
    if (dev->csn == 0) {
        dev->flags |= ISA_FLG_ACTIVE;
        return true;
    }

    /* start configure */
    setting = pnp_device_get_settings(dev);
    conf = pnp_device_get_conf(dev, setting->conf);
    card = pnp_device_get_card(dev);
    activate = ((card->flags | dev->flags | conf->flags) & (ISA_FLG_DISABLED | ISA_FLG_INVALID)) ? false : true;
    pnp_enter_config_state(dev);

    /* memory resources */
    for (i=0; i<ISA_MAX_DEV_MEM; i++) {
        if (conf->memrange[i].flags & (1 << 7)) {
            /* mem32 */
            if (activate && (i < conf->nmem)) {
                uint32_t lo = setting->membase[i];
                uint32_t hi = conf->memrange[i].length;
                if (pnp_read_reg(PNP_REG_MEMCONTROL32(i)) & 1) { hi += lo; }
                pnp_write_reg(PNP_REG_MEMBASE32_24(i), (lo >> 24) & 0xff);
                pnp_write_reg(PNP_REG_MEMBASE32_16(i), (lo >> 16) & 0xff);
                pnp_write_reg(PNP_REG_MEMBASE32_08(i), (lo >>  8) & 0xff);
                pnp_write_reg(PNP_REG_MEMBASE32_00(i), (lo >>  0) & 0xff);
                pnp_write_reg(PNP_REG_MEMRANGE32_24(i), (hi >> 24) & 0xff);
                pnp_write_reg(PNP_REG_MEMRANGE32_16(i), (hi >> 16) & 0xff);
                pnp_write_reg(PNP_REG_MEMRANGE32_08(i), (hi >>  8) & 0xff);
                pnp_write_reg(PNP_REG_MEMRANGE32_00(i), (hi >>  0) & 0xff);
            } else {
                pnp_write_reg(PNP_REG_MEMBASE32_24(i), 0);
                pnp_write_reg(PNP_REG_MEMBASE32_16(i), 0);
                pnp_write_reg(PNP_REG_MEMBASE32_08(i), 0);
                pnp_write_reg(PNP_REG_MEMBASE32_00(i), 0);
                pnp_write_reg(PNP_REG_MEMRANGE32_24(i), 0);
                pnp_write_reg(PNP_REG_MEMRANGE32_16(i), 0);
                pnp_write_reg(PNP_REG_MEMRANGE32_08(i), 0);
                pnp_write_reg(PNP_REG_MEMRANGE32_00(i), 0);
            }
        } else {
            /* mem24 */
            if (activate && (i < conf->nmem)) {
                uint32_t lo = setting->membase[i];
                uint32_t hi = conf->memrange[i].length;
                if (pnp_read_reg(PNP_REG_MEMCONTROL(i)) & 1) { hi += lo; }
                pnp_write_reg(PNP_REG_MEMBASE_HI(i), (lo >> 16) & 0xff);
                pnp_write_reg(PNP_REG_MEMBASE_LO(i), (lo >>  8) & 0xff);
                pnp_write_reg(PNP_REG_MEMRANGE_HI(i), (hi >> 16) & 0xff);
                pnp_write_reg(PNP_REG_MEMRANGE_LO(i), (hi >>  8) & 0xff);
            } else {
                pnp_write_reg(PNP_REG_MEMBASE_HI(i), 0);
                pnp_write_reg(PNP_REG_MEMBASE_LO(i), 0);
                pnp_write_reg(PNP_REG_MEMRANGE_HI(i), 0);
                pnp_write_reg(PNP_REG_MEMRANGE_LO(i), 0);
            }
        }
    }

    /* io port resources */
    for (i=0; i<ISA_MAX_DEV_PORT; i++) {
        if (activate && (i < conf->nio) && (conf->iorange[i].length)) {
            pnp_write_reg(PNP_REG_IOBASE_HI(i), (setting->iobase[i] >> 8) & 0xff);
            pnp_write_reg(PNP_REG_IOBASE_LO(i), (setting->iobase[i] >> 0) & 0xff);
        } else {
            pnp_write_reg(PNP_REG_IOBASE_HI(i), 0);
            pnp_write_reg(PNP_REG_IOBASE_LO(i), 0);
        }
    }

    /* irq port resources */
    for (i=0; i<ISA_MAX_DEV_IRQ; i++) {
        if (activate && (i < conf->nirq) && (conf->irqmask[i] != 0)) {
            pnp_write_reg(PNP_REG_IRQLEVEL(i), setting->irq[i]);
            pnp_write_reg(PNP_REG_IRQTYPE(i), 2);   /* edge triggered, high=true */
        } else {
            pnp_write_reg(PNP_REG_IRQLEVEL(i), 0);
            pnp_write_reg(PNP_REG_IRQTYPE(i), 2);   /* edge triggered, high=true */
        }
    }

    /* dma resources */
    for (i=0; i<ISA_MAX_DEV_DMA; i++) {
        if (activate && (i < conf->ndma) && (conf->dmamask[i] != 0)) {
            pnp_write_reg(PNP_REG_DMACHN(i), setting->dma[i]);
        } else {
            pnp_write_reg(PNP_REG_DMACHN(i), 4); /* dma 4 means disabled */
        }
    }

    /* activation */
    dev->flags |= (activate ? ISA_FLG_ACTIVE : 0);
    pnp_write_reg(PNP_REG_ACTIVATE, activate ? 1 : 0);
    pnp_leave_config_state();
    return activate;
}


/*-----------------------------------------------------------------------------------
 *
 * device enumeration
 *
 *---------------------------------------------------------------------------------*/

pnp_card_t* pnp_create_card(uint32_t id) {
    pnp_card_t* card = (pnp_card_t*) isabios_mem_alloc(sizeof(pnp_card_t));
    card->id = id;
    card->name = isabios_mem_alloc(PNP_MAX_NAME);
    sprintf(card->name, "%s", pnp_id_to_string(id));

    if (isa.pnp.cards == 0) {
        isa.pnp.cards = card;
    } else {
        pnp_card_t* last = isa.pnp.cards;
        while (last) {
            if (last->next == 0) {
                last->next = card;
                break;
            }
            last = (pnp_card_t*) last->next;
        }
    }
    return card;
}

pnp_device_t* pnp_create_device(pnp_card_t* card, uint32_t id) {
    uint8_t ldn = 0;
    pnp_device_t* dev = isabios_mem_alloc(sizeof(pnp_device_t));
    dev->name = isabios_mem_alloc(PNP_MAX_NAME);
    dev->ids = isabios_mem_alloc((PNP_MAX_IDS + 1) * sizeof(uint32_t));
    dev->settings = isabios_mem_alloc(sizeof(pnp_setting_t));
    if (card->devices == 0) {
        card->devices = dev;
    } else {
        pnp_device_t* last = card->devices;
        while (last) {
            ldn++;
            if (last->next == 0) {
                last->next = dev;
                break;
            }
            last = (pnp_device_t*) last->next;
        }
    }

    /* legacy device is active by default */
    if (card->csn == 0) {
        dev->flags |= ISA_FLG_ACTIVE;
    }

    dev->parent = card;
    dev->csn = card->csn;
    dev->ldn = ldn;
    dev->ids[0] = id;
    sprintf(dev->name, "%s", pnp_id_to_string(id));
    return dev;
}

pnp_conf_t* pnp_create_conf(pnp_device_t* dev) {
    uint8_t id = 0;
    pnp_conf_t* conf = isabios_mem_alloc(sizeof(pnp_conf_t));
    conf->iorange = isabios_mem_alloc(ISA_MAX_DEV_PORT * sizeof(pnp_range_t));
    conf->memrange = isabios_mem_alloc(ISA_MAX_DEV_MEM * sizeof(pnp_range_t));
    conf->irqmask = isabios_mem_alloc(ISA_MAX_DEV_IRQ * sizeof(uint32_t));
    conf->dmamask = isabios_mem_alloc(ISA_MAX_DEV_DMA * sizeof(uint16_t));
    if (dev->confs == 0) {
        dev->confs = conf;
    } else {
        pnp_conf_t* last = dev->confs;
        while (last) {
            id++;
            if (last->next == 0) {
                last->next = conf;
                break;
            }
            last = (pnp_conf_t*) last->next;
        }
    }
    conf->id = id;
    return conf;
}

void pnp_device_init_settings(pnp_device_t* dev, uint8_t conf_id) {
    pnp_conf_t* conf = pnp_device_get_conf(dev, conf_id);
    if (conf) {
        int i, j;
        dev->settings->conf = conf->id;
        for (i=0; i<conf->nio; i++) {
            dev->settings->iobase[i] = conf->iorange[i].base_min;
        }
        for (i=0; i<conf->nmem; i++) {
            dev->settings->membase[i] = conf->memrange[i].base_min;
        }
        for (i=0; i<conf->nirq; i++) {
            for (j=0; j<16; j++) {
                if (conf->irqmask[i] & (1 << j)) {
                    dev->settings->irq[i] = j;
                    break;
                }
            }
        }
        for (i=0; i<conf->ndma; i++) {
            for (j=0; j<8; j++) {
                if (conf->dmamask[i] & (1 << j)) {
                    dev->settings->dma[i] = j;
                    break;
                }
            }
        }
    }
}

static bool parse_resource_buffer(uint8_t csn, uint32_t card_id, uint32_t card_sn, uint8_t* resbuf, uint32_t reslen) {

    bool fail = false;
    pnp_device_t* dev = 0;
    pnp_card_t* card = 0;
    pnp_conf_t* conf = 0;
    int32_t len = 0;

    card = pnp_create_card(card_id);
    sprintf(card->name, "%s:%08lx", pnp_id_to_string(card_id), card_sn);
    card->id = card_id;
    card->sn = card_sn;
    card->csn = csn;

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
                dev = pnp_create_device(card, *((uint32_t*)res_data));
                conf = pnp_create_conf(dev);
            } break;

            /*-------------------------------------------------------
             * compatible device
             *-----------------------------------------------------*/
            case 0x03:
            {
                int i;
                for (i=1; i<ISA_MAX_DEV_IDS; i++) {
                    if (dev->ids[i] == 0) {
                        dev->ids[i] = *((uint32_t*)res_data);
                        break;
                    }
                }
            } break;

            /*-------------------------------------------------------
             * irq format
             *-----------------------------------------------------*/
            case 0x04:
            {
                uint32_t irqmask = isabios_swap16(*((uint16_t*)res_data));
                uint32_t irqflag = (res_len > 2) ? res_data[2] : 0x01; /* default: high true edge sensitive */
                if (conf->nirq < ISA_MAX_DEV_IRQ) {
                    #ifdef ISA_LOG_DEBUG
                    isabios_log(ISA_LOG_DEBUG, "irq fmt: %d %02x %04x\r\n", conf->nirq, irqflag, irqmask);
                    #endif
                    conf->irqmask[conf->nirq] = ((irqflag << 16) | irqmask);
                    conf->nirq++;
                }
            } break;

            /*-------------------------------------------------------
             * dma format
             *-----------------------------------------------------*/
            case 0x05:
            {
                uint16_t dmamask = res_data[0] + (res_data[1] << 8);
                if (conf->ndma < ISA_MAX_DEV_DMA) {
                    #ifdef ISA_LOG_DEBUG
                    isabios_log(ISA_LOG_DEBUG, "dma fmt: %d %04x\r\n", conf->ndma, dmamask);
                    #endif
                    conf->dmamask[conf->ndma] = dmamask;
                    conf->ndma++;
                }
            } break;

            /*-------------------------------------------------------
             * start dependant
             *-----------------------------------------------------*/
            case 0x06:
            {
                conf = pnp_create_conf(dev);
                /* copy information from template conf0 */
                if (dev->confs->nio) {
                    conf->nio = dev->confs->nio;
                    memcpy(conf->iorange, dev->confs->iorange, dev->confs->nio * sizeof(pnp_range_t));
                }
                if (dev->confs->nmem) {
                    conf->nmem = dev->confs->nmem;
                    memcpy(conf->memrange, dev->confs->memrange, dev->confs->nmem * sizeof(pnp_range_t));
                }
                if (dev->confs->nirq) {
                    conf->nirq = dev->confs->nirq;
                    memcpy(conf->irqmask, dev->confs->irqmask, dev->confs->nirq * sizeof(uint32_t));
                }
                if (dev->confs->ndma) {
                    conf->ndma = dev->confs->ndma;
                    memcpy(conf->dmamask, dev->confs->dmamask, dev->confs->ndma * sizeof(uint16_t));
                }
                conf->flags = (dev->confs->flags & 0xff00);
                if (res_len > 0) {
                    conf->flags |= res_data[0];
                }
                /* adjust id number to compensate for template being thrown away later */
                conf->id = conf->id - 1;
            } break;

            /*-------------------------------------------------------
             * end dependant
             *-----------------------------------------------------*/
            case 0x07:
            {
                conf = dev->confs;
            } break;

            /*-------------------------------------------------------
             * io range
             *-----------------------------------------------------*/
            case 0x08:
            {
                if (conf->nio < ISA_MAX_DEV_PORT) {
                    pnp_range_t* range = &conf->iorange[conf->nio];
                    range->base_min = isabios_swap16(*((uint16_t*)&res_data[1]));
                    range->base_max = isabios_swap16(*((uint16_t*)&res_data[3]));
                    range->align    = res_data[5];
                    range->length   = res_data[6];
                    #ifdef ISA_LOG_DEBUG
                    isabios_log(ISA_LOG_DEBUG, "io-range: %d %08lx-%08lx : %04x\r\n", conf->nio, range->base_min, range->base_max, range->length);
                    #endif
                    conf->nio++;
                }
            } break;

            /*-------------------------------------------------------
             * io fixed
             *-----------------------------------------------------*/
            case 0x09:
            {
                if (conf->nio < ISA_MAX_DEV_PORT) {
                    pnp_range_t* range = &conf->iorange[conf->nio];
                    range->base_min = isabios_swap16(*((uint16_t*)&res_data[0]));
                    range->base_max = range->base_min;
                    range->align    = 1;
                    range->length   = res_data[2];
                    #ifdef ISA_LOG_DEBUG
                    isabios_log(ISA_LOG_DEBUG, "io-fixed: %d %08lx-%08lx : %04x\r\n", conf->nio, range->base_min, range->base_max, range->length );
                    #endif
                    conf->nio++;
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
                if (conf->nmem < ISA_MAX_DEV_MEM) {
                    pnp_range_t* range = &conf->memrange[conf->nmem];
                    range->flags    = res_data[0] & 0x7f; /* use bit7 to indicate 16/32 descriptor */
                    range->base_min = ((uint32_t)(isabios_swap16(*((uint16_t*)&res_data[1])))) << 8;
                    range->base_max = ((uint32_t)(isabios_swap16(*((uint16_t*)&res_data[3])))) << 8;
                    range->align    = ((uint32_t)(isabios_swap16(*((uint16_t*)&res_data[5])))) << 8;
                    range->length   = ((uint32_t)(isabios_swap16(*((uint16_t*)&res_data[7])))) << 8;
                    #ifdef ISA_LOG_DEBUG
                    isabios_log(ISA_LOG_DEBUG, "mem-range: %d %08lx-%08lx : %04x\r\n", conf->nmem, range->base_min, range->base_max, range->length );
                    #endif
                    conf->nmem++;
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
                if (conf->nmem < ISA_MAX_DEV_MEM) {
                    pnp_range_t* range = &conf->memrange[conf->nmem];
                    range->flags    = res_data[0] | (1 << 7);  /* use bit7 to indicate 16/32 descriptor */
                    range->base_min = isabios_swap32(*((uint32_t*)&res_data[1]));
                    range->base_max = isabios_swap32(*((uint32_t*)&res_data[5]));
                    range->align    = isabios_swap32(*((uint32_t*)&res_data[9]));
                    range->length   = isabios_swap32(*((uint32_t*)&res_data[13]));
                    #ifdef ISA_LOG_DEBUG
                    isabios_log(ISA_LOG_DEBUG, "mem32-range: %d %08lx-%08lx : %04x\r\n", conf->nmem, range->base_min, range->base_max, range->length );
                    #endif
                    conf->nmem++;
                }
            } break;

            /*-------------------------------------------------------
             * mem32-fixed
             *-----------------------------------------------------*/
            case 0x86:
            {
                if (conf->nmem < ISA_MAX_DEV_MEM) {
                    pnp_range_t* range = &conf->memrange[conf->nmem];
                    range->flags    = res_data[0] | (1 << 7);  /* use bit7 to indicate 16/32 descriptor */
                    range->base_min = isabios_swap32(*((uint16_t*)&res_data[1]));
                    range->base_max = range->base_min;
                    range->align    = 1;
                    range->length   = isabios_swap32(*((uint16_t*)&res_data[5]));
                    #ifdef ISA_LOG_DEBUG
                    isabios_log(ISA_LOG_DEBUG, "mem32-fixed: %d %08lx-%08lx : %04x\r\n", conf->nmem, range->base_min, range->base_max, range->length );
                    #endif
                    conf->nmem++;
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

    /* clean up and validate configs */
    if (!fail) {

        uint8_t devs_total = 0;
        uint8_t devs_invalid = 0;
        dev = card->devices;
        while (dev) {
            uint8_t confs_total = 0;
            uint8_t confs_invalid = 0;

            /* throw away the template if this is a multi-conf situation */
            if (dev->confs->next) {
                dev->confs = (pnp_conf_t*) dev->confs->next;
            }

            /* validate configs */
            conf = dev->confs;
            while (conf) {
                bool valid = true;
                /* must have at least some kind of resource */
                if ((conf->nmem + conf->nio + conf->nirq + conf->ndma) == 0) {
                    valid = false;
                }
                /* must support ISA high=true edge triggered interrupts */
                if ((conf->nirq > 0) && ((conf->irqmask[0] & (1UL << 16)) == 0)) {
                    valid = false;
                }
                if ((conf->nirq > 1) && ((conf->irqmask[1] & (1UL << 16)) == 0)) {
                    valid = false;
                }
                if (!valid) {
                    conf->flags |= (ISA_FLG_INVALID | ISA_FLG_DISABLED);
                    confs_invalid++;
                }
                confs_total++;
                conf = (pnp_conf_t*) conf->next;
            }

            if (confs_invalid == confs_total) {
                dev->flags |= (ISA_FLG_INVALID | ISA_FLG_DISABLED);
                devs_invalid++;
            }

            /* inital settings based on first config */
            pnp_device_init_settings(dev, 0);

            devs_total++;
            dev = (pnp_device_t*) dev->next;
        }
        card->ndevices = devs_total;
        fail = (devs_invalid == devs_total) ? true : false;
    }
    return fail ? false : true;
}

int pnp_enumerate(void)
{
    int csn;
    static int valid_cards = -1;
    static uint8_t* resbuf = 0;

    /* do nothing if pnp is disabled */
    if (isa.pnp.rdport == 0) {
        valid_cards = 0;
    }

    /* don't do this twice */
    if (valid_cards >= 0) {
        return valid_cards;
    }

    /* allocated temporary buffer for resouce data */
    resbuf = isabios_mem_alloc_temp(PNP_TEMP_RESBUF_SIZE);
    if (!resbuf) {
        valid_cards = 0;
        return valid_cards;
    }
    memset(resbuf, 0, PNP_TEMP_RESBUF_SIZE);

    /* reset all csns */
    pnp_write_reg(PNP_REG_CONFIG_CONTROL, PNP_CONFIG_CONTROL_RESET | PNP_CONFIG_CONTROL_WAIT_KEY | PNP_CONFIG_CONTROL_RESET_CSN);
    delayus(4000);
    pnp_write_key();
    delayus(4000);
    pnp_write_reg(PNP_REG_CONFIG_CONTROL, PNP_CONFIG_CONTROL_RESET | PNP_CONFIG_CONTROL_RESET_CSN);
    delayus(4000);
    pnp_write_reg(PNP_REG_WAKE_CSN, 0);
    delayus(4000);
    pnp_write_reg(PNP_REG_SET_RDPORT, isa.pnp.rdport >> 2);
    delayus(4000);

    /* do serial isolation, parse resources and assign csns */
    for (csn = 1; csn <= ISA_MAX_CARDS; csn++ ) {
        int try;
        uint32_t card_id = 0;
        uint32_t card_sn = 0;
        bool id_valid = false;

        /* get vendor and device id */
        for (try=0; try<2 && !id_valid; try++) {
            int i;
            uint8_t data[9];
            int32_t crc = 0x6a;

            pnp_write_reg(PNP_REG_WAKE_CSN, 0);
            delayus(10000UL);

            memset(data, 0, sizeof(data));
            isa.bus.outp(isa.pnp.adport, PNP_REG_SERIAL_ISOLATION);
            delayus(4000);

            for (i=0; i<72; i++) {
                int bit = (isa.bus.inp(isa.pnp.rdport) == 0x55) ? 1 : 0;
                delayus(500);
                bit = ((isa.bus.inp(isa.pnp.rdport) == 0xAA) && bit) ? 1 : 0;
                delayus(500);
                id_valid = id_valid || bit;
                if (i < 64) {
                    crc = (crc >> 1) | (((crc ^ (crc >> 1) ^ bit) << 7) & 0xff);
                }
                data[i / 8] = (data[i / 8] >> 1) | (bit ? 0x80 : 0);
            }

            id_valid = id_valid && (data[8] == crc);
            if (id_valid) {
                uint32_t* d = (uint32_t*)data;
                card_id = d[0]; 
                card_sn = isabios_swap32(d[1]);
            }
        }
        if (!id_valid) {
            break;
        }

        /* read card resources */
        {
            bool done = false; bool ok = true; uint32_t size = 0;
            pnp_write_reg(PNP_REG_SELECT_CSN, csn);
            while (!done && ok) {
                ok = pnp_read_res(&resbuf[size], 1); size++;
                if (ok) {
                    uint8_t res_tag = resbuf[size-1];
                    if ((res_tag & 0x80) == 0) {
                        /* small resource */
                        uint8_t res_len = (res_tag & 0x07);
                        uint8_t res_num = ((res_tag >> 3) & 0xf);
                        ok = pnp_read_res(&resbuf[size], res_len);
                        size += res_len;
                        if (ok && (res_num == 0xf)) {
                            done = true;
                        }
                    }
                    else {
                        /* large resource */
                        /*uint8_t res_num = (res_tag & 0x7f);*/
                        ok = pnp_read_res(&resbuf[size], 2);
                        size += 2;
                        if (ok) {
                            uint16_t res_len = resbuf[size-2] + (resbuf[size-1] << 8);
                            ok = pnp_read_res(&resbuf[size], res_len);
                            size += res_len;
                        }
                    }
                }
            }

            /* parse resource buffer, create card and devices */
            if (ok && parse_resource_buffer(csn, card_id, card_sn, resbuf, size)) {
                valid_cards++;
            }

            /* next one for serial isolation */
            pnp_write_reg(PNP_REG_WAKE_CSN, 0);
            delayus(10000UL);
        }
    }

    pnp_write_reg(PNP_REG_CONFIG_CONTROL, PNP_CONFIG_CONTROL_WAIT_KEY);
    return valid_cards;
}

int pnp_configure(void) {
    int count = 0;
    pnp_card_t* card = pnp_get_card(0);
    while (card) {
        if (card->csn) {
            pnp_device_t* dev = card->devices;
            while (dev) {
                count += pnp_device_configure(dev) ? 1 : 0;
                dev = (pnp_device_t*) dev->next;
            }
        }
        card = (pnp_card_t*) card->next;
    }
    return count;
}

bool pnp_init(void) {

    /* create dummy card for non-pnp devices */
    if (!pnp_get_card(pnp_string_to_id("OLD0000"))) {
        pnp_card_t* card = pnp_create_card(pnp_string_to_id("OLD0000"));
        sprintf(card->name, "Legacy devices");
        card->csn = 0;
    }

    return true;
}
