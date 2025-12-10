#include "isa_core.h"
#include "stdio.h"
#include "string.h"
#include "mint/basepage.h"
#include "mint/cookie.h"
#include "mint/osbind.h"
#include "isa_rw.h"
#include "isa_pnp.h"

#ifdef __PUREC__
uint32_t    _StkSize = 4096;
#endif

isa_core_t  isa;
isa_dev_t   isa_bus_devs[ISA_MAX_DEVS];

#define MAX_IRQ_FUNCS 14

typedef struct {
    uint32_t count;
    uint32_t func[MAX_IRQ_FUNCS+1];
} irqlist_t;

irqlist_t irq_lists[16];

extern void irqvec_isa02_mfp2B0(void);
extern void irqvec_isa03_mfp2B1(void);
extern void irqvec_isa04_mfp2B2(void);
extern void irqvec_isa05_mfp2B3(void);
extern void irqvec_isa07_mfp2B6(void);
extern void irqvec_isa10_mfp2B7(void);
extern void irqvec_isa11_mfp2A6(void);
extern void irqvec_isa14_mfp2A7(void);

/*-----------------------------------------------------------------------------------
 * interrupts
 *---------------------------------------------------------------------------------*/
uint32_t irq_attach_s(uint8l_t irq, uint32_t func) {
    irqlist_t* il;
    if (irq == 9) { irq = 2; }  /* isa interrupts 2 and 9 are the same */
    il = &irq_lists[irq];
    if (il->count < MAX_IRQ_FUNCS) {
        il->func[il->count] = func;
        il->count++;
        return il->count;
    }
    return 0;
}

uint32_t irq_remove_s(uint8l_t irq, uint32_t func) {
    uint32_t i;
    irqlist_t* il;
    if (irq == 9) { irq = 2; }  /* isa interrupts 2 and 9 are the same */
    il = &irq_lists[irq];
    for (i=0; i<il->count; i++) {
        if (il->func[i] == func) {
            il->count--;
            il->func[i] = il->func[il->count];
            return il->count + 1;
        }
    }
    return 0;
}

static uint16_t irq_disable_interrupt_save = 0;
long irq_di(void) { irq_disable_interrupt_save = disable_interrupts(); return 0; }
long irq_ei(void) { restore_interrupts(irq_disable_interrupt_save); return 0; }


uint32_t _ISA_API irq_attach(uint8l_t irq, uint32_t func) {
    int32_t rt;
    Supexec(irq_di);
    rt = irq_attach_s(irq, func);
    if (rt == 1) {
        /* first handler was added */
    }
    Supexec(irq_ei);
    return rt;
}

uint32_t _ISA_API irq_remove(uint8l_t irq, uint32_t func) {
    int32_t rt;
    Supexec(irq_di);
    rt = irq_remove_s(irq, func);
    if (rt == 1) {
        /* last hander was removed */
    }
    Supexec(irq_ei);
    return rt;
}


/*-----------------------------------------------------------------------------------
 * device helpers
 *---------------------------------------------------------------------------------*/
