//-------------------------------------------------------------------------
// temps.c
// temperature and fan control
//-------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "system.h"
#include "settings.h"
#include "temps.h"

#if defined(DISABLE_TEMPS)

void InitTemps(void) { }
void ProcessTemps(void) { }

#else

#define tempsensor_avgcount 8
#define tempsensor_avgmask  (tempsensor_avgcount - 1)
#define tempsensor_delay    5

static __xdata uint8_t pos;
static __xdata uint8_t status;
static __xdata uint16_t adc[2][tempsensor_avgcount];


static void SetFanState(uint8_t idx, bool enable) {
    if (idx == 0) {
        if (enable && !(status & TEMP_STATUS_FAN0)) {
            status |= TEMP_STATUS_FAN0;
            P3_DIR &= ~(1 << 5);
        } else if (!enable && (status & TEMP_STATUS_FAN0)) {
            status &= ~TEMP_STATUS_FAN0;
            P3_DIR |= (1 << 5);
        }
    }
#if !defined(DISABLE_TEMP2)
    // todo: drive pins
    else if (idx == 1) {
        if (enable && !(status & TEMP_STATUS_FAN1)) {
            status |= TEMP_STATUS_FAN1;
        } else if (!enable && (status & TEMP_STATUS_FAN1)) {
            status &= ~TEMP_STATUS_FAN1;
        }
    }
#endif
}

static uint16_t ReadTempSensorRaw(uint8_t idx) {
    uint16_t t = 0;
#if defined(DISABLE_TEMP2)
    if (idx != 0) {
        return 0;
    }
#endif
    t = ADC_FIFO; t = ADC_FIFO;
    t = ADC_FIFO; t = ADC_FIFO;
    ADC_CHANN = (1 << idx);
    ADC_CTRL |= bADC_SAMPLE;
    delayus(tempsensor_delay);
    ADC_CTRL &= ~bADC_SAMPLE;
    delayus(tempsensor_delay);
    while ((ADC_STAT & bADC_DATA_OK) == 0) {
        delayus(1);
    }
    t = ADC_FIFO;
    return t;
}

static uint16_t ReadTempSensorAvg(int idx) {
    uint32_t ret = 0;
    for (int i=0; i<tempsensor_avgcount; i++) {
        ret += (uint32_t) (adc[idx][i]);
    }
    return (uint16_t) (ret / tempsensor_avgcount);
}

static bool GetBoardTemperature(uint16_t* adc_out, uint8_t* res_out, uint8_t* deg_out) {
    //
    //  Standalone                    Raven.A2
    //
    //     5V                          3V
    //      |                           |
    //      r1 (10k)                    r1 (10k)
    //      |                           |
    //      ------|-----> adc           ------------> adc
    //      |     |                     |
    //      rf    r2 (10k)              rf
    //      |     |                     |
    //    gnd   gnd                   gnd
    //

    // get averaged adc value from pin
    uint16_t adc = ReadTempSensorAvg(0);
    if (adc_out) { *adc_out = adc; }
    if ((adc >= 0x07f0) || (adc < 0x0010)) {
        if (res_out) { *res_out = 0; }
        if (deg_out) { *deg_out = 0; }
        return false;
    }
   
    // convert to voltage
    float vf = (3.3f * (float)adc) / 2048.0f;

    // calculate thermistor resistance
#if defined(BOARD_RAVEN_A2)
    float rf = 10.0f / (3.3f - vf);
#else
    float rf = ((-20.0f * vf) / ((4.0f * vf) - 10.0f));
#endif

    // fit in byte as ohm/100 so that we act like eiffel does
    uint8_t r = (rf < 0.1f ? 0 : rf > 25.0f ? 250 : (uint8_t)(rf * 10.0f));
    if (res_out) { *res_out = r; }

    // approximate temperature in celsius
    if (deg_out) {
        settings_eiffel_temp_t* eiffeltemp = &Settings.EiffelTemp[0];
        *deg_out = eiffeltemp->Temp[11];
        uint8_t r1 = eiffeltemp->Rctn[0];
        uint8_t t1 = eiffeltemp->Temp[0];
        for (int i=1; i<12; i++) {
            uint8_t r0 = r1; r1 = eiffeltemp->Rctn[i];
            uint8_t t0 = t1; t1 = eiffeltemp->Temp[i];
            if (r <= r1) {
                float fd = (float)(r - r0) / (float)(r1 - r0);
                float f0 = (float)t0;
                float f1 = (float)t1;
                *deg_out = (uint8_t) (f0 - (fd * (f0 - f1)));
                break;
            }
        }
    }

    TRACE("t0 %d, rf = %d", (int16_t)(vf*1000), (int16_t)(rf*100));
    TRACE("t0 %x %d %d %d\n", adc, adc_out ? *adc_out : 0, res_out ? *res_out : 0, deg_out ? *deg_out : 0);
    return true;
}

