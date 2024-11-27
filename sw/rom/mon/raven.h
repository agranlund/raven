/*--------------------------------------------------------------------------------
Important:
- This header must remain compatible with both GCC and PureC
- Bios functions must be callable from both GCC and PureC compiled binaries:
    - Functions has to be cdecl and all arguments 32bit
--------------------------------------------------------------------------------*/
#ifndef _RAVEN_H_
#define _RAVEN_H_

/*--------------------------------------------------------------------------------*/
#define C_RAVN_VER      0x20241127UL
#define C_RAVN_PTR      0x40000000UL
#ifndef C_RAVN
#define C_RAVN          0x5241564EUL
#endif

/*--------------------------------------------------------------------------------*/
#if defined(__GNUC__)
#define _RVAPI
#define _RVINL static inline
#else
#define _RVAPI cdecl
#define _RVINL static
#endif

/*--------------------------------------------------------------------------------*/
typedef struct
{
/* 0x0000 */
    uint32_t    magic;
    uint32_t    version;
    uint32_t    reserved0000[6];

/* 0x0020 */
    uint32_t    _RVAPI (*dbg_GPI)(uint32_t num);
    void        _RVAPI (*dbg_GPO)(uint32_t num, uint32_t val);
    uint32_t    reserved0020[6];

/* 0x0040 */
    void        _RVAPI (*rtc_Read)(uint32_t addr, uint8_t* buf, uint32_t siz);
    void        _RVAPI (*rtc_Write)(uint32_t addr, uint8_t* buf, uint32_t siz);
    uint32_t    reserved0040[2];
    uint32_t    _RVAPI (*cfg_Read)(const char* name);
    void        _RVAPI (*cfg_Write)(const char* name, uint32_t value);
    uint32_t    reserved0050[2];

/* 0x0060 */
    int32_t     _RVAPI (*i2c_Aquire)(void);
    void        _RVAPI (*i2c_Release)(void);
    void        _RVAPI (*i2c_Start)(void);
    void        _RVAPI (*i2c_Stop)(void);
    uint32_t    _RVAPI (*i2c_Read)(uint32_t ack);
    uint32_t    _RVAPI (*i2c_Write)(uint32_t val);
    uint32_t    reserved0060[2];
    
/* 0x0080 */

} raven_t;


#define RV60_GPI            0x20
#define RV60_GPO            0x24
#define RV60_RTC_READ       0x40
#define RV60_RTC_WRITE      0x44
#define RV60_CFG_READ       0x50
#define RV60_CFG_WRITE      0x54
#define RV60_I2C_AQUIRE     0x60
#define RV60_I2C_RELEASE    0x64
#define RV60_I2C_START      0x68
#define RV60_I2C_STOP       0x6C
#define RV60_I2C_READ       0x70
#define RV60_I2C_WRITE      0x74


#endif /* _RAVEN_H_ */

