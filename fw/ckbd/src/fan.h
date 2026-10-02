#ifndef _FANCTRL_H_
#define _FANCTRL_H_

#if defined(BOARD_RAVEN_A2)

#define ARGB_MAXLEDS	120

// argb
void InitArgb(void);
void ProcessArgb(void);

void argb_Fill(uint8_t r, uint8_t g, uint8_t b);
void argb_SetColor(uint8_t idx, uint8_t num, uint8_t r, uint8_t g, uint8_t b);
void argb_SetData(uint8_t idx, uint8_t num, uint8_t* data);
void argb_Send(void);
uint8_t* argb_Lock(uint8_t idx);
void argb_Unlock(uint8_t num);
#endif // BOARD_RAVEN_A2

// pwm
void InitPwm(void);
void ProcessPwm(void);
void pwm_SetSpeed(uint8_t speed);
uint8_t pwm_GetSpeed(void);

#endif // _FANCTRL_H_