static bool GetCoreTemperature(uint16_t* adc_out, uint8_t* res_out, uint8_t* deg_out) {
    //
    //  Raven.A2
    //
    //      3V
    //      |
    //      r1 (1k)
    //      |
    //      ------------> adc
    //      |
    //      060_therm0
    //      060_therm1
    //      |
    //    gnd
    //

// MC68060DE.pdf
// The nominal resistance and temperature coefficient of the internal die resistor connected
// across the thermal sensing pins (THERM1, THERM0) as listed in the User's Manual (rev. 1)
// on page 2-16 is incorrect. The correct value for the temperature coefficient is approximately
// 2.8 ohms/ degree C. The correct value for the nominal resistance is approximately 780
// ohms at 25 degrees C.


#if defined(DISABLE_TEMP2)
    if (adc_out) { *adc_out = 0; }
    if (res_out) { *res_out = 0; }
    if (deg_out) { *deg_out = 0; }
    return false;
#else
    uint16_t adc = ReadTempSensorAvg(1);
    if (adc_out) { *adc_out = adc; }
    if ((adc >= 0x07f0) || (adc < 0x0010)) {
        if (res_out) { *res_out = 0; }
        if (deg_out) { *deg_out = 0; }
        return false;
    }

    // todo: use user defined calibration settings here,
    // same as we do for board temperature.

    const float rn = 870; //780.0f;
    const float at = 2.8f;

    // convert to voltage
    float vf = (3.3f * (float)adc) / 2048.0f;

    // calculate THERM01 resistance
    float rt = (vf * 1000) / (3.3f - vf);

    // calculate temperature
    float t = 25.0f + ((rt - rn) / at);

    TRACE("adc = %d, vf = %d, rt = %d, t = %d", adc, (int16_t)(vf*1000), (int16_t)rt, (int16_t)t);

    if (res_out) { *res_out = (uint8_t) (rt / 100); }
    if (deg_out) { *deg_out = (uint8_t) t; }
    return true;
#endif    
}


//---------------------------------------------------------------------------------------------

bool GetTemps(uint8_t idx, uint8_t* status_out, uint16_t* adc_out, uint8_t* res_out, uint8_t* deg_out)
{
    bool result;
    if (idx == 0) {
        result = GetBoardTemperature(adc_out, res_out, deg_out);
    } else if (idx == 1) {
        result = GetCoreTemperature(adc_out, res_out, deg_out);
    } else {
        result = false;
        if (adc_out) { *adc_out = 0x7ff; }
        if (res_out) { *res_out = 0; }
        if (deg_out) { *deg_out = 0; }
    }
    if (status_out) {
        *status_out = status;
    }
    return result;
}

