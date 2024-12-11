/*
 * Raven selftest
 *
 * todo:
 * - rtc / i2c
 * - mfp timer interrupt
 * - uart gpio
 * - eiffel
  * - atari parallel port
 * - atari serial port
 * - ramtest
 * - romtest    
 * 
 */

#include <lib.h>
#include "sys.h"
#include "hw/mfp.h"

static void _safe_delay(uint32_t c) { for (uint32_t i = 0; i < (c * 5000); i++) { nop(); } }
static void delay_long() { _safe_delay(1000); }

static int globalresult = 0;
#define dotest(name, f, ...) { \
    puts(" " name "..."); \
    int tresult = f(__VA_ARGS__); \
    globalresult |= tresult; \
    if (tresult == 0)  { puts("  OK"); } \
    else { puts("  FAIL"); } \
}

void putsn(const char *str) { while (*str != '\0') { putchar(*str++); } }

void puts_bitresult(int bits, uint32_t expect, uint32_t result) {
    putsn("   expect: "); for (uint32_t m = (1 << (bits - 1)), c = 0; c < bits; c++, m >>= 1) {putchar((expect & m) ? '1' : '0'); } putchar('\n');
    putsn("   result: "); for (uint32_t m = (1 << (bits - 1)), c = 0; c < bits; c++, m >>= 1) {putchar((result & m) ? '1' : '0'); } putchar('\n');
    putsn("   badbit: "); for (uint32_t m = (1 << (bits - 1)), c = 0; c < bits; c++, m >>= 1) {putchar(((result & m) == (expect & m)) ? '.' : 'x'); } putchar('\n');
}

/*
 * MFP register read/write
 *  Basic MFP and partial PD test
 */
int test_mfp_rw(int verbose, int mfpnum) {
    int result = 0;
    uint32_t mfp = (mfpnum == 0) ? RV_PADDR_MFP1 : RV_PADDR_MFP2;
    uint8_t offs[2] = { 0x13, 0x15 };

    for (int i=0; i<1; i++) {
        uint8_t reg = offs[i];
        uint8_t rold = IOB(mfp, reg);
        IOB(mfp, reg) = 0x55; uint8_t rb0 = IOB(mfp, reg);
        IOB(mfp, reg) = 0xAA; uint8_t rb1 = IOB(mfp, reg);
        IOB(mfp, reg) = rold;

        if ((rb0 != 0x55) || (rb1 != 0xaa)) {
            fmt("  R/W Error on $%l\n", mfp+reg);
            puts_bitresult(8, 0x55, rb0);
            puts_bitresult(8, 0xaa, rb1);
            putsn("  Hint: ");
            if (mfpnum == 0) {
                if (i==0) { puts("U403[PD23:16], U103[D7:0]"); }
                else { puts("U403[PD23:16], U106[D23:16]"); }
            } else {
                if (i==0) { puts("U401[PD23:16], U103[D7:0]"); }
                else { puts("U401[PD23:16], U106[D23:16]"); }
            }
            result |= (1 << i);
        } 
    }
    return result;
}

/*
 * YM2149 register read/write
 *  Basic YM and partial PD test
 */
int test_ym(int verbose) {
    static const unsigned char data[14*2] = {
        0, 0x34, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0xFE,
        8, 0x10, 9, 0, 10, 0, 11, 0, 12, 16, 13, 9,
    };

    volatile uint8_t* ym2149b = (volatile uint8_t*)RV_PADDR_YM;
    volatile uint32_t* ym2149l = (volatile uint32_t*)RV_PADDR_YM;

    // write + readback test
    ym2149b[0] = 0;         // cha freq low
    volatile uint8_t rold = ym2149b[0];
    ym2149b[2] = 0x55; volatile uint8_t rb0 = ym2149b[0];
    ym2149b[2] = 0xaa; volatile uint8_t rb1 = ym2149b[0];
    ym2149b[2] = rold;
    if (rb0 != 0x55 || rb1 != 0xaa) {
        fmt("  R/W Error on $%l/2\n", RV_PADDR_YM);
        puts_bitresult(8, 0x55, rb0);
        puts_bitresult(8, 0xaa, rb1);
        puts("  Hint: U405[31:24], U402, U106[D31:24], U103[D15:8]");
    }

    // output sound, byte access
    puts("  Byte writes (you should hear a bell sound)");
    for (int i=0; i<14*2; i+=2) {
        ym2149b[0] = data[i+0];
        ym2149b[2] = data[i+1];
    }
    _safe_delay(1000);

    // output sound, long access
    puts("  Long writes (you should hear a bell sound)");
    for (int i=0; i<14; i++) {
        unsigned int d0 = 0xFF & data[(i<<1)+0];
        unsigned int d1 = 0xFF & data[(i<<1)+1];
        unsigned int d = (d0 << 24) | (d1 << 8);
        *ym2149l = d;
    }
    _safe_delay(1000);

    return 0;    
}

void main(void)
{
    int verbose = 1;
    puts("\nRaven selftest");
    dotest("YM2149", test_ym, verbose);
    dotest("MFP1:R/W", test_mfp_rw, verbose, 0);
    dotest("MFP2:R/W", test_mfp_rw, verbose, 1);

    if (globalresult != 0) {
        puts("\n** Result: FAIL, see log for details");
    } else {
        puts("\n** Result: OK\n");
    }
}

