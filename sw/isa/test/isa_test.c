#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../isa.h"

isa_t* bus;

static volatile uint32_t counter = 0;
void irqfun(void) {
    counter++;
}

static void nop(void) 0x4e71;

void some_delay(void) {
    uint32_t i, j;
    for (i = 0; i < 1000000; i++) {
        nop(); nop(); nop(); nop(); nop();
        nop(); nop(); nop(); nop(); nop();
    }
}

int main() {
    int32_t num = 0;
    printf("now0: %ld\n", counter);
    bus = isa_init();

    printf("now1: %ld (%08lx)\n", counter, (uint32_t)(bus->irq_attach));
    num = bus->irq_attach(9, (uint32_t)irqfun);
    printf("now2: %ld (%ld)\n", counter, num);

    some_delay();
    printf("now3: %ld\n", counter);

    some_delay();
    some_delay();
    printf("now4: %ld\n", counter);

    return 0;
}