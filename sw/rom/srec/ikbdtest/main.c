/*
 * Raven ikbd test
 * for testing/debugging ikbd stuff
 */

#include <lib.h>
#include "sys.h"
#include "hw/uart.h"
#include "hw/ikbd.h"

void main(void)
{
    uint8_t recv[8];
    puts("\n");
    puts("Raven ikbd test");
    puts("Press [ESC] to exit");
    puts("\n");

#if 1
    puts("reset");
    ikbd_send(0x80);
    ikbd_send(0x01);
    // wait for ack, or timeout
    for (uint32_t i = 0; i < (1000 * 1000); i++) {
        if (ikbd_rxrdy()) {
            if (ikbd_recv() == 0xF0) {
                break;
            }
        }
    }
    // wait for silence
    puts("wait for silence");
    for (uint32_t i = 0; i < (1000 * 1000); i++) {
        if (ikbd_rxrdy()) {
            (void)ikbd_recv();
            i = 0;
        }
    }
#endif    

#if 1    
    puts("get info");
    ikbd_send(0x03);
    uint32_t ckbd_version = 0;
    // wait for version, or timeout
    for (uint32_t i = 0; i < (500 * 10000); i++) {
        if (ikbd_rxrdy()) {
            recv[0] = ikbd_recv();
            if (recv[0] == 0xF6) {
                recv[1] = ikbd_recv();
                recv[2] = ikbd_recv();
                recv[3] = ikbd_recv();
                recv[4] = ikbd_recv();
                recv[5] = ikbd_recv();
                recv[6] = ikbd_recv();
                recv[7] = ikbd_recv();
                printf("ikbd: %02x %02x %02x %02x %02x %02x %02x %02x\n", recv[0], recv[1], recv[2], recv[3], recv[4], recv[5], recv[6], recv[7]);
                if (recv[1] == 0x2C) {
                    // got version info
                    ckbd_version = recv[7] | (recv[6] << 8) | (recv[5] << 16) | (recv[4] << 24);
                    break;
                }
            }
        }
    }
    printf("ckbd build: %08x\n", ckbd_version);
#endif

#if 0
    // abs mouse mode
    puts("abs mouse mode");
    ikbd_send(0x09);
    ikbd_send(640 >> 8);
    ikbd_send(640 & 0xff);
    ikbd_send(480 >> 8);
    ikbd_send(480 & 0xff);
#if 0
    // absolute report on button up/down
    ikbd_send(0x07);
    ikbd_send(0x03);
#endif
#endif    

    while(1) {
        while (uart_rxrdy()) {
            recv[0] = uart_recv();
#if 0
            printf("uart: %02x\n", recv[0]);
#endif            
            if (recv[0] == 0x1B) {      // esc = quit
                puts("Bye.");
                return;
            }
            else if (recv[0] == 0x08) { // backspace = ckbd bootloader
                ikbd_send(0x26);
            }
            else if (recv[0] == 0x31) { // 1 = poll mouse
                ikbd_send(0x0D);
            }
            else if (recv[0] == 0x32) { // 2 = poll joystick
                ikbd_send(0x16);
            }
        }

        while (ikbd_rxrdy()) {
            recv[0] = ikbd_recv();

            if (recv[0] == 0xF6) {      // relative mouse
                recv[1] = ikbd_recv();
                recv[2] = ikbd_recv();
                recv[3] = ikbd_recv();
                recv[4] = ikbd_recv();
                recv[5] = ikbd_recv();
                recv[6] = ikbd_recv();
                recv[7] = ikbd_recv();
                printf("ikbd: %02x %02x %02x %02x %02x %02x %02x %02x\n", recv[0], recv[1], recv[2], recv[3], recv[4], recv[5], recv[6], recv[7]);
            }
            else if (recv[0] == 0xF7) { // absolute mouse
                recv[1] = ikbd_recv();
                recv[2] = ikbd_recv();
                recv[3] = ikbd_recv();
                recv[4] = ikbd_recv();
                recv[5] = ikbd_recv();
                printf("ikbd: %02x %02x, %d, %d\n", recv[0], recv[1], (recv[2] << 8) | recv[3], (recv[4] << 8) | recv[5]);
            }
            else
            {
                printf("ikbd: %02x\n", recv[0]);
            }

        }
    }
}
