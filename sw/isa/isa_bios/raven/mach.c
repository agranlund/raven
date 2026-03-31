#include <stdbool.h>
#include <stdint.h>
#include <mint/cookie.h>
#include "isa_bios.h"

#define IRQ_OPT_VECS 1

/*-----------------------------------------------------------------------------------
 * Raven specific interrupt vectors
 *---------------------------------------------------------------------------------*/
extern void irqvec_isa02_mfp2B0(void);
extern void irqvec_isa03_mfp2B1(void);
extern void irqvec_isa04_mfp2B2(void);
extern void irqvec_isa05_mfp2B3(void);
extern void irqvec_isa07_mfp2B6(void);
extern void irqvec_isa10_mfp2B7(void);
extern void irqvec_isa11_mfp2A6(void);
extern void irqvec_isa14_mfp2A7(void);

#if IRQ_OPT_VECS
extern void irqvec_isa02_mfp2B0_0(void); extern void irqvec_isa02_mfp2B0_1(void); extern void irqvec_isa02_mfp2B0_2(void); extern void irqvec_isa02_mfp2B0_3(void); extern void irqvec_isa02_mfp2B0_4(void); extern void irqvec_isa02_mfp2B0_5(void); extern void irqvec_isa02_mfp2B0_6(void);
extern void irqvec_isa03_mfp2B1_0(void); extern void irqvec_isa03_mfp2B1_1(void); extern void irqvec_isa03_mfp2B1_2(void); extern void irqvec_isa03_mfp2B1_3(void); extern void irqvec_isa03_mfp2B1_4(void); extern void irqvec_isa03_mfp2B1_5(void); extern void irqvec_isa03_mfp2B1_6(void);
extern void irqvec_isa04_mfp2B2_0(void); extern void irqvec_isa04_mfp2B2_1(void); extern void irqvec_isa04_mfp2B2_2(void); extern void irqvec_isa04_mfp2B2_3(void); extern void irqvec_isa04_mfp2B2_4(void); extern void irqvec_isa04_mfp2B2_5(void); extern void irqvec_isa04_mfp2B2_6(void);
extern void irqvec_isa05_mfp2B3_0(void); extern void irqvec_isa05_mfp2B3_1(void); extern void irqvec_isa05_mfp2B3_2(void); extern void irqvec_isa05_mfp2B3_3(void); extern void irqvec_isa05_mfp2B3_4(void); extern void irqvec_isa05_mfp2B3_5(void); extern void irqvec_isa05_mfp2B3_6(void);
extern void irqvec_isa07_mfp2B6_0(void); extern void irqvec_isa07_mfp2B6_1(void); extern void irqvec_isa07_mfp2B6_2(void); extern void irqvec_isa07_mfp2B6_3(void); extern void irqvec_isa07_mfp2B6_4(void); extern void irqvec_isa07_mfp2B6_5(void); extern void irqvec_isa07_mfp2B6_6(void);
extern void irqvec_isa10_mfp2B7_0(void); extern void irqvec_isa10_mfp2B7_1(void); extern void irqvec_isa10_mfp2B7_2(void); extern void irqvec_isa10_mfp2B7_3(void); extern void irqvec_isa10_mfp2B7_4(void); extern void irqvec_isa10_mfp2B7_5(void); extern void irqvec_isa10_mfp2B7_6(void);
extern void irqvec_isa11_mfp2A6_0(void); extern void irqvec_isa11_mfp2A6_1(void); extern void irqvec_isa11_mfp2A6_2(void); extern void irqvec_isa11_mfp2A6_3(void); extern void irqvec_isa11_mfp2A6_4(void); extern void irqvec_isa11_mfp2A6_5(void); extern void irqvec_isa11_mfp2A6_6(void);
extern void irqvec_isa14_mfp2A7_0(void); extern void irqvec_isa14_mfp2A7_1(void); extern void irqvec_isa14_mfp2A7_2(void); extern void irqvec_isa14_mfp2A7_3(void); extern void irqvec_isa14_mfp2A7_4(void); extern void irqvec_isa14_mfp2A7_5(void); extern void irqvec_isa14_mfp2A7_6(void);
#endif

void raven_isa_irq_assign(void) {
    volatile uint32_t* mfpvecs = (volatile uint32_t*)0x140;
    switch ((uint16_t)(isa_irq_list[2].count)) {
        #if IRQ_OPT_VECS
        case 0:     mfpvecs[0] = (uint32_t)irqvec_isa02_mfp2B0_0; break;
        case 1:     mfpvecs[0] = (uint32_t)irqvec_isa02_mfp2B0_1; break;
        case 2:     mfpvecs[0] = (uint32_t)irqvec_isa02_mfp2B0_2; break;
        case 3:     mfpvecs[0] = (uint32_t)irqvec_isa02_mfp2B0_3; break;
        case 4:     mfpvecs[0] = (uint32_t)irqvec_isa02_mfp2B0_4; break;
        case 5:     mfpvecs[0] = (uint32_t)irqvec_isa02_mfp2B0_5; break;
        #endif
        default:    mfpvecs[0] = (uint32_t)irqvec_isa02_mfp2B0;   break;
    }
    switch ((uint16_t)(isa_irq_list[3].count)) {
        #if IRQ_OPT_VECS
        case 0:     mfpvecs[1] = (uint32_t)irqvec_isa03_mfp2B1_0; break;
        case 1:     mfpvecs[1] = (uint32_t)irqvec_isa03_mfp2B1_1; break;
        case 2:     mfpvecs[1] = (uint32_t)irqvec_isa03_mfp2B1_2; break;
        case 3:     mfpvecs[1] = (uint32_t)irqvec_isa03_mfp2B1_3; break;
        case 4:     mfpvecs[1] = (uint32_t)irqvec_isa03_mfp2B1_4; break;
        case 5:     mfpvecs[1] = (uint32_t)irqvec_isa03_mfp2B1_5; break;
        #endif
        default:    mfpvecs[1] = (uint32_t)irqvec_isa03_mfp2B1;   break;
    }
    switch ((uint16_t)(isa_irq_list[4].count)) {
        #if IRQ_OPT_VECS
        case 0:     mfpvecs[2] = (uint32_t)irqvec_isa04_mfp2B2_0; break;
        case 1:     mfpvecs[2] = (uint32_t)irqvec_isa04_mfp2B2_1; break;
        case 2:     mfpvecs[2] = (uint32_t)irqvec_isa04_mfp2B2_2; break;
        case 3:     mfpvecs[2] = (uint32_t)irqvec_isa04_mfp2B2_3; break;
        case 4:     mfpvecs[2] = (uint32_t)irqvec_isa04_mfp2B2_4; break;
        case 5:     mfpvecs[2] = (uint32_t)irqvec_isa04_mfp2B2_5; break;
        #endif
        default:    mfpvecs[2] = (uint32_t)irqvec_isa04_mfp2B2;   break;
    }
    switch ((uint16_t)(isa_irq_list[5].count)) {
        #if IRQ_OPT_VECS
        case 0:     mfpvecs[3] = (uint32_t)irqvec_isa05_mfp2B3_0; break;
        case 1:     mfpvecs[3] = (uint32_t)irqvec_isa05_mfp2B3_1; break;
        case 2:     mfpvecs[3] = (uint32_t)irqvec_isa05_mfp2B3_2; break;
        case 3:     mfpvecs[3] = (uint32_t)irqvec_isa05_mfp2B3_3; break;
        case 4:     mfpvecs[3] = (uint32_t)irqvec_isa05_mfp2B3_4; break;
        case 5:     mfpvecs[3] = (uint32_t)irqvec_isa05_mfp2B3_5; break;
        #endif
        default:    mfpvecs[3] = (uint32_t)irqvec_isa05_mfp2B3;   break;
    }
    switch ((uint16_t)(isa_irq_list[7].count)) {
        #if IRQ_OPT_VECS
        case 0:     mfpvecs[6] = (uint32_t)irqvec_isa07_mfp2B6_0; break;
        case 1:     mfpvecs[6] = (uint32_t)irqvec_isa07_mfp2B6_1; break;
        case 2:     mfpvecs[6] = (uint32_t)irqvec_isa07_mfp2B6_2; break;
        case 3:     mfpvecs[6] = (uint32_t)irqvec_isa07_mfp2B6_3; break;
        case 4:     mfpvecs[6] = (uint32_t)irqvec_isa07_mfp2B6_4; break;
        case 5:     mfpvecs[6] = (uint32_t)irqvec_isa07_mfp2B6_5; break;
        #endif
        default:    mfpvecs[6] = (uint32_t)irqvec_isa07_mfp2B6;   break;
    }
    switch ((uint16_t)(isa_irq_list[10].count)) {
        #if IRQ_OPT_VECS
        case 0:     mfpvecs[7] = (uint32_t)irqvec_isa10_mfp2B7_0; break;
        case 1:     mfpvecs[7] = (uint32_t)irqvec_isa10_mfp2B7_1; break;
        case 2:     mfpvecs[7] = (uint32_t)irqvec_isa10_mfp2B7_2; break;
        case 3:     mfpvecs[7] = (uint32_t)irqvec_isa10_mfp2B7_3; break;
        case 4:     mfpvecs[7] = (uint32_t)irqvec_isa10_mfp2B7_4; break;
        case 5:     mfpvecs[7] = (uint32_t)irqvec_isa10_mfp2B7_5; break;
        #endif
        default:    mfpvecs[7] = (uint32_t)irqvec_isa10_mfp2B7;   break;
    }
    switch ((uint16_t)(isa_irq_list[11].count)) {
        #if IRQ_OPT_VECS
        case 0:     mfpvecs[14] = (uint32_t)irqvec_isa11_mfp2A6_0; break;
        case 1:     mfpvecs[14] = (uint32_t)irqvec_isa11_mfp2A6_1; break;
        case 2:     mfpvecs[14] = (uint32_t)irqvec_isa11_mfp2A6_2; break;
        case 3:     mfpvecs[14] = (uint32_t)irqvec_isa11_mfp2A6_3; break;
        case 4:     mfpvecs[14] = (uint32_t)irqvec_isa11_mfp2A6_4; break;
        case 5:     mfpvecs[14] = (uint32_t)irqvec_isa11_mfp2A6_5; break;
        #endif
        default:    mfpvecs[14] = (uint32_t)irqvec_isa11_mfp2A6;   break;
    }
    switch ((uint16_t)(isa_irq_list[14].count)) {
        #if IRQ_OPT_VECS
        case 0:     mfpvecs[15] = (uint32_t)irqvec_isa14_mfp2A7_0; break;
        case 1:     mfpvecs[15] = (uint32_t)irqvec_isa14_mfp2A7_1; break;
        case 2:     mfpvecs[15] = (uint32_t)irqvec_isa14_mfp2A7_2; break;
        case 3:     mfpvecs[15] = (uint32_t)irqvec_isa14_mfp2A7_3; break;
        case 4:     mfpvecs[15] = (uint32_t)irqvec_isa14_mfp2A7_4; break;
        case 5:     mfpvecs[15] = (uint32_t)irqvec_isa14_mfp2A7_5; break;
        #endif
        default:    mfpvecs[15] = (uint32_t)irqvec_isa14_mfp2A7;   break;
    }
}

void raven_irq_setup(void) {
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

    /* disable interrupts */
    mfpbase[0x07] &= ~ia_mask;
    mfpbase[0x09] &= ~ib_mask;

    /* initialize interrupt vectors */
    isa_irq_assign = raven_isa_irq_assign;
    raven_isa_irq_assign();

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


void raven_bus_setup(void) {
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
}

bool isabios_setup_raven(void) {
#if ISABIOS_STANDALONE
    uint32_t cookie;
    if (Getcookie(C_RAVN, (long*)&cookie) != C_FOUND) {
        return false;
    }
#endif    
    raven_bus_setup();
    raven_irq_setup();
    return true;
}