isa_dev_t* _ISA_API dev_find(const char* idstr, uint16l_t idx) {
    uint32_t id = StrToId(idstr);
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

/*-----------------------------------------------------------------------------------
 * 
 * Hardware specific bus config
 *
 *---------------------------------------------------------------------------------*/
static bool bus_hwconf(void) {
    long cookie = 0;

    isa.bus.endian      = ISA_ENDIAN_LELS;
    isa.bus.irqmask     = 0x0000;
    isa.bus.drqmask     = 0x00;
    isa.bus.membase     = 0x00000000UL;
    isa.bus.iobase      = 0x00000000UL;

    if (Getcookie(C_hade, &cookie) == C_FOUND)          /* Hades */
    {
        isa.bus.iobase  = 0xFFF30000UL;
        isa.bus.membase = 0xFF000000UL;
    }
    else if (Getcookie(C__MIL, &cookie) == C_FOUND)     /* Milan */
    {
        #if 1
        isa.bus.iobase = 0xC0000000UL;
        #else        
        isa.bus.endian = ISA_ENDIAN_LEAS;
        isa.bus.iobase = 0x80000000;
        #endif
    }
    else if (Getcookie(C__P2I, &cookie) == C_FOUND)     /* Panther2 */
    {
        uint32_t* cardpth2 = (uint32_t*) (cookie+6);
        isa.bus.iobase  = *cardpth2;
        cardpth2 += 2;
        isa.bus.membase = *cardpth2;
    }    
    else if (Getcookie(C_RAVN, &cookie) == C_FOUND)     /* Raven */
    {
        isa.bus.iobase  = 0x81000000UL;
        isa.bus.membase = 0x82000000UL;
        isa.bus.irqmask = (0
            | (1UL << 14)
            | (1UL << 11)
            | (1UL << 10)
            | (1UL <<  7)
            | (1UL <<  5)
            | (1UL <<  4)
            | (1UL <<  3)
            | (1UL <<  2)
        );

        /* enable ISA interrupts */
        
        /*
         * todo:
         *  this is disabled for now because Raven.A1 board has a hardware
         *  error preventing ISA interrupts from being detected.
         *  
         *  IRQ lines should be pulled high to +5V rather than low to GND.
         * 
         *  Cards trigger IRQ by pulling the line low for x cycles and then
         *  release, making it go back high so that we can detect
         *  that low -> high transition.
         * 
         *  Raven.A1 board pulls them low, thinking that cards were
         *  supposed to drive the lines high on interrupts.
         * 
         *  Will be fixed in Raven.A2 and some kind of hardware patch
         *  might make it's way to the A1 version.
         *
         */
        #if 0
        {
            /*            
            mfp2_B0 =  0 = I0 = irq2/9
            mfp2_B1 =  1 = I1 = irq3
            mfp2_B2 =  2 = I2 = irq4
            mfp2_B3 =  3 = I3 = irq5
            mfp2_B6 =  6 = I4 = irq7
            mfp2_B7 =  7 = I5 = irq10
            mfp2_A6 = 14 = I6 = irq11
            mfp2_A7 = 15 = I7 = irq14
            */
            const uint8_t ia_mask = 0xC0; /* xx...... */
            const uint8_t ib_mask = 0xCF; /* xx..xxxx */

            volatile uint8_t *mfpbase = (volatile uint8_t*) 0xA0000A00UL;
            uint32_t* mfpvecs = (uint32_t*)0x140;

            /* disable interrupts */
            mfpbase[0x07] &= ~ia_mask;
            mfpbase[0x09] &= ~ib_mask;

            /* set vectors */
            mfpvecs[ 0] = (uint32_t)irqvec_isa02_mfp2B0;    /* also irq 9 */
            mfpvecs[ 1] = (uint32_t)irqvec_isa03_mfp2B1;
            mfpvecs[ 2] = (uint32_t)irqvec_isa04_mfp2B2;
            mfpvecs[ 3] = (uint32_t)irqvec_isa05_mfp2B3;
            mfpvecs[ 6] = (uint32_t)irqvec_isa07_mfp2B6;
            mfpvecs[ 7] = (uint32_t)irqvec_isa10_mfp2B7;
            mfpvecs[14] = (uint32_t)irqvec_isa11_mfp2A6;
            mfpvecs[15] = (uint32_t)irqvec_isa14_mfp2A7;

            /* enable isa interrupts */
            mfpbase[0x0b] &= ~ia_mask;  /* clear pending    */
            mfpbase[0x0f] &= ~ia_mask;  /* clear service    */
            mfpbase[0x13] |=  ia_mask;  /* clear mask       */
            mfpbase[0x0d] &= ~ib_mask;  /* clear pending    */
            mfpbase[0x11] &= ~ib_mask;  /* clear service    */
            mfpbase[0x15] |=  ib_mask;  /* clear mask       */
            mfpbase[0x07] |= ia_mask;   /* enable interrupt */
            mfpbase[0x09] |= ib_mask;   /* enable interrupt */
        }
        #endif
    }
    return true;
}

bool bus_init(void)
{
    memset((void*)&isa, 0, sizeof(isa_core_t));
    isa.bus.devs = isa_bus_devs;

    /* hardware autodetect */
    if (!bus_hwconf())
        return false;

    /* bus settings from config file */
    {
        uint32_t cfgint;
        const char* cfgstr = GetInfStr("isa.endian");
        if (cfgstr && strcmp(cfgstr, "be") == 0)    isa.bus.endian = ISA_ENDIAN_BE;
        if (cfgstr && strcmp(cfgstr, "leas") == 0)  isa.bus.endian = ISA_ENDIAN_LEAS;
        if (cfgstr && strcmp(cfgstr, "lels") == 0)  isa.bus.endian = ISA_ENDIAN_LELS;
        if (GetInfHex("isa.iobase", &cfgint))       isa.bus.iobase = cfgint;
        if (GetInfHex("isa.membase", &cfgint))      isa.bus.membase = cfgint;
        if (GetInfHex("isa.irqmask", &cfgint))      isa.bus.irqmask = cfgint;
        if (GetInfHex("isa.drqmask", &cfgint))      isa.bus.drqmask = cfgint;
    }

    /* check if we have minimum settings */
    if (isa.bus.iobase == 0)
        return false;

    /* common setup */
    isa.bus.version = ISA_BIOS_VERSION;
    isa.bus.find_dev = dev_find;
#if 0    
    isa.bus.delayus = isa_delayus;
    isa.bus.delayms = isa_delayms;
#endif
    switch (isa.bus.endian) {
        case ISA_ENDIAN_BE:
            isa.bus.inp = isa.bus.inp ? isa.bus.inp : inp_be;
            isa.bus.outp = isa.bus.outp ? isa.bus.outp : outp_be;
            isa.bus.inpw = isa.bus.inpw ? isa.bus.inpw : inpw_be;
            isa.bus.outpw = isa.bus.outpw ? isa.bus.outpw : outpw_be;
            break;
        case ISA_ENDIAN_LELS:
            isa.bus.inp = isa.bus.inp ? isa.bus.inp : inp_lels;
            isa.bus.outp = isa.bus.outp ? isa.bus.outp : outp_lels;
            isa.bus.inpw = isa.bus.inpw ? isa.bus.inpw : inpw_lels;
            isa.bus.outpw = isa.bus.outpw ? isa.bus.outpw : outpw_lels;
            break;
        case ISA_ENDIAN_LEAS:
            isa.bus.inp = isa.bus.inp ? isa.bus.inp : inp_leas;
            isa.bus.outp = isa.bus.outp ? isa.bus.outp : outp_leas;
            isa.bus.inpw = isa.bus.inpw ? isa.bus.inpw : inpw_leas;
            isa.bus.outpw = isa.bus.outpw ? isa.bus.outpw : outpw_leas;
            break;
    }

    isa.bus.irq_attach = irq_attach;
    isa.bus.irq_remove = irq_remove;

    /* plug-and-pray */
    pnp_init();

    /* create cookie interface */
    return Createcookie(C__ISA, (uint32_t)&isa.bus);
}

long super_main(void) {

    OpenFiles();
    if (!bus_init() || (isa.bus.iobase == 0)) {
        return 0;
    }

    printf("\r\n");
    printf("ISA I/O: 0x%08lx\r\n", isa.bus.iobase);
    if (isa.bus.membase) {
        printf("ISA MEM: 0x%08lx\r\n", isa.bus.membase);
    }
    printf("\r\n");

    CloseFiles();
    return 1;
}

int main() {
    if (!Supexec(super_main)) {
        return -1;
    }
    ExitTsr();
    return 0;
}
