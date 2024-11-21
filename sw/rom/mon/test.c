#include "sys.h"
#include "hw/uart.h"
#include "hw/ikbd.h"
#include "hw/rtc.h"

int test_ym_write(int size)
{
    static const unsigned char data[14*2] = {
        0, 0x34, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0xFE,
        8, 0x10, 9, 0, 10, 0, 11, 0, 12, 16, 13, 9,
    };

    if (size == 1) {
        volatile unsigned char* ym2149 = (volatile unsigned char*)PADDR_YM;
        for (int i=0; i<14*2; i+=2) {
            ym2149[0] = data[i+0];
            ym2149[2] = data[i+1];
        }
    }
    else if (size == 2) {
    }
    else if (size == 4) {
        volatile unsigned int* ym2149 = (volatile unsigned int*)PADDR_YM;
        for (int i=0; i<14; i++) {
            unsigned int d0 = 0xFF & data[(i<<1)+0];
            unsigned int d1 = 0xFF & data[(i<<1)+1];
            unsigned int d = (d0 << 24) | (d1 << 8);
            *ym2149 = d;
        }
    }
    return 0;
}

int test_ym() {
    puts("Testing YM");
    puts("You should hear the same bell sound twice.");
    puts("  Byte access"); test_ym_write(1);
    sys_Delay(2000);
    puts("  Long access"); test_ym_write(4);
    sys_Delay(2000);
    puts("  Done.");
    return 0;
}


int test_ikbd_gpo(uint32_t val) {
    puts("Testing GPO...");
    puts("  PowerLED should blink");
    for (int i=0; i<5; i++) {
        ikbd_GPO(0, 0);
        sys_Delay(500);
        ikbd_GPO(0, 1);
        sys_Delay(500);
    }
    puts("  Done.");
    return 0;
}

int test_echo(uint32_t delay) {
    puts("Testing COM1 echo...");
    puts("Anything received will be echo'ed back.");
    puts("Restart computer to exit test");
    while(1) {
        char c = uart_recv();
        uart_send(c);
        if (delay > 0) {
            sys_Delay(delay);
        }
    }
    return 0;
}

void monTest(char* test, uint32_t val)
{
   if (strcmp(test, "ym") == 0) {
        test_ym();
    }
    else if (strcmp(test, "echo") == 0) {
        test_echo(val);
    }
    else if (strcmp(test, "gpo") == 0) {
        test_ikbd_gpo(val);
    }
    else {
        puts("Test commands:\n"
             "  echo {delay}\n"
             "  gpo\n"
             "  ym" );
    }
}
