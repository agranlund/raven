#include "isa_bios.h"
#include "stdio.h"
#include "string.h"
#include "mint/basepage.h"
#include "mint/cookie.h"
#include "mint/osbind.h"
#include "isa_rw.h"
#include "isa_pnp.h"
#include "isa_inf.h"

/*
 * todo:
 *
 *  we could expose a PCI_BIOS interface.
 * 
 *  In fact, the PCI_BIOS API draft mentioned being suitable for ISA also.
 *  But I don't think that was ever tested or thought through.
 * 
 *  - The biggest issue is that the API cannot safely mix PCI and ISA
 *    devices because there is no guarantee against GUID collisions.
 *
 *    PCI GUIDs are unique only among PCI devices
 *    ISA GUIDs are unique only among ISA devices
 
 *    A PCI device could in theory have the same GUID as a completely
 *    different ISA device, and the API only specifies a single
 *    "get device by guid" function.
 * 
 *  - A secondary issue could that PCI, and thus PCI_BIOS, specifies up to
 *    five resource per device. ISA can in theory have 12 (8 io + 4 mem)
 *    Likely not an issue in practice.
 * 
 *  - A third issue as it stands is that PCI_BIOS has no provision for
 *    interrogating interrupt numbers used by a device.
 *    It provides a function to hook all interrupts for a given card,
 *    which is better than nothing and fine for PCI.
 *    But not quite the same as hooking a specific irq of a specific
 *    device on a specific card as is not uncommon for ISA.
 *
 */


/*-----------------------------------------------------------------------------------
 * globals
 *---------------------------------------------------------------------------------*/
isabios_t   isa;
irqlist_t   isa_irq_list[16];
void        (*isa_irq_assign)(void);


/*-----------------------------------------------------------------------------------
 *
 * temp crap to be removed.
 * 
 * todo: different interface for drivers/apps to interface with isa devices.
 *       use handles instead of direct access to structures.
 *
 *       rvsound and mxplay plugins are currently relying on access to
 *       the old isadev_t structures so this temp stuff is here to
 *       maintain backwards compat until a new interface is made and those
 *       softwares have been updated to use it.
 *
 *---------------------------------------------------------------------------------*/

static isa_dev_t* _ISA_API _TEMP_pubdevs_find(const char* idstr, uint16l_t idx) {
    uint32_t id = pnp_string_to_id(idstr);
    uint16_t found = 0;
    uint16_t i; int j;
    for (i=0; i<isa.bus.numdevs; i++) {
        for (j=0; j<ISA_MAX_DEV_IDS && (isa.bus.devs[i].id[j] != 0); j++) {
            if (isa.bus.devs[i].id[j] == id) {
                if (found == idx) {
                    return &isa.bus.devs[i];
                }
                found++;
                break;
            }
        }
    }
    return 0;
}

static void _TEMP_pubdevs_create(void) {
    /* enumerate devices */
    uint16_t numdevs = 0;
    pnp_device_t* dev;   
    pnp_card_t* card;
    card = pnp_get_card(0);
    while (card) {
        dev = card->devices;
        while (dev) {
            if (dev->flags & ISA_FLG_ACTIVE) {
                pnp_setting_t* sett = pnp_device_get_settings(dev);
                pnp_conf_t* conf = sett ? pnp_device_get_conf(dev, sett->conf) : 0;
                numdevs += (conf ? 1 : 0);
            }
            dev = (pnp_device_t*) dev->next;
        }
        card = (pnp_card_t*) card->next;        
    }

    /* now create public devices */
    if (numdevs) {
        int i; isa_dev_t* pdev;
        isa.bus.numdevs = numdevs;
        isa.bus.devs = isabios_mem_alloc(sizeof(isa_dev_t) * numdevs);
        pdev = isa.bus.devs;
        card = pnp_get_card(0);
        while (card) {
            dev = card->devices;
            while (dev) {
                if (dev->flags & ISA_FLG_ACTIVE) {
                    pnp_setting_t* sett = pnp_device_get_settings(dev);
                    pnp_conf_t* conf = sett ? pnp_device_get_conf(dev, sett->conf) : 0;
                    if (conf) {
                        for (i=0; i<ISA_MAX_DEV_IDS; i++) { pdev->id[i] = dev->ids[i];   }
                        for (i=0; i<conf->nio;  i++) { pdev->port[i] = sett->iobase[i];  }
                        for (i=0; i<conf->nmem; i++) { pdev->mem[i]  = sett->membase[i]; }
                        for (i=0; i<conf->nirq; i++) { pdev->irq[i]  = sett->irq[i];     }
                        for (i=0; i<conf->ndma; i++) { pdev->dma[i]  = sett->dma[i];     }
                        pdev++;
                    }
                }
                dev = (pnp_device_t*) dev->next;
            }
            card = (pnp_card_t*) card->next;        
        }
    }
}



/*-----------------------------------------------------------------------------------
 *
 * machine independent interrupt handling
 *
 *---------------------------------------------------------------------------------*/
 static uint32_t irq_attach_s(uint8l_t irq, uint32_t func) {
    /* IRQ2 and IRQ9 are the same */
    if (irq == 9) {
        irq = 2;
    }
    /* make sure platform supports this irq number */
    if (isa.bus.irqmask & (1UL << irq)) {
        irqlist_t* il = &isa_irq_list[irq];
        /* put callback in next free slot */
        if (il->count < ISA_MAX_IRQFUNCS) {
            il->func[il->count] = func;
            il->count++;
            /* reassign platform vectors if needed */
            if (isa_irq_assign) {
                isa_irq_assign();
            }
            return il->count;
        }
    }
    return 0;
}

static uint32_t irq_remove_s(uint8l_t irq, uint32_t func) {
    uint32_t i;
    irqlist_t* il;
    /* IRQ2 and IRQ9 are the same */
    if (irq == 9) {
        irq = 2;
    }
    /* search for the callback */
    il = &isa_irq_list[irq];
    for (i=0; i<il->count; i++) {
        /* remove it */
        if (il->func[i] == func) {
            il->count--;
            il->func[i] = il->func[il->count];
            /* reassign platform vectors if needed */
            if (isa_irq_assign) {
                isa_irq_assign();
            }
            return il->count + 1;
        }
    }
    return 0;
}

static uint16_t irq_disable_interrupt_save = 0;
static long irq_di(void) { irq_disable_interrupt_save = isabios_disable_interrupts(); return 0; }
static long irq_ei(void) { isabios_restore_interrupts(irq_disable_interrupt_save); return 0; }

#define IRQ_CRITICAL_BEGIN() Supexec(irq_di)
#define IRQ_CRITICAL_END()   Supexec(irq_ei)

static uint32_t _ISA_API irq_attach(uint8l_t irq, void(*func)(void)) {
    int32_t rt;
    IRQ_CRITICAL_BEGIN();
    rt = irq_attach_s(irq, (uint32_t)func);
    if (rt == 1) {
        /* first handler was added */
    }
    IRQ_CRITICAL_END();
    return rt;
}

static uint32_t _ISA_API irq_remove(uint8l_t irq, void(*func)(void)) {
    int32_t rt;
    IRQ_CRITICAL_BEGIN();
    rt = irq_remove_s(irq, (uint32_t)func);
    if (rt == 1) {
        /* last hander was removed */
    }
    IRQ_CRITICAL_END();
    return rt;
}




/*-----------------------------------------------------------------------------------
 *
 * Info / logging
 *
 *---------------------------------------------------------------------------------*/
void print_businfo(void) {

    #if ISABIOS_ENABLE_LOG_SCREEN
    {
        isabios_print("\r\n\33pISA PnP Bios v%d.%02x\33q\r\n", (ISA_BIOS_VERSION >> 8), (ISA_BIOS_VERSION & 0xFF));
        isabios_print("I/O: 0x%08lx\r\n", isa.bus.iobase);
        if (isa.bus.membase) { isabios_print("MEM: 0x%08lx\r\n", isa.bus.membase); }
        if (isa.bus.irqmask) { isabios_print("IRQ: 0x%08lx\r\n",  (uint32_t)isa.bus.irqmask); }
        if (isa.bus.drqmask) { isabios_print("DMA: 0x%08lx\r\n",  (uint32_t)isa.bus.drqmask); }
        isabios_print("\r\n");
    }
    #endif

    #if ISABIOS_ENABLE_LOG_FILE
    {
        int i;
        isabios_log("\r\n");
        isabios_log("--------------------------------------------------------------------------\r\n");
        isabios_log("Interface\r\n");
        isabios_log("--------------------------------------------------------------------------\r\n");
        isabios_log("I/O: 0x%08lx\r\n", isa.bus.iobase);
        isabios_log("MEM: 0x%08lx\r\n", isa.bus.membase);
        isabios_log("END: ");
        switch (isa.bus.endian) {
            case ISA_ENDIAN_LELS: isabios_log("LELS"); break;
            case ISA_ENDIAN_LEAS: isabios_log("LEAS"); break;
            default: isabios_log("BE"); break;
        }
        isabios_log("\r\n");
        isabios_log("IRQ:"); for (i=0; i<16; i++) { if (isa.bus.irqmask & (1 << i)) { isabios_log(" %d", i); } } isabios_log("\r\n");
        isabios_log("DMA:"); for (i=0; i<8;  i++) { if (isa.bus.drqmask & (1 << i)) { isabios_log(" %d", i); } } isabios_log("\r\n");
    }
    #endif
}

void print_resources(void) {
    #if ISABIOS_ENABLE_LOG_FILE
    {
        int i, j;
        pnp_card_t* card;

        isabios_log("\r\n");
        isabios_log("--------------------------------------------------------------------------\r\n");
        isabios_log("Resources\r\n");
        isabios_log("--------------------------------------------------------------------------\r\n");
        card = pnp_get_card(0);
        while (card) {
            pnp_device_t* dev = card->devices;
            if (dev) {
                isabios_log("[CARD%2d - %s] '%s'\r\n", card->csn, pnp_id_to_string(card->id), card->name);
                while (dev) {
                    pnp_conf_t* conf = dev->confs;
                    if (conf) {
                        isabios_log(" [DEV%2d - %s] '%s'\r\n", dev->ldn, pnp_id_to_string(dev->ids[0]), dev->name);
                        for (i=1; i<ISA_MAX_DEV_IDS; i++) {
                            if (dev->ids[i]) {
                                isabios_log(" [        %s]\r\n", pnp_id_to_string(dev->ids[i]));
                            }
                        }
                        while (conf) {
                            uint32_t resources = conf->nio + conf->nmem + conf->nirq + conf->ndma;
                            if (resources && !(conf->flags & ISA_FLG_INVALID)) {
                                int num;
                                isabios_log("  [CONF%2d]\r\n", conf->id);
                                for (j=0; j<conf->nio; j++)  {
                                    isabios_log("    IO%d: %08lx-%08lx : %08lx\r\n", j, conf->iorange[j].base_min, conf->iorange[j].base_max, conf->iorange[j].length);
                                }
                                for (j=0; j<conf->nmem; j++) {
                                    isabios_log("   MEM%d: %08lx-%08lx : %08lx\r\n", j, conf->memrange[j].base_min, conf->memrange[j].base_max, conf->memrange[j].length);
                                }
                                for (num=0, j=0; j<conf->nirq; j++) {
                                    isabios_log("   IRQ%d:", j);
                                    for (num=0, i=0; i<16; i++) { if (conf->irqmask[j] & (1 << i)) { isabios_log("%s%d", ((num==0) ? " " : " "), i); num++; } }
                                    isabios_log("\r\n");
                                }
                                for (num=0, j=0; j<conf->ndma; j++) {
                                    isabios_log("   DMA%d:", j);
                                    for (num=0, i=0; i<8;  i++) { if (conf->dmamask[j] & (1 << i)) { isabios_log("%s%d", ((num==0) ? " " : " "), i); num++; } }
                                    isabios_log("\r\n");
                                }
                            }
                            conf = (pnp_conf_t*) conf->next;
                        }
                    }
                    dev = (pnp_device_t*) dev->next;
                }
                isabios_log("\r\n");
            }
            card = (pnp_card_t*) card->next;
        }
    }
    #endif    
}

