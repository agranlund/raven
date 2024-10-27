#ifndef _RAVEN_H_
#define _RAVEN_H_

#define C_RAVN_VER		0x20240907UL
#define C_RAVN_PTR		0x40000000UL
#ifndef C_RAVN
#define C_RAVN          0x5241564EUL
#endif
 
typedef struct
{
/*0x0000*/  
    uint32_t    magic;
    uint32_t    version;
    uint32_t    reserved0000[6];
/*0x0020*/
    void        cdecl (*dbg_GPO)(int32_t num, int32_t val);
    uint32_t    reserved0020[7];
/*0x0040*/
    void        cdecl (*rtc_Get)(uint32_t addr, uint8_t* buf, uint32_t siz);
    void        cdecl (*rtc_Set)(uint32_t addr, uint8_t* buf, uint32_t siz);
    uint32_t    reserved0040[6];
/*0x0060*/
    int32_t     cdecl (*i2c_Aquire)(void);
    void        cdecl (*i2c_Release)(void);
    void        cdecl (*i2c_Start)(void);
    void        cdecl (*i2c_Stop)(void);
    uint32_t    cdecl (*i2c_Read)(uint32_t ack);
    uint32_t    cdecl (*i2c_Write)(uint32_t val);
    uint32_t    reserved0060[2];
/*0x0080*/

} raven_t;


/*---------------------------------------------

 api

---------------------------------------------*/
#include <mint/cookie.h>
#if defined(__GCC__)
#define RVAPI static inline
#else
#define RVAPI static
#endif


/*---------------------------------------------
 rv60_Get
---------------------------------------------*/
RVAPI const raven_t* rv60_Get() {
	const raven_t* cookie = 0;
	if (Getcookie(C_RAVN,(int32_t*)&cookie) != C_FOUND)
		return 0;
	if (cookie->magic != C_RAVN)
		return 0;
	return cookie;
}

#endif /* _RAVEN_H_ */

