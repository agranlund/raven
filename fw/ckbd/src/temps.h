#ifndef __TEMPS_H__
#define __TEMPS_H__

#define TEMP_STATUS_ADC0    (1<<0)
#define TEMP_STATUS_FAN0    (1<<1)
#define TEMP_STATUS_ADC1    (1<<2)
#define TEMP_STATUS_FAN1    (1<<3)

#define FANCONTROL_OFF  0
#define FANCONTROL_ON   1
#define FANCONTROL_AUTO 3


extern void InitTemps(void);
extern void ProcessTemps(void);

extern bool GetTemps(uint8_t idx, uint8_t* status_out, uint16_t* adc_out, uint8_t* res_out, uint8_t* deg_out);

#endif /* __TEMPS_H__ */