void print_devices(void) {

    #if ISABIOS_ENABLE_LOG_SCREEN
    {
        pnp_card_t* card = pnp_get_card(0);
        while (card) {
            bool printed_card = false;
            pnp_device_t* dev = card->devices;
            while (dev) {
                if (dev->flags & ISA_FLG_ACTIVE) {
                    if (!printed_card) {
                        printed_card = true;
                        isabios_print("%s\r\n", card->name);
                    }
                    isabios_print(" [%01x:%01x] %s\r\n", dev->csn, dev->ldn, dev->name);
                }
                dev = (pnp_device_t*) dev->next;
            }
            if (printed_card) {
                isabios_print("\r\n");
            }
            card = (pnp_card_t*) card->next;
        }
        isabios_print("\r\n");
    }
    #endif

    #if ISABIOS_ENABLE_LOG_FILE
    {
        pnp_card_t* card;
        isabios_log("--------------------------------------------------------------------------\r\n");
        isabios_log("Configured\r\n");
        isabios_log("--------------------------------------------------------------------------\r\n");
        card = pnp_get_card(0);
        while (card) {
            pnp_device_t* dev = card->devices;
            while (dev) {
                if (dev->flags & ISA_FLG_ACTIVE) {
                    pnp_setting_t* set = pnp_device_get_settings(dev);
                    pnp_conf_t* conf = set ? pnp_device_get_conf(dev, set->conf) : 0;
                    if (conf) {
                        int j;
                        isabios_log("%02x:%02x ID:  ", dev->csn, dev->ldn);
                        for (j=0; j<ISA_MAX_DEV_IDS && dev->ids[j]; j++) { isabios_log("%s ", pnp_id_to_string(dev->ids[j])); } isabios_log("\r\n");
                        if (conf->nio)  { isabios_log("      IO:  "); for (j=0; j<conf->nio;  j++) { isabios_log("%04x ",  set->iobase[j]);  } isabios_log("\r\n"); }
                        if (conf->nmem) { isabios_log("      MEM: "); for (j=0; j<conf->nmem; j++) { isabios_log("%08lx ", set->membase[j]); } isabios_log("\r\n"); }
                        if (conf->nirq) { isabios_log("      IRQ: "); for (j=0; j<conf->nirq; j++) { isabios_log("%d ",    set->irq[j]);     } isabios_log("\r\n"); }
                        if (conf->ndma) { isabios_log("      DMA: "); for (j=0; j<conf->ndma; j++) { isabios_log("%d ",    set->dma[j]);     } isabios_log("\r\n"); }
                        isabios_log("\r\n");
                    }
                }
                dev = (pnp_device_t*) dev->next;
            }
            card = (pnp_card_t*) card->next;
        }
    }
    #endif
}


/*-----------------------------------------------------------------------------------
 *
 * Private bios interface
 *
 *---------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------
 * bus initialize
 *---------------------------------------------------------------------------------*/
