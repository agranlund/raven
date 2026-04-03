#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <tos.h>
#include "isa_bios.h"
#include "isa_pnp.h"

static char* inf;
static char* inf_temp;

/*-----------------------------------------------------------------------------------
 * internal helpers
 *---------------------------------------------------------------------------------*/

static int string_to_int(const char* s) {
    return s ? atoi(s) : 0;
}

static uint32_t string_to_hex(const char* s) {
    int i; uint32_t result = 0;
    if (s) {
        /* skip hex designators */
        if (s[0] == '$') {
            s += 1;
        } else if (s[0] == '0' && s[1] == 'x') {
            s += 2;
        }
        /* parse number */
        for (i=0; i<8 && *s; i++) {
            char c = *s++;
            result <<= 4;
            if (c >= '0' && c <= '9') {
                result |= (0 + (c - '0'));
            } else if (c >= 'a' && c <= 'f') {
                result |= (10 + (c - 'a'));
            } else if (c >= 'A' && c <= 'F') {
                result |= (10 + (c - 'A'));
            } else {
                i = 8;
            }
        }
    }
    return result;
}

static const char* get_string(const char* key) {
    const char* src = inf;
    while (*src != 0) {
        int32_t len = strlen(src);
        const char* s = strstr(src, key);
        src += (len + 1);
        if (s) {
            return src;
        }
    }
    return 0;
}

bool get_hex(const char* key, uint32_t* val) {
    const char* str = get_string(key);
    if (str) {
        *val = string_to_hex(str);
        return true;
    }
    return false;
}

static const char* get_command(const char* find, const char* start, char** outc, int* outs) {
    int i; const char* src;
    int maxs = *outs; *outs = 0;
    
    src = start ? start : inf;
    for (i=0; i<maxs; i++) {
        outc[i] = 0;
    }
    while (*src != 0) {
        const char* next = src + strlen(src) + 1;
        if (strstr(src, find) == src) {
            char* token; int count;
            strncpy(inf_temp, src, 127);
            inf_temp[127] = 0;
            token = strtok(inf_temp, ".");
            count = 0;
            while (token) {
                if (count > 0) {
                    outc[count - 1] = token;
                }
                count += 1;
                token = strtok(0, ".");
            }
            *outs = count - 1;
            return next;
        }
        src = next;
    }
    return 0;
}

static pnp_device_t* find_device(pnp_card_t* card, uint32_t id, uint32_t idx) {
    bool wildcard = false;
    uint32_t count = 0;

    if (card == 0) {
        wildcard = true;
        card = pnp_get_card(0);
    }

    while (card) {
        pnp_device_t* dev = card->devices;
        while (dev) {
            int i;
            for (i=0; (i<ISA_MAX_DEV_IDS) && (dev->ids[i] != 0); i++) {
                if (dev->ids[i] == id) {
                    if (count == idx) {
                        return dev;
                    }
                    count++;
                    break;
                }
            }
            dev = (pnp_device_t*) dev->next;
        }
        card = (pnp_card_t*) (wildcard ? card->next : 0);
    }
    return 0;
}

static pnp_card_t* find_card(uint32_t id, uint32_t idx) {
    uint32_t count = 0;
    pnp_card_t* card = pnp_get_card(0);
    while (card && (id != 0)) {
        if (card->id == id) {
            if (count == idx) {
                return card;
            }
            count++;
        }
        card = (pnp_card_t*) card->next;
    }
    return card;
}

static uint32_t string_to_id(const char* str, uint32_t* idx) {
    uint32_t id = pnp_string_to_id(str);
    if (idx) {
        uint32_t len = strlen(str);
        if ((len == 9) && (str[len-2] == ':')) {
            *idx = str[len-1] - '0';
        } else {
            *idx = 0;
        }
    }
    return id;
}

static void configure_cards(void) {
    char* strc[8]; int strs = 2;
    const char* offs = 0;
    do {
        offs = get_command("card.", offs, strc, &strs);
        if (offs && (strs == 2)) {
            uint32_t cix;
            uint32_t cid = string_to_id(strc[0], &cix);
            pnp_card_t* card = find_card(cid, cix);
            if (!card) {
                continue;
            }
            if (strcmp(strc[1], "name") == 0) {
                uint32_t oldlen = strlen(card->name);
                uint32_t newlen = strlen(offs);
                if (oldlen < newlen) {
                    card->name = isabios_mem_alloc(newlen+1);
                }
                strcpy(card->name, offs);
            } else if (strcmp(strc[1], "enable") == 0) {
                if (string_to_int(offs) > 0)
                    card->flags &= ~ISA_FLG_DISABLED;
                else
                    card->flags |= ISA_FLG_DISABLED;
            }
        }
    } while(offs);
}

