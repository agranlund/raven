/*--------------------------------------------------------------------------------
Important:
- This header must remain compatible with both GCC and PureC
- Bios functions must be callable from both GCC and PureC compiled binaries:
    - Functions has to be cdecl and all arguments 32bit
--------------------------------------------------------------------------------*/
#ifndef _RAVEN_H_
#define _RAVEN_H_

/*--------------------------------------------------------------------------------*/
#define C_RAVN_VER          0x20250307UL
#define C_RAVN_PTR          0x40000000UL
#ifndef C_RAVN
#define C_RAVN              0x5241564EUL
#endif

/*--------------------------------------------------------------------------------*/
#define RV_PADDR_SIMM0      0x00000000UL
#define RV_PADDR_SIMM1      0x01000000UL
#define RV_PADDR_SIMM2      0x02000000UL
#define RV_PADDR_SIMM3      0x40000000UL
#define RV_PADDR_UART1      0x20000000UL
#define RV_PADDR_UART2      0x20000020UL
#define RV_PADDR_IDE        0xA0000000UL
#define RV_PADDR_YM         0xA1000800UL
#define RV_PADDR_MFP1       0xA1000A00UL
#define RV_PADDR_MFP2       0xA0000A00UL
#define RV_PADDR_ISA_RAM    0x80000000UL
#define RV_PADDR_ISA_IO     0x81000000UL
#define RV_PADDR_ISA_RAM16  0x82000000UL
#define RV_PADDR_ISA_IO16   0x83000000UL


/*--------------------------------------------------------------------------------*/
#if !defined(__ASM__)
#if defined(__GNUC__)
#define _RVAPI
#if (__STDC_VERSION__ > 199409L) 
#define _RVINL static inline
#else
#define _RVINL static
#endif
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
#if defined(__GNUC__)
    struct X86EMU* _RVAPI (*x86)(void);
#else
    void*       _RVAPI (*x86)(void);
#endif
    uint32_t    _RVAPI (*vga_Init)(void);
    void        _RVAPI (*vga_Clear)(void);
    uint32_t    _RVAPI (*vga_Addr)(void);
    void        _RVAPI (*vga_640_480_1)(void);
    void        _RVAPI (*vga_320_200_8)(void);
    void        _RVAPI (*vga_SetPal)(uint32_t idx, uint32_t num, uint8_t* pal);
    void        _RVAPI (*vga_GetPal)(uint32_t idx, uint32_t num, uint8_t* pal);

/* 0x00A0 */
    void        _RVAPI (*cache_On)(void);
    void        _RVAPI (*cache_Off)(void);
    void        _RVAPI (*cache_Flush)(void);
    void        _RVAPI (*mmu_Map)(uint32_t log, uint32_t phys, uint32_t size, uint32_t flags);
    void        _RVAPI (*mmu_Redirect)(uint32_t logsrc, uint32_t logdst, uint32_t size);
    void        _RVAPI (*mmu_Invalid)(uint32_t log, uint32_t size);
    void        _RVAPI (*mmu_Flush)(void);
    uint32_t*   _RVAPI (*mmu_GetPageDescriptor)(uint32_t log);

/* 0x00C0 */
    void        _RVAPI (*mon_Exec)(const char* s);
    uint32_t    _RVAPI (*rom_Id)(void);
    uint32_t    _RVAPI (*rom_Program)(void* data, uint32_t size);
    uint32_t    reserved00C0[3];
    int32_t     _RVAPI (**mon_fgetchar)(void);
    void        _RVAPI (**mon_fputchar)(int32_t c);

} raven_t;


_RVINL raven_t* raven(void) { return *((raven_t**)(C_RAVN_PTR)); }

#endif /* __ASM__ */
#endif /* _RAVEN_H_ */

