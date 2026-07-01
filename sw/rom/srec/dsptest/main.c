/*
 * Raven DSP56303 test
 */

#include <lib.h>
#include "sys.h"
#include "hw/mfp.h"

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


static void delay(uint32_t c) { for (uint32_t i = 0; i < (c * 5000); i++) { nop(); } }

static void dsp_on(void) {
	*((volatile uint8_t*)(RV_PADDR_UART1 + 0x10)) |= (1<<1);
	delay(100);
}

static void dsp_off(void) {
	*((volatile uint8_t*)(RV_PADDR_UART1 + 0x10)) &= ~(1<<1);
	delay(100);
}

static void dsp_send(uint8_t* b, uint32_t len) {
	volatile uint8_t* dsp = (volatile uint8_t*)(RV_PADDR_DSP);
	for (; len; len--) {
		while ((dsp[0x08] & (1 << 1)) == 0);
		dsp[0x14] = *b++;
		dsp[0x18] = *b++;
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

	printf("DSP RUN\n");

	uint32_t len = ((((uint32_t)dsp_prog[6]) << 16) | (((uint32_t)dsp_prog[7]) << 8) | (((uint32_t)dsp_prog[8]) << 0));

	dsp_send(&dsp_prog[6], 1);		// len
	dsp_send(&dsp_prog[3], 1);		// offset
	dsp_send(&dsp_prog[9], len);	// data

	delay(10);
	cvr = dsp[0x4];
	isr = dsp[0x8];
	ivr = dsp[0xc];

	if ((isr & (1 << 3)) == 0) {
		printf("fault. expected HF2 (ISR bit 3) set\n");
		goto done;
	}

	int32_t flips = 0;
	for (int i=0; i<128; i++) {
		uint8_t isr_now = dsp[0x8];
		if (isr_now != isr) {
			flips++;
			isr = isr_now;
			printf("HF2=%d HF3=%d\n", (isr_now & (1 << 3)) ? 1 : 0, (isr_now & (1 << 4)) ? 1 : 0);
		}
	}

	if (flips == 0) {
		printf("fault. expected HF3 (ISR bit 4) toggle\n");
		goto done;
	}

	printf("ok.\n");

done:
	dsp_off();
}