static void configure_device(pnp_device_t* dev, char* cmd, const char* val) {
    pnp_setting_t* settings = (dev && cmd && val) ? dev->settings : 0;
    pnp_conf_t* conf = settings ? pnp_device_get_conf(dev, settings->conf) : 0;
    if (conf) {
        int idx = 0;
        int32_t len = strlen(cmd);
        if (len > 0) {
            if ((cmd[len-1]>='0') && (cmd[len-1]<='9')) {
                idx = cmd[len-1] - '0';
                cmd[len-1] = 0;
            }
            if (strcmp(cmd, "io") == 0) {
                if (idx < ISA_MAX_DEV_PORT) {
                    if (dev->csn == 0) {
                        conf->nio = idx < conf->nio ? conf->nio : idx + 1;
                        conf->iorange[idx].base_min = string_to_hex(val);
                        conf->iorange[idx].base_max = conf->iorange[idx].base_min;
                        settings->iobase[idx] = conf->iorange[idx].base_min;
                    } else if (idx < conf->nio) {
                        settings->iobase[idx] = string_to_hex(val);
                    }
                }
            }
            else if (strcmp(cmd, "mem") == 0) {
                if (idx < ISA_MAX_DEV_MEM) {
                    if (dev->csn == 0) {
                        conf->nmem = idx < conf->nmem ? conf->nmem : idx + 1;
                        conf->memrange[idx].flags = 0x11;
                        conf->memrange[idx].base_min = string_to_hex(val);
                        conf->memrange[idx].base_max = conf->memrange[idx].base_min;
                        settings->membase[idx] = conf->memrange[idx].base_min;
                    } else if (idx < conf->nmem) {
                        settings->membase[idx] = string_to_hex(val);
                    }
                }
            }
            else if (strcmp(cmd, "irq") == 0) {
                if (idx < ISA_MAX_DEV_IRQ) {
                    uint32_t irq = (uint32_t)string_to_int(val);
                    if (irq > 15) { irq = 0; }
                    if (dev->csn == 0) {
                        conf->nirq = idx < conf->nirq ? conf->nirq : idx + 1;
                        conf->irqmask[idx] = (1UL << irq) | 0x00010000UL;
                        settings->irq[idx] = irq;
                    } else if (idx < conf->nirq) {
                        settings->irq[idx] = irq;
                    }
                }
            }
            else if (strcmp(cmd, "dma") == 0) {
                if (idx < ISA_MAX_DEV_DMA) {
                    uint16_t dma = (uint16_t)string_to_int(val);
                    if (dma > 8) { dma = 4; }
                    if (dev->csn == 0) {
                        conf->ndma = idx < conf->ndma ? conf->ndma : idx + 1;
                        conf->dmamask[idx] = (1UL << dma);
                        settings->dma[idx] = dma;
                    } else if (idx < conf->ndma) {
                        settings->dma[idx] = dma;
                    }
                }
            }
            else if (strcmp(cmd, "name") == 0) {
                uint32_t oldlen = strlen(dev->name);
                uint32_t newlen = strlen(val);
                if (oldlen < newlen) {
                    dev->name = isabios_mem_alloc(newlen + 1);
                }
                strcpy(dev->name, val);
            }
            else if (strcmp(cmd, "id") == 0) {
                if (idx < ISA_MAX_DEV_IDS) {
                    dev->ids[idx] = pnp_string_to_id(val);
                }
            }
            else if (strcmp(cmd, "enable") == 0) {
                if (string_to_int(val) == 0)
                    dev->flags |= ISA_FLG_DISABLED;
                else
                    dev->flags &= ~ISA_FLG_DISABLED;
            }
            else if(strcmp(cmd, "conf") == 0) {
                int num = string_to_int(val);
                if ((num > 0) && (num < PNP_MAX_CONFS)) {
                    pnp_device_init_settings(dev, num);
                }
            }
        }
    }
}

