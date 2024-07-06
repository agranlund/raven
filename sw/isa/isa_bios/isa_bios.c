#include "isa_core.h"
#include "stdio.h"
#include "string.h"
#include "mint/basepage.h"
#include "mint/cookie.h"
#include "isa_rw.h"
#include "isa_pnp.h"

isa_core_t  isa;

//-----------------------------------------------------------------------------------
// interrupts
//  todo: implement and move these to a separate isa_irq.c file
//-----------------------------------------------------------------------------------
unsigned int irq_set_hades(unsigned char irq, unsigned int funct) {
    return 0;
}
unsigned char irq_en_hades(unsigned char irq, unsigned char enabled) {
    return 0;
}

unsigned int irq_set_milan(unsigned char irq, unsigned int funct) {
    return 0;
}
unsigned char irq_en_milan(unsigned char irq, unsigned char enabled) {
    return 0;
}

unsigned int irq_set_panther(unsigned char irq, unsigned int funct) {
    return 0;
}
unsigned char irq_en_panther(unsigned char irq, unsigned char enabled) {
    return 0;
}

unsigned int irq_set_raven(unsigned char irq, unsigned int funct) {
    return 0;
}
unsigned char irq_en_raven(unsigned char irq, unsigned char enabled) {
    return 0;
}

//-----------------------------------------------------------------------------------
// device helpers
//-----------------------------------------------------------------------------------
isa_dev_t* dev_find(const char* idstr, unsigned short idx) {
    uint32 id = StrToId(idstr);
    unsigned short found = 0;
    for (unsigned short i=0; i<isa.bus.numdevs; i++) {
        for (int j=0; j<ISA_MAX_DEV_IDS && (isa.bus.devs[i].id[j] != 0); j++) {
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


//-----------------------------------------------------------------------------------
//
// Hardware specific bus config
//
//-----------------------------------------------------------------------------------
static bool bus_hwconf() {
    isa.bus.endian      = ISA_ENDIAN_BE;
    isa.bus.irqmask     = 0xFFFF;
    isa.bus.drqmask     = 0x0F;

    long cookie = 0;
    if (Getcookie(C_hade, &cookie) == C_FOUND)            // Hades
    {
        isa.bus.endian = ISA_ENDIAN_LELS;
        isa.bus.iobase  = 0xFFF30000;
        isa.bus.membase = 0xFF000000;
    }
    else if (Getcookie(C__MIL, &cookie) == C_FOUND)       // Milan
    {
        isa.bus.endian = ISA_ENDIAN_LELS;
        isa.bus.iobase = 0xC0000000;
        //isa.bus.endian = ISA_ENDIAN_LEAS;
        //isa.bus.iobase = 0x80000000;
    }
    else if (Getcookie(C__P2I, &cookie) == C_FOUND)       // Panther/2
    {
        uint32* cardpth2 = (uint32*) (cookie+6);
        isa.bus.endian = ISA_ENDIAN_LELS;
        isa.bus.iobase  = *cardpth2;
        cardpth2 += 2;
        isa.bus.membase = *cardpth2;
    }    
    else if (Getcookie(C_RAVN, &cookie) == C_FOUND)       // Raven
    {
        isa.bus.endian = ISA_ENDIAN_LELS;
        isa.bus.irqmask = 0x4CF0;       // 14,11,10,7,5,4,3 (2/9)
        isa.bus.iobase  = 0x81000000;
        isa.bus.membase = 0x82000000;
        //isa.bus.irq_set = irq_set_raven;
        //isa.bus.irq_en  = irq_en_raven;
    }
    return true;
}

bool bus_init()
{
    memset((void*)&isa, 0, sizeof(isa_core_t));

    // hardware autodetect
    if (!bus_hwconf())
        return false;

    // bus settings from config file
    uint32 cfgint;
    const char* cfgstr = GetInfStr("isa.endian");
    if (cfgstr && strcmp(cfgstr, "be") == 0)    isa.bus.endian = ISA_ENDIAN_BE;
    if (cfgstr && strcmp(cfgstr, "leas") == 0)  isa.bus.endian = ISA_ENDIAN_LEAS;
    if (cfgstr && strcmp(cfgstr, "lels") == 0)  isa.bus.endian = ISA_ENDIAN_LELS;
    if (GetInfHex("isa.iobase", &cfgint))       isa.bus.iobase = cfgint;
    if (GetInfHex("isa.membase", &cfgint))      isa.bus.membase = cfgint;
    if (GetInfHex("isa.irqmask", &cfgint))      isa.bus.irqmask = cfgint;
    if (GetInfHex("isa.drqmask", &cfgint))      isa.bus.drqmask = cfgint;

    // check if we have minimum settings
    if (isa.bus.iobase == 0)
        return false;

    // common setup
    isa.bus.version = ISA_BIOS_VERSION;
    isa.bus.find_dev = dev_find;
    switch (isa.bus.endian) {
        case ISA_ENDIAN_BE:
            isa.bus.inp = isa.bus.inp ? isa.bus.inp : inp_be;
            isa.bus.outp = isa.bus.outp ? isa.bus.outp : outp_be;
            isa.bus.inpw = isa.bus.inpw ? isa.bus.inpw : inpw_be;
            isa.bus.outpw = isa.bus.outpw ? isa.bus.outpw : outpw_be;
            isa.bus.inp_buf = isa.bus.inp_buf ? isa.bus.inp_buf : inp_be_buf;
            isa.bus.outp_buf = isa.bus.outp_buf ? isa.bus.outp_buf : outp_be_buf;
            isa.bus.inpw_buf = isa.bus.inpw_buf ? isa.bus.inpw_buf : inpw_be_buf; 
            isa.bus.outpw_buf = isa.bus.outpw_buf ? isa.bus.outpw_buf : outpw_be_buf;
            break;
        case ISA_ENDIAN_LELS:
            isa.bus.inp = isa.bus.inp ? isa.bus.inp : inp_lels;
            isa.bus.outp = isa.bus.outp ? isa.bus.outp : outp_lels;
            isa.bus.inpw = isa.bus.inpw ? isa.bus.inpw : inpw_lels;
            isa.bus.outpw = isa.bus.outpw ? isa.bus.outpw : outpw_lels;
            isa.bus.inp_buf = isa.bus.inp_buf ? isa.bus.inp_buf : inp_lels_buf;
            isa.bus.outp_buf = isa.bus.outp_buf ? isa.bus.outp_buf : outp_lels_buf;
            isa.bus.inpw_buf = isa.bus.inpw_buf ? isa.bus.inpw_buf : inpw_lels_buf; 
            isa.bus.outpw_buf = isa.bus.outpw_buf ? isa.bus.outpw_buf : outpw_lels_buf;
            break;
        case ISA_ENDIAN_LEAS:
            isa.bus.inp = isa.bus.inp ? isa.bus.inp : inp_leas;
            isa.bus.outp = isa.bus.outp ? isa.bus.outp : outp_leas;
            isa.bus.inpw = isa.bus.inpw ? isa.bus.inpw : inpw_leas;
            isa.bus.outpw = isa.bus.outpw ? isa.bus.outpw : outpw_leas;
            isa.bus.inp_buf = isa.bus.inp_buf ? isa.bus.inp_buf : inp_leas_buf;
            isa.bus.outp_buf = isa.bus.outp_buf ? isa.bus.outp_buf : outp_leas_buf;
            isa.bus.inpw_buf = isa.bus.inpw_buf ? isa.bus.inpw_buf : inpw_leas_buf; 
            isa.bus.outpw_buf = isa.bus.outpw_buf ? isa.bus.outpw_buf : outpw_leas_buf;
            break;
    }

    // plug-and-pray
    pnp_init();

    // create cookie interface
    return Createcookie(C__ISA, (uint32)&isa.bus);
}

int super_main() {
    OpenFiles();
    if (!bus_init() || (isa.bus.iobase == 0)) {
        return 0;
    }

    printf("\r\n");
    printf("ISA I/O: 0x%08x\r\n", isa.bus.iobase);
    if (isa.bus.membase) {
        printf("ISA MEM: 0x%08x\r\n", isa.bus.membase);
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
