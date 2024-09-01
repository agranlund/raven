    #include "sys.h"
    #include "hw/uart.h"

    #define UART_BAUDR      750000
    #define UART_BAUDD      (UART_CLK/(UART_BAUDR*16))
    #define UART_FLOWCTRL   0

    .text
    .global __main
    .global _main
    .global _vec_boot
    .global _vec_berr
    .global _vec_other
    .global _vec_rte

    .extern _sys_Init
    .extern __bss_start
    .extern _ravenBios

// ----------------------------------------------------------
// vector table
// ----------------------------------------------------------
    .dcb.l     1,_ravenBios         //   0 / 0x00 : initial sp (bios ptr)
    .dcb.l     1,__main             //   1 / 0x04 : initial pc
    .dcb.l     1,_vec_berr          //   2 / 0x08 : bus errror
    .dcb.l   253,_vec_other         //  --------- 3-255

//--------------------------------------------
// BPRINT(uart*, char*)
//--------------------------------------------
            .macro BPRINT,r,str
            move.l  \str,a0
    0:      move.b  (a0)+,d0
            beq.b   2f
    1:      btst.b  #5,UART_LSR(\r)
            beq.b   1b
            move.b  d0,UART_THR(\r)
            bra.b   0b
    2:      nop
            .endm

// ----------------------------------------------------------
// boot
// ----------------------------------------------------------
__main:
_main:
_vec_boot:
    move.w  #0x2700,sr                          // no interrupts
    cinva   ic                                  // invalidate caches
    cinva   dc
    cinva   bc
    move.l  #0x400000,d0                        // clear branch cache
    movec   d0,cacr                             // disable all caches
    sub.l   d0,d0
    movec   d0,tc                               // disable pmmu
    pflusha
    //move.l  #0x00ff8040,d0
    sub.l   d0,d0
    movec   d0,dtt0
    movec   d0,itt0
    sub.l   d0,d0
    movec   d0,dtt1
    movec   d0,itt1
    pflusha
    reset                                       // reset peripherals
    sub.l   d0,d0                               // reset vbr
    movec   d0,vbr

    move.l  #__bss_start,sp                     // bios stack

    // init uart2
    move.l  #PADDR_UART2,a5
    bclr.b  #7,UART_LCR(a5)                     // access normal regs
    move.b  #0x00,UART_IER(a5)                  // no interrupts

    //move.b  #0x00,UART_FCR(a5)                // fifo off
    move.b  #0x01,UART_FCR(a5)                  // fifo on
    move.b  #0x07,UART_FCR(a5)                  // clear fifo buffers
    move.b  #0x03,UART_LCR(a5)                  // 8 data bits, no parity, 1 stop bit
    move.b  #0x00,UART_SPR(a5)                  // clear scratch reg
    move.b  #0x00,UART_MCR(a5)                  // modem control

    // enable tcr regs
#if (UART_FLOWCTRL != 0)
    move.b  UART_LCR(a5),d0                         // access efr regs
    move.b  #0xBF,UART_LCR(a5)
//    bset.b  #4,UART_EFR(a5)
//    bset.b  #6,UART_MCR(a5)
//    move.b  #0x4A,UART_TCR(a5)                    // rts resume/halt levels
//    move.b  #0x88,UART_TLR(a5)                    // rx/tx fifo trigger
//    bclr.b  #6,UART_MCR(a5)
//    bclr.b  #4,UART_EFR(a5)
    move.b  #0xC0,UART_EFR(a5)                      // auto cts,rts
    move.b  d0,UART_LCR(a5)                         // normal regs
    bset.b  #1,UART_MCR(a5)                         // start rts flow control
#endif

    bset.b  #7,UART_LCR(a5)                         // access divisor regs
    move.b  #(UART_BAUDD&0xFF),UART_DLL(a5)         // baudrate L
    move.b  #((UART_BAUDD>>8)&0xFF),UART_DLM(a5)    // baudrate H
    bclr.b  #7,UART_LCR(a5)                         // access normal regs

    // welcome message
    BPRINT  a5,#msg_boot

    // check ram0
    move.l  #0x00000000,0x00000000
    cmp.l   #0x00000000,0x00000000
    beq.b   vec_boot_ram_ok
    BPRINT  a5,#msg_ramfail
    stop    #0x2700

vec_boot_ram_ok:

    // exception handlers
    move.l  #_vec_berr,0x8      // bus error
    move.l  #_vec_rte, 0x60     // irq0
    move.l  #_vec_rte, 0x64     // irq1
    move.l  #_vec_rte, 0x68     // irq2
    move.l  #_vec_rte, 0x6C     // irq3
    move.l  #_vec_rte, 0x70     // irq4
    move.l  #_vec_rte, 0x74     // irq5
    move.l  #_vec_rte, 0x78     // irq6
    move.l  #_vec_rte, 0x7C     // irq7

    // init
    jsr     _sys_Init

    // reset
    move.w  #0x2700,sr
    reset
    bra     _vec_boot


// ----------------------------------------------------------
// dummy vector
// ----------------------------------------------------------
_vec_other:
    move.l  #PADDR_UART2,a5
    BPRINT  a5,#msg_exception
1:
    stop    #0x2700
    bra     1b

_vec_berr:
    move.l  #PADDR_UART2,a5
    BPRINT  a5,#msg_berr
1:
    stop    #0x2700
    bra     1b

_vec_rte:
    rte

// ----------------------------------------------------------
// constants
// ----------------------------------------------------------
msg_boot:
    .asciz  "\r\nRAVEN060\r\n"
msg_ramfail:
    .asciz  "ERROR: NO RAM IN SLOT0\r\n"
msg_berr:
    .asciz  "ERROR: ACCESS ERROR\r\n"
msg_exception:
    .asciz  "ERROR: UNEXPECTED EXCEPTION\r\n"