bool isabios_bus_init(void) {
    bool mach = false;

    memset((void*)&isa, 0, sizeof(isabios_t));

    /* default bus settings */
    isa.bus.endian      = ISA_ENDIAN_LELS;
    isa.bus.irqmask     = 0x0000;
    isa.bus.drqmask     = 0x00;
    isa.bus.membase     = 0x00000000UL;
    isa.bus.iobase      = 0x00000000UL;

    /* default pnp ports */
    isa.pnp.adport      = 0x279;
    isa.pnp.wrport      = 0xa79;
    isa.pnp.rdport      = 0x203;
    
    /* irq setup */
    isa_irq_assign      = 0;

    /* machine dependent setup */
#if ISABIOS_MACH_HADES
    if (!mach) { mach = isabios_setup_hades(); }
#endif
#if ISABIOS_MACH_MILAN
    if (!mach) { mach = isabios_setup_milan(); }
#endif
#if ISABIOS_MACH_PANTHER
    if (!mach) { mach = isabios_setup_panther(); }
#endif
#if ISABIOS_MACH_RAVEN
    if (!mach) { mach = isabios_setup_raven(); }
#endif

#if ISABIOS_ENABLE_INF
    isabios_inf_configure_bus();
#endif

    /* common functions */
    isa.bus.version = ISA_BIOS_VERSION;
    isa.bus.find_dev = _TEMP_pubdevs_find;
    switch (isa.bus.endian) {
        case ISA_ENDIAN_BE:
            isa.bus.inp = isa.bus.inp ? isa.bus.inp : inp_be;
            isa.bus.outp = isa.bus.outp ? isa.bus.outp : outp_be;
            isa.bus.inpw = isa.bus.inpw ? isa.bus.inpw : inpw_be;
            isa.bus.outpw = isa.bus.outpw ? isa.bus.outpw : outpw_be;
            break;
        case ISA_ENDIAN_LEAS:
            isa.bus.inp = isa.bus.inp ? isa.bus.inp : inp_leas;
            isa.bus.outp = isa.bus.outp ? isa.bus.outp : outp_leas;
            isa.bus.inpw = isa.bus.inpw ? isa.bus.inpw : inpw_leas;
            isa.bus.outpw = isa.bus.outpw ? isa.bus.outpw : outpw_leas;
            break;
        case ISA_ENDIAN_LELS:
            isa.bus.inp = isa.bus.inp ? isa.bus.inp : inp_lels;
            isa.bus.outp = isa.bus.outp ? isa.bus.outp : outp_lels;
            isa.bus.inpw = isa.bus.inpw ? isa.bus.inpw : inpw_lels;
            isa.bus.outpw = isa.bus.outpw ? isa.bus.outpw : outpw_lels;
            break;
    }

    /* we need at least io access for this to make sense */
    isa.bus.irq_attach = irq_attach;
    isa.bus.irq_remove = irq_remove;

    print_businfo();
    return (isa.bus.iobase != 0);
}


/*-----------------------------------------------------------------------------------
 * pnp initialize
 *---------------------------------------------------------------------------------*/
bool isabios_pnp_init(void) {
    pnp_init();
    pnp_enumerate();
    print_resources();
#if ISABIOS_ENABLE_INF
    isabios_inf_configure_devices();
#endif
    return true;
}


/*-----------------------------------------------------------------------------------
 * bios init, called before anything else
 *---------------------------------------------------------------------------------*/
bool isabios_init(void) {
    isabios_mem_init(64 * 1024UL);
    isabios_log_init();
    isabios_inf_init();

    if (!isabios_bus_init()) {
        return false;
    }

    isabios_pnp_init();
    return true;
}


/*-----------------------------------------------------------------------------------
 * bios cleanup, should always be called when done
 *---------------------------------------------------------------------------------*/
void isabios_cleanup(void) {
    isabios_inf_close();
    isabios_log_close();
    isabios_mem_close();
}


/*-----------------------------------------------------------------------------------
 * start, configures devices and installs public interface
 *---------------------------------------------------------------------------------*/
bool isabios_start(void) {
    pnp_configure();
    print_devices();
    _TEMP_pubdevs_create();
    if (!isabios_createcookie(C__ISA, (uint32_t)&isa.bus)) {
        return false;
    }
    return true;
}





/*-----------------------------------------------------------------------------------
 *
 * Application
 *
 *---------------------------------------------------------------------------------*/

 #if ISABIOS_STANDALONE

long super_main(void) {

    if (!isabios_init()) {
        isabios_cleanup();
        return -1;
    }

    if (!isabios_start()) {
        isabios_cleanup();
        return -2;
    }

    isabios_cleanup();
    return 0;
}

int main() {
    long ret = Supexec(super_main);
    if (ret == 0) {
        extern uint32_t _PgmSize;
        Ptermres(_PgmSize, 0);
    }
    return (int)ret;
}

#endif
