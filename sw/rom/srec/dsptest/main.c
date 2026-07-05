/*
 * Raven DSP56303 test
 */

#include <lib.h>
#include "sys.h"
#include "hw/mfp.h"

extern unsigned char EmbededP56_dspboot[];
#if 1
static unsigned char* dsp_prog = EmbededP56_dspboot;
#else
static uint8_t dsp_prog[] = { 
/*
	org p:$0
start:
	bset	#3,x:$ffffc2
loop:
	bchg	#4,x:$ffffc2
	rep	#$fff
	nop
	jmp	loop
*/
	0x00,0x00,0x00,	/* space  */
	0x00,0x00,0x00,	/* offset */
	0x00,0x00,0x08,	/* length */
	0x0a,0x70,0x23, 0xff,0xff,0xc2, 0x0b,0x70,0x04, 0xff,0xff,0xc2,
	0x06,0xff,0xaf, 0x00,0x00,0x00, 0x0a,0xf0,0x80, 0x00,0x00,0x02,
	0x0,0x0,0x3		/* end of file */
};
#endif

#define DSP_DELAY_ONOFF			100
#define DSP_DELAY_SEND_BOOT		1

static void delay(uint32_t c) { for (uint32_t i = 0; i < (c * 5000); i++) { nop(); } }

static void dsp_on(void) {
	*((volatile uint8_t*)(RV_PADDR_UART1 + 0x10)) |= (1<<1);
	delay(DSP_DELAY_ONOFF);
}

static void dsp_off(void) {
	*((volatile uint8_t*)(RV_PADDR_UART1 + 0x10)) &= ~(1<<1);
	delay(DSP_DELAY_ONOFF);
}

static void dsp_send_slow(uint8_t* b, uint32_t len) {
	volatile uint8_t* dsp = (volatile uint8_t*)(RV_PADDR_DSP);
	for (; len; len--) {
		delay(DSP_DELAY_SEND_BOOT);
		while ((dsp[0x08] & (1 << 1)) == 0) {
			delay(DSP_DELAY_SEND_BOOT);
		}
		dsp[0x14] = *b++;
		delay(DSP_DELAY_SEND_BOOT);
		dsp[0x18] = *b++;
		delay(DSP_DELAY_SEND_BOOT);
		dsp[0x1C] = *b++;
	}
}

void main(void)
{
    puts("\nDSP56303 test");

	volatile uint8_t* dsp = (volatile uint8_t*)RV_PADDR_DSP;

	printf("DSP OFF\n");
	dsp_off();
	uint8_t cvr_expected = 0xff;
	uint8_t isr_expected = 0xff;
	uint8_t ivr_expected = 0xff;

	uint8_t cvr = dsp[0x4];
	uint8_t isr = dsp[0x8];
	uint8_t ivr = dsp[0xc];
	printf("CVR=%02x ISR=%02x IVR=%02x\n", cvr, isr, ivr);
	if ((cvr != cvr_expected) || (isr != isr_expected) || (ivr != ivr_expected)) {
		printf("fault. expected values:\n");
		printf("CVR=%02x ISR=%02x IVR=%02x\n", cvr_expected, isr_expected, ivr_expected);
		goto done;
	}

	printf("DSP ON\n");
	dsp_on();
	cvr = dsp[0x4];
	isr = dsp[0x8];
	ivr = dsp[0xc];
	cvr_expected = 0x32;
	isr_expected = 0x06;
	ivr_expected = 0x0f;

	printf("CVR=%02x ISR=%02x IVR=%02x\n", cvr, isr, ivr);

	if ((cvr != cvr_expected) || (isr != isr_expected) || (ivr != ivr_expected)) {
		printf("fault. expected values:\n");
		printf("CVR=%02x ISR=%02x IVR=%02x\n", cvr_expected, isr_expected, ivr_expected);
		goto done;
	}

	uint32_t len = ((((uint32_t)dsp_prog[6]) << 16) | (((uint32_t)dsp_prog[7]) << 8) | (((uint32_t)dsp_prog[8]) << 0));
	uint32_t off = ((((uint32_t)dsp_prog[3]) << 16) | (((uint32_t)dsp_prog[4]) << 8) | (((uint32_t)dsp_prog[5]) << 0));
	printf("DSP LOAD off:$%06x len:$%06x\n", off, len);

	dsp_send_slow(&dsp_prog[6], 1);		// len
	dsp_send_slow(&dsp_prog[3], 1);		// offset
	dsp_send_slow(&dsp_prog[9], len);	// data
	delay(100);

	printf("DSP RUN\n");
	cvr = dsp[0x4];
	isr = dsp[0x8];
	ivr = dsp[0xc];
	printf("CVR=%02x ISR=%02x IVR=%02x\n", cvr, isr, ivr);

	bool wait_hf2_ok = false;
	for (int i=0; i<8192 && !wait_hf2_ok; i++) {
		delay(10);
		isr = dsp[0x8];
		wait_hf2_ok = (isr & (1 << 3)) ? true : false;
	}
	if (!wait_hf2_ok) {
		printf("fault. expected HF2 (ISR bit 3) set\n");
		goto done;
	}

	int32_t flips = 0;
	for (int j=0; j<16; j++) {
		delay(j);
		for (int i=0; i<256; i++) {
			uint8_t isr_now = dsp[0x8];
			if (isr_now != isr) {
				flips++;
				isr = isr_now;
				//printf("HF2=%d HF3=%d\n", (isr_now & (1 << 3)) ? 1 : 0, (isr_now & (1 << 4)) ? 1 : 0);
			}
		}
	}

	if (flips == 0) {
		printf("fault. expected HF3 (ISR bit 4) toggle\n");
		cvr = dsp[0x4];
		isr = dsp[0x8];
		ivr = dsp[0xc];
		printf("CVR=%02x ISR=%02x IVR=%02x\n", cvr, isr, ivr);
		goto done;
	}

	printf("ok %d.\n", flips);

done:
	dsp_off();
}

