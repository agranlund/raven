#include <stdio.h>
#include <string.h>
#include "system.h"
#include "settings.h"
#include "fan.h"

//---------------------------------------------------------------
// PWM
//---------------------------------------------------------------
__xdata static uint8_t pwmSpeed = 0;

void InitPwm(void)
{
#if defined(BOARD_RAVEN_A2)
	P2 &= ~bPWM2;
    P2_DIR |= ~bPWM2;

    PWM_CK_SE =  20;            // divide to ~25khz (from 46mhz with period 100)
    PWM_CYCLE = 100;            // period
    PWM_DATA2 =   0;            // duty cycle

    PWM_CTRL &= ~bPWM_CLR_ALL;  // empty fifo
    PWM_CTRL &= ~bPWM_MOD_MFM;	// standard pwm mode
    PWM_CTRL &= ~bPWM2_OUT_EN;  // PWM2 output enabled
    PWM_CTRL &= ~bPWM2_POLAR;   // set polarity
#endif	
}

void ProcessPwm(void)
{
}

void pwm_SetSpeed(uint8_t speed)
{
#if defined(BOARD_RAVEN_A2)
	if (speed == 0) {
		PWM_DATA2 = 0;
		PWM_CTRL &= ~bPWM2_OUT_EN;
	} else {
		if (speed == 255) {
			PWM_DATA2 = 100;
		} else {
			PWM_DATA2 = (uint8_t)(((unsigned int)speed * 20) / 51);
		}
		PWM_CTRL |= bPWM2_OUT_EN;
	}
#else
	if (speed > 0) {
		P3_DIR &= ~(1 << 5);
	} else {
		P3_DIR |= (1 << 5);
	}
#endif
	pwmSpeed = speed;
}

uint8_t pwm_GetSpeed(void)
{
	return pwmSpeed;
}


//---------------------------------------------------------------
// ARGB
//---------------------------------------------------------------
#if defined(BOARD_RAVEN_A2)

__xdata static uint8_t argbData[ARGB_MAXLEDS*3];
__xdata static int16_t argbChanged;
__xdata static int16_t argbLocked;

#define ARGB_PIN    4
#define ARGB_PORT   0xB0
SBIT(ARGB, ARGB_PORT, ARGB_PIN);

uint8_t* argb_Lock(uint8_t idx) {
	if ((argbLocked >= 0) || (idx >= ARGB_MAXLEDS)) {
		return 0;
	}
	argbLocked = idx;
	return &argbData[idx*3];
}
void argb_Unlock(uint8_t num) {
	if (argbLocked >= 0) {
		int start = argbLocked;
		int amount = (int)num;
		int end = start + amount;

		if (end > ARGB_MAXLEDS) {
			end = ARGB_MAXLEDS;
			amount = (ARGB_MAXLEDS - start);
		}

		if (end > argbChanged) {
			argbChanged = end;
		}

		argbLocked = -1;
	}
}

void InitArgb(void)
{
	memset(argbData, 0, sizeof(argbData));
	argbChanged = 0;
	argbLocked = -1;
	P3_DIR |= (1 << ARGB_PIN);

	argb_SetColor(
		0,
		ARGB_MAXLEDS,
		(uint8_t) (((uint16_t)Settings.FanRgbI * Settings.FanRgbR) >> 8),
		(uint8_t) (((uint16_t)Settings.FanRgbI * Settings.FanRgbG) >> 8),
		(uint8_t) (((uint16_t)Settings.FanRgbI * Settings.FanRgbB) >> 8)
	);
	ProcessArgb();
}

void argb_SetColor(uint8_t idx, uint8_t num, uint8_t r, uint8_t g, uint8_t b)
{
	int start = (int)idx;
	int amount = (int)num;
	int end = start + amount;

	if (end > ARGB_MAXLEDS) {
		end = ARGB_MAXLEDS;
		amount = (ARGB_MAXLEDS - start);
	}

	if (end > argbChanged) {
		argbChanged = end;
	}

	uint8_t* dst = &argbData[start*3];
	for (int i=0; i<amount; i++) {
		*dst++ = g;
		*dst++ = r;
		*dst++ = b;
	}
}

void argb_Fill(uint8_t r, uint8_t g, uint8_t b)
{
	argb_SetColor(0, ARGB_MAXLEDS, r, g, b);
}

void argb_SetData(uint8_t idx, uint8_t num, uint8_t* data)
{
	int start = (int)idx;
	int amount = (int)num;
	int end = start + amount;

	if (end > ARGB_MAXLEDS) {
		end = ARGB_MAXLEDS;
		amount = (ARGB_MAXLEDS - start);
	}

	if (end > argbChanged) {
		argbChanged = end;
	}

	uint8_t* dst = &argbData[start*3];
	for (int i=0; i<amount; i++) {
		*dst++ = data[1]; // g
		*dst++ = data[0]; // r
		*dst++ = data[2]; // b
		data += 3;
	}
}


static void argb_Transmit(uint8_t num)
{
	// 48mhz -- 0.02084 us per tick
	// 0: 0.35us high, 0.80us low
	// 1: 0.70us high, 0.60us low 
	// all values += 150ns

	__asm

  	mov  r2,dpl             ; r2 = num
    mov	dptr,#_argbData     ; dptr = data

00001$:
    movx  a,@dptr       ; a = *dptr
    inc   dptr          ; dptr++
    mov   r3,#8         ; shift out 8 bits

00002$:
    setb  _ARGB         ; start bit

	; 17 cycles high
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    rlc  a              ; shift out msb
    mov  _ARGB,c        ; set pin according to carry

	; 17 cycles high or low
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    clr  _ARGB          ; end bit

	; 17 cycles low
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    djnz r3,00002$	; 3 cycles + 1 cycle at start of loop

    djnz r2,00001$

    __endasm;
}

void ProcessArgb(void)
{
	if ((argbChanged > 0) && (argbLocked < 0))
	{
		EA = 0;
		argb_Transmit(argbChanged*3);
		EA = 1;
		argbChanged = 0;
	}
}

#endif // BOARD_RAVEN_A2
