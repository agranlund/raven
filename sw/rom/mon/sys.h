#ifndef _SYS_H_
#define _SYS_H_

#ifndef __ASM__

// types
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;
typedef signed char     sint8;
typedef signed short    sint16;
typedef signed int      sint32;


typedef enum
{
    ECPUSKU_UNKNOWN,
    ECPUSKU_EC,
    ECPUSKU_LC,
    ECPUSKU_RC,
} ECpuSKU;


typedef struct
{
    uint32* urp;
    uint32* srp;
    uint32  tcr;
    uint32  itt0;
    uint32  itt1;
    uint32  dtt0;
    uint32  dtt1;
} TMMU;


// macros
#define IOB(base,offs)  *((volatile  uint8*)(base + offs))
#define IOW(base,offs)  *((volatile uint16*)(base + offs))
#define IOL(base,offs)  *((volatile uint32*)(base + offs))

static inline void nop() {
    __asm__ __volatile__( "\tnop\n" : : : );
}

// functions
uint16  GetIPL();
void    SetIPL(uint16 v);
uint32  GetCACR();
void    SetCACR(uint32 v);
uint32  GetVBR();
void    SetVBR(uint32 v);
uint32  GetPCR();
void    SetPCR(uint32 v);
void    GetMMU(TMMU* mmu);
void    SetMMU(TMMU* mmu);

uint32  Call(uint32 addr);

uint32 DetectCPU(uint32* revout, uint32* idout);

void InitMonitor();
void StartMonitor();

void vecNMI();
void vecRTE();
void vecMFP_I0();
void vecMFP_I1();
void vecMFP_I2();
void vecMFP_I3();
void vecMFP_I4();
void vecMFP_I5();
void vecMFP_I6();
void vecMFP_I7();

void sys_Init();

void kmem_Init();
uint32 kmem_Alloc(uint32 size, uint32 align);

void vbr_Init();
void vbr_Set(uint32 vec, uint32 addr);

void pmmu_Init(uint32* simms);
void pmmu_Map(uint32 log, uint32 phys, uint32 size, uint32 flags);
void pmmu_Redirect(uint32 src, uint32 dst, uint32 size);

void uart_printChar(const char d);
void uart_printString(const char* string);
void uart_printHex(uint32 bits, const char* prefix, uint32 val, const char* suffix);

void uart1_gpo(uint8 bit, uint8 output);

int strcmp(char* s0, char* s1);
uint32 strtoi(char* s);

#endif //!__ASM__

// timer frequencies
#define UART_CLK        24000000
#define MFP1_CLK        2457600
#define MFP2_CLK        2000000

// memory map
#define PADDR_SIMM0     0x00000000
#define PADDR_SIMM1     0x01000000
#define PADDR_SIMM2     0x02000000
#define PADDR_SIMM3     0x03000000
#define PADDR_UART1     0x20000000
#define PADDR_UART2     0x20000020
#define PADDR_IDE       0xA0000000
#define PADDR_MFP2      0xA0000A00
#define PADDR_YM        0xA1000800
#define PADDR_MFP1      0xA1000A00
#define PADDR_ISA8_RAM  0x80000000
#define PADDR_ISA8_IO   0x81000000
#define PADDR_ISA16_RAM 0x82000000
#define PADDR_ISA16_IO  0x83000000

// uart regs
#define UART_DLL        0x00
#define UART_RHR        0x00
#define UART_THR        0x00
#define UART_DLM        0x04
#define UART_IER        0x04
#define UART_DLD        0x08
#define UART_ISR        0x08
#define UART_FCR        0x08
#define UART_EFR        0x08
#define UART_LCR        0x0C
#define UART_MCR        0x10
#define UART_LSR        0x14
#define UART_MSR        0x18
#define UART_TCR        0x18
#define UART_SPR        0x1C
#define UART_TLR        0x1C
#define UART_FRD        0x1C

// ym regs
#define YM_ADDR         0x00
#define YM_DATA         0x02

// mfp regs
#define MFP_GPDR        0x01
#define MFP_AER         0x03
#define MFP_DDR         0x05
#define MFP_IERA        0x07
#define MFP_IERB        0x09
#define MFP_IPRA        0x0B
#define MFP_IPRB        0x0D
#define MFP_ISRA        0x0F
#define MFP_ISRB        0x11
#define MFP_IMRA        0x13
#define MFP_IMRB        0x15
#define MFP_VR          0x17
#define MFP_TACR        0x19
#define MFP_TBCR        0x1B
#define MFP_TCDCR       0x1D
#define MFP_TADR        0x1F
#define MFP_TBDR        0x21
#define MFP_TCDR        0x23
#define MFP_TDDR        0x25
#define MFP_SCR         0x27
#define MFP_UCR         0x29
#define MFP_RSR         0x2B
#define MFP_TSR         0x2D
#define MFP_UDR         0x2F


// gpio
#define UART1_GPO_PWRLED    0
#define UART1_GPO_TP307     1


#endif // _SYS_H_
