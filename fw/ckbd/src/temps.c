//---------------------------------------------------------------------
// temps.c
// temperature and fan control
//---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "settings.h"
#include "temps.h"

//#define DISABLE_TEMPS

#if defined(DISABLE_TEMPS)

void InitTemps(void) { }
void ProcessTemps(void) { }

#else

#define tempsensor_avgcount 8
#define tempsensor_avgmask  (tempsensor_avgcount - 1)
#define tempsensor_delay    5

static uint8_t pos;
static uint8_t status;
static uint16_t adc[2][tempsensor_avgcount];


void SetFanState(uint8_t idx, bool enable) {
    // todo: drive motor pin
    if (idx == 0) {
        if (enable) {
            status |= TEMP_STATUS_FAN0;
        } else {
            status &= ~TEMP_STATUS_FAN0;
        }
    } else if (idx == 1) {
        if (enable) {
            status |= TEMP_STATUS_FAN1;
        } else {
            status &= ~TEMP_STATUS_FAN1;
        }
    }
}

static uint16_t ReadTempSensorRaw(uint8_t idx) {
    uint16_t t = 0;
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

static void UpdateTempSensors(void) {
    adc[0][pos] = ReadTempSensorRaw(0);
    adc[1][pos] = ReadTempSensorRaw(1);
    pos = (pos + 1) & tempsensor_avgmask;
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
    if (*adc_out) { *adc_out = adc; }
    if ((adc >= 0x07f0) || (adc < 0x0010)) {
        if (res_out) { *res_out = 0; }
        if (deg_out) { *deg_out = 0; }
        return false;
    }
   
    // convert to voltage
    float vf = ((float)adc / 2047.0f) * 3.3f;

    // calculate thermistor resistance
#if defined(BOARD_RAVEN_A1) || defined(BOARD_PROTO)
    float rf = ((-20.0f * vf) / ((4.0f * vf) - 10.0f));
#else
    float rf = 10.0f / (3.3f - vf);
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

#if defined(BOARD_RAVEN_A1) || defined(BOARD_PROTO)
    uint16_t adc = 0x7ff;
#else
    uint16_t adc = ReadTempSensorAvg(1);
#endif
    if (*adc_out) { *adc_out = adc; }
    if ((adc >= 0x07f0) || (adc < 0x0010)) {
        if (res_out) { *res_out = 0; }
        if (deg_out) { *deg_out = 0; }
        return false;
    }
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
    P1_IE &= ~0b00111111;                           // enable AIN0 + AIN1
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
        UpdateTempSensors();
    }
    status = GetBoardTemperature(0, 0, 0) ? TEMP_STATUS_ADC0 : 0;// TEMP_STATUS_FAN0;
    status |= GetCoreTemperature(0, 0, 0) ? TEMP_STATUS_ADC1 : 0; //TEMP_STATUS_FAN1;
    TRACE("sensor status = %02x", status);
}

void ProcessTemps(void) {
    static uint32_t last = 0;
    if (elapsed(last) < 1000) {
        return;
    }
    UpdateTempSensors();

    // board temperature and fan control
    if (Settings.FanControl0 == FANCONTROL_OFF) {
        SetFanState(0, false);
    } else if (Settings.FanControl0 == FANCONTROL_ON) {
        SetFanState(0, true);
    } else if (Settings.FanControl0 == FANCONTROL_AUTO) {
        uint16_t adc; uint8_t res, deg;
        if (GetBoardTemperature(&adc, &res, &deg)) {
            if ((status & TEMP_STATUS_FAN0) && (deg <= Settings.EiffelTemp[0].Low)) {
                SetFanState(0, false);
            }
            else if (((status & TEMP_STATUS_FAN0) == 0) && (deg >= Settings.EiffelTemp[0].High)) {
                SetFanState(0, true);
            }
        }
    }
    
    // core temperature, fan control and auto shutdown
    if ((Settings.FanControl1 == FANCONTROL_AUTO) || (Settings.CoreTempShutdown & 0x80))
    {
        uint16_t adc; uint8_t res, deg;
        if (GetCoreTemperature(&adc, &res, &deg)) {
            TRACE("temp1 is %u, %u, %d", adc, res * 100, deg);

            // cpu overheat protection
            if ((Settings.CoreTempShutdown & (1 << 7)) && (deg > (Settings.CoreTempShutdown & 0x7f))) {
                // todo
            }

            // auto fan control
            if (Settings.FanControl1 == FANCONTROL_AUTO) {
                if ((status & TEMP_STATUS_FAN1) && (deg <= Settings.EiffelTemp[1].Low)) {
                    SetFanState(1, false);
                }
                else if (((status & TEMP_STATUS_FAN1) == 0) && (deg >= Settings.EiffelTemp[1].High)) {
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
    
    // todo: drive fan pins


    last = msnow;
}

#endif // !DISABLE_TEMPS
