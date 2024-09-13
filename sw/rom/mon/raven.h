/*--------------------------------------------------------------------------------
Important:
- This header must be compatible with both GCC and PureC
- Bios functions must be callable from both GCC and PureC compiled binaries:
    - Functions has to be cdecl and all function arguments 32bit
--------------------------------------------------------------------------------*/
#ifndef _RAVEN_H_
#define _RAVEN_H_

#ifndef C_RAVN
#define C_RAVN          0x5241564E
#endif

#if defined(__GNUC__)
#define _RVAPI
#else
#define _RVAPI  cdecl
#endif

typedef struct
{
/* 0x0000 */
    uint32_t    magic;
    uint32_t    version;
    uint32_t    reserved0000[6];
/* 0x0020 */
    void        _RVAPI (*dbg_GPO)(int32_t num, int32_t val);
    uint32_t    reserved0020[7];
/* 0x0040 */
    void        _RVAPI (*rtc_Get)(uint32_t addr, uint8_t* buf, uint32_t siz);
    void        _RVAPI (*rtc_Set)(uint32_t addr, uint8_t* buf, uint32_t siz);
    uint32_t    reserved0040[6];
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

#endif // _RAVEN_H_


