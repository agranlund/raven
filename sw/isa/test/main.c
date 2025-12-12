/* 
 * very simple isa_bios interrupt test
 *
 * assumes OPL3 on 0x388 with IRQ5 and a soundcard that
 * actually triggers hardware interrupts on the OPL timer.
 * 
 * soundcards normally don't support that but the opl3sa
 * does so that's good enough for functionality testing.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define ISA_EXCLUDE_LIB_MSDOS
#include "isa.h"
#include "vga.h"

isa_t* bus;

static void nop(void) 0x4e71;
void some_delay(void) {
    uint32_t i;
    for (i = 0; i < 1000000UL; i++) {
        nop(); nop(); nop(); nop(); nop();
        nop(); nop(); nop(); nop(); nop();
    }
}

static uint16_t opl_port = 0x388;
static void opl_write(uint16_t idx, uint8_t val) {
    bus->outp(opl_port + 0, idx);
    bus->outp(opl_port + 1, val);
}

static volatile uint32_t counter[6];
void irq_test1(void) { counter[0]++; opl_write(0x04, 0x80); }
void irq_test2(void) { counter[1]++; }
void irq_test3(void) { counter[2]++; }
void irq_test4(void) { counter[3]++; }
void irq_test5(void) { counter[4]++; }
void irq_test6(void) { counter[5]++; }
void irq_test_null(void) { }

int main() {
    uint32_t result;
    int i;
    bus = isa_init();

    /* verify isa_bios irq support */
    printf("irq mask           : 0x%04x\n", bus->irqmask);
    for (i=0; i<16; i++ ) {
        result = bus->irq_attach(i, irq_test_null);
        if (result) {
            printf("irq supported      : %d\n", i);
        }
        bus->irq_remove(i, irq_test_null);
    }

    /* irq5 test using opl3-timer2 */
    for (i=0; i<=6; i++) {

        memset((void*)counter, 0, sizeof(counter));
        result = 0;
        if (i >=1 ) { result = bus->irq_attach(5, irq_test1); }
        if (i >=2 ) { result = bus->irq_attach(5, irq_test2); }
        if (i >=3 ) { result = bus->irq_attach(5, irq_test3); }
        if (i >=4 ) { result = bus->irq_attach(5, irq_test4); }
        if (i >=5 ) { result = bus->irq_attach(5, irq_test5); }
        if (i >=6 ) { result = bus->irq_attach(5, irq_test6); }

        printf("irq5 counts [%d][%ld] : ", i, result);

        opl_write(0x04, 0x60);      /* reset timer1+2 */
        opl_write(0x04, 0x80);      /* reset irq */
        opl_write(0x03, 0xff);      /* set timer2 */
        opl_write(0x04, 0x42);      /* start timer2 (~320us period) */
        some_delay();
        opl_write(0x04, 0x60);      /* reset timer1+2 */
        opl_write(0x04, 0x80);      /* reset irq */

        printf("%ld,%ld,%ld,%ld,%ld,%ld\n", counter[0], counter[1], counter[2], counter[3], counter[4],  counter[5]);

        if (i >=1 ) { bus->irq_remove(5, irq_test1); }
        if (i >=2 ) { bus->irq_remove(5, irq_test2); }
        if (i >=3 ) { bus->irq_remove(5, irq_test3); }
        if (i >=4 ) { bus->irq_remove(5, irq_test4); }
        if (i >=5 ) { bus->irq_remove(5, irq_test5); }
        if (i >=6 ) { bus->irq_remove(5, irq_test6); }
    }
    return 0;
}