static void configure_devices(void) {
    uint32_t cid, cix, did, dix;
    pnp_card_t* card; pnp_device_t* dev;
    char* cmd; char* strc[8]; int strs;

    const char* offs = 0;
    do {
        strs = 3; offs = get_command("dev.", offs, strc, &strs);
        if (offs) {
            cmd = strc[strs-1];
            if (strs == 3) {
                /* dev.<card>.<device>.<cmd> = value */

                /* treat card id 0 as legacy device */
                if (strcmp(strc[0], "0") != 0) {
                    cid = string_to_id(strc[0], &cix);
                }

                did = string_to_id(strc[1], &dix);
                card = find_card(cid, cix);
                dev = (card ? find_device(card, did, dix) : 0);

                /* manually added legacy device */
                if (!dev && card && (card->csn == 0)) {
                    dev = pnp_create_device(card, did);
                    pnp_create_conf(dev);
                }

                if (dev) {
                    configure_device(dev, cmd, offs);
                }
            } else if (strs == 2) {
                /* dev.<device>.<cmd> = value */
                did = string_to_id(strc[0], &dix);
                dev = find_device(0, did, dix);
                if (dev) {
                    configure_device(dev, cmd, offs);
                }
            }
        }
    } while(offs);
}

/*-----------------------------------------------------------------------------------
 * configure devices
 *---------------------------------------------------------------------------------*/

void isabios_inf_configure_devices(void) {
    if (inf) {
        configure_cards();
        configure_devices();
    }
}

/*-----------------------------------------------------------------------------------
 * configure bus
 *---------------------------------------------------------------------------------*/

void isabios_inf_configure_bus(void) {
    if (inf) {
        uint32_t cfgint;
        const char* cfgstr = get_string("isa.endian");
        if (cfgstr && strcmp(cfgstr, "be") == 0)    isa.bus.endian  = ISA_ENDIAN_BE;
        if (cfgstr && strcmp(cfgstr, "leas") == 0)  isa.bus.endian  = ISA_ENDIAN_LEAS;
        if (cfgstr && strcmp(cfgstr, "lels") == 0)  isa.bus.endian  = ISA_ENDIAN_LELS;
        if (get_hex("isa.iobase",  &cfgint))        isa.bus.iobase  = cfgint;
        if (get_hex("isa.membase", &cfgint))        isa.bus.membase = cfgint;
        if (get_hex("isa.irqmask", &cfgint))        isa.bus.irqmask = cfgint;
        if (get_hex("isa.drqmask", &cfgint))        isa.bus.drqmask = cfgint;
        if (get_hex("isa.pnp.port", &cfgint) && (cfgint != 0)) {
            isa.pnp.rdport = (uint16_t)cfgint;
        }
        if (get_hex("isa.pnp.enable", &cfgint) && (cfgint == 0)) {
            isa.pnp.rdport = 0;
        }
    }
}

/*-----------------------------------------------------------------------------------
 * close inf file
 *---------------------------------------------------------------------------------*/

void isabios_inf_close(void) {
    inf = 0;
}

/*-----------------------------------------------------------------------------------
 * open inf file
 *---------------------------------------------------------------------------------*/

void isabios_inf_init(void) {
    int16_t fhandle;
    int32_t fsize;
    char *src, *dst, *end;
    bool keepspaces;

    char fname[32] = "c:\\isa_bios.inf";
    fname[0] = 'a' + (char) (*((volatile uint16_t*)0x446));

    fsize = Fopen(fname, 0);
    if (fsize < 0) {
        return;
    }
    fhandle = (int16_t)fsize;

    fsize = Fseek(0, fhandle, SEEK_END);
    if (fsize < 2) {
        Fclose(fhandle);
        return;
    }

    inf = isabios_mem_alloc_temp(fsize+1);
    inf_temp = isabios_mem_alloc_temp(128);
    Fseek(0, fhandle, SEEK_SET);
    Fread(fhandle, fsize, inf);
    inf[fsize] = 0;
    Fclose(fhandle);

    /* skip comments until end of line */
    src = dst = inf;
    end = src + fsize;
    while (src < end) {
        char c = *src++;
        if (c == '#') {
            while ((src < end) && (*src != '\r') && (*src != '\n')) { src++; }
            while ((src < end) && ((*src == '\r') || (*src == '\n'))) { src++; }
        } else {
            *dst++ = c;
        }
    }

    /* convert whitespaces and illegal characters.
       keep spaces when inside quotation marks, but don't keep the
       actual quotation marks themselves. */
    end = dst; src = inf;
    keepspaces = false;
    while (src < end) {
        uint8_t c = *((uint8_t*)src);
        if (c == 0x22) {
            keepspaces = !keepspaces;
            *src = 0;
        } else  if ((c < (keepspaces ? 0x20 : 0x21)) || (c > 0x7E) || (c == '=')) {
            *src = 0;
        }
        src++;
    }

    /* extract strings */
    end = src; src = inf; dst = inf; *end = 0;
    while (src < end) {
        while ((src < end) && (*src == 0)) { src++; }
        while ((src < end) && (*src != 0)) { *dst++ = *src++; }
        *dst++ = 0;
    }
}