void InitTemps(void) {
    uint16_t t;

    // fans on by default
    // todo: fan1
    P3_DIR &= ~(1<<5);
    P3_PU &= ~(1<<5);
    P3 &= ~(1<<5);

    // adc setup
#if !defined(DISABLE_TEMP2)
    P1_IE &= 0b11111100;                           // enable AIN0 + AIN1
#else
    P1_IE &= 0b11111110;                           // enable AIN0
#endif
    ADC_SETUP = bADC_POWER_EN | bADC_EXT_SW_EN;     // power enable + extended mode
    ADC_EX_SW = bADC_RESOLUTION;                    // 11bit resolution
    ADC_CK_SE = 16;                                 // clock division (must be between 1 and 12 mhz)
    for (uint8_t i = 0; i < 16; i++) {
       t = ADC_FIFO;
    }
    ADC_CTRL   = 0b00000000;                        // manual mode
    ADC_CHANN  = 0b00000000;                        // AIN0
    pos = 0;                                        // prime averaging buffers
    status = TEMP_STATUS_ADC0 | TEMP_STATUS_ADC1;
    for (int i=0; i<tempsensor_avgcount; i++) {
        adc[0][pos] = ReadTempSensorRaw(0);
        adc[1][pos] = ReadTempSensorRaw(1);
        pos = (pos + 1) & tempsensor_avgmask;
    }

    // initial status
    status =  (TEMP_STATUS_FAN0 | (GetBoardTemperature(0, 0, 0) ? TEMP_STATUS_ADC0 : 0));
    status |= (TEMP_STATUS_FAN1 | (GetCoreTemperature(0, 0, 0)  ? TEMP_STATUS_ADC1 : 0));
    TRACE("sensor status = $%x", status);
}

void ProcessTemps(void) {
    static __xdata uint32_t last = 0;
    static __xdata uint8_t idx = 0;
    if (elapsed(last) < 500) {
        return;
    }

    adc[idx][pos] = ReadTempSensorRaw(idx);

    if (idx == 0) {
        // board temperature and fan control
        if (Settings.FanControl0 == FANCONTROL_OFF) {
            SetFanState(0, false);
        } else if (Settings.FanControl0 == FANCONTROL_ON) {
            SetFanState(0, true);
        } else if (Settings.FanControl0 == FANCONTROL_AUTO) {
            uint16_t adc; uint8_t res, deg;
            if (GetBoardTemperature(&adc, &res, &deg)) {
                if (deg <= Settings.EiffelTemp[0].Low) {
                    SetFanState(0, false);
                }
                else if (deg >= Settings.EiffelTemp[0].High) {
                    SetFanState(0, true);
                }
            }
        }
    } else {
        // core temperature, fan control and auto shutdown
        #if !defined(DISABLE_TEMP2)
        if ((Settings.FanControl1 == FANCONTROL_AUTO) || (Settings.CoreTempShutdown & 0x80))
        {
            uint16_t adc; uint8_t res, deg;
            if (GetCoreTemperature(&adc, &res, &deg)) {
                //TRACE("temp1 is %u, %u, %d", adc, res * 100, deg);

                // cpu overheat protection
                if ((Settings.CoreTempShutdown & (1 << 7)) && (deg > (Settings.CoreTempShutdown & 0x7f))) {
                    // todo
                }

                // auto fan control
                if (Settings.FanControl1 == FANCONTROL_AUTO) {
                    if (deg <= Settings.EiffelTemp[1].Low) {
                        SetFanState(1, false);
                    }
                    else if (deg >= Settings.EiffelTemp[1].High) {
                        SetFanState(1, true);
                    }
                }
            }
        }
        if (Settings.FanControl1 == FANCONTROL_OFF) {
            SetFanState(1, false);
        } else if (Settings.FanControl1 == FANCONTROL_ON) {
            SetFanState(1, true);
        }
        #endif
        pos = (pos + 1) & tempsensor_avgmask;
    }
    idx = (idx + 1) & 1;
    last = msnow;
}

#endif // !DISABLE_TEMPS
