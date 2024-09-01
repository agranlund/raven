#ifndef _RAVEN_H_
#define _RAVEN_H_

#ifndef C_RAVN
#define C_RAVN          0x5241564E
#endif

typedef struct
{
    uint32_t    magic;
    uint32_t    version;

    void        (*dbg_GPO)(int num, int val);

    bool        (*i2c_Aquire)(void);
    void        (*i2c_Release)(void);
    void        (*i2c_Start)(void);
    void        (*i2c_Stop)(void);
    uint8_t     (*i2c_Read)(uint8_t ack);
    uint8_t     (*i2c_Write)(uint8_t val);
    
    void        (*rtc_SetDateTime)(uint32_t dt, uint32_t mask);
    uint32_t    (*rtc_GetDateTime)(void);
    void        (*rtc_GetRam)(uint8_t addr, uint8_t* buf, uint8_t siz);
    void        (*rtc_SetRam)(uint8_t addr, uint8_t* buf, uint8_t siz);

    // ikbd
    // midi
    // uart

} raven_t;

#endif // _RAVEN_H_



