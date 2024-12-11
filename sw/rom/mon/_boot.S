    #include "raven.h"
    #include "hw/uart.h"

//  #define UART_BAUDR      750000
    #define UART_BAUDR      115200
    #define UART_BAUDD      (UART_CLK/(UART_BAUDR*16))
    #define UART_FLOWCTRL   0

    .global start

    .extern sys_Init
    .extern _ravenBios
    .extern __stack_top

    .section .note.GNU-stack,"",%progbits

// ----------------------------------------------------------
// vector table
// ----------------------------------------------------------
    .section    .vectors
    .align      4
    .dcb.l     1,ravenBios          //   0 / 0x00 : initial sp (bios ptr)
    .dcb.l     1,start              //   1 / 0x04 : initial pc
    .dcb.l     1,vec_berr           //   2 / 0x08 : bus errror
    .dcb.l   253,vec_other          //  --------- 3-255

//--------------------------------------------
// BPRINT(uart*, char*)
//--------------------------------------------
            .macro BPRINT,r,str
            move.l  \str,%a0
    0:      move.b  (%a0)+,%d0
            beq.b   2f
    1:      btst.b  #5,UART_LSR(\r)
            beq.b   1b
            move.b  %d0,UART_THR(\r)
            bra.b   0b
    2:      nop
            .endm

// ----------------------------------------------------------
// boot
// ----------------------------------------------------------
    .text
    .align  2
    .type start, @function
start:
    // setup hardware
    move.w  #0x2700,%sr                             // no interrupts
    cinva   %bc                                     // invalidate caches
    move.l  #0x400000,%d0                           // clear branch cache
    movec   %d0,%cacr                               // disable all caches
    sub.l   %d0,%d0
    movec   %d0,%tc                                 // disable pmmu
    pflusha
    //move.l  #0x00ff8040,%d0
    sub.l   %d0,%d0
    movec   %d0,%dtt0
    movec   %d0,%itt0
    sub.l   %d0,%d0
    movec   %d0,%dtt1
    movec   %d0,%itt1
    pflusha
    reset                                           // reset peripherals
    sub.l   %d0,%d0                                 // reset vbr
    movec   %d0,%vbr

    // setup stack
    move.l  #__stack_top,%sp

    // setup uart2
    move.l  #RV_PADDR_UART2,%a5
    move.b  #0x00,UART_LCR(%a5)                     // access normal
    move.b  #0x00,UART_IER(%a5)                     // no interrupts
    move.b  #0x01,UART_FCR(%a5)                     // fifo on
    move.b  #0x07,UART_FCR(%a5)                     // clear fifo buffers
    move.b  #0x00,UART_SPR(%a5)                     // clear scratch reg
    move.b  #0x00,UART_MCR(%a5)                     // modem control

    move.b  #0xBF,UART_LCR(%a5)                     // access efr register
    bset.b  #4,UART_EFR(%a5)                        // enable access
#if (UART_FLOWCTRL != 0)
    bset.b  #6,UART_EFR(%a5)                        // auto rts
#endif
    
    move.b  #0x80,UART_LCR(%a5)                     // access baud registers
    move.b  #(UART_BAUDD&0xFF),UART_DLL(%a5)        // baudrate L
    move.b  #((UART_BAUDD>>8)&0xFF),UART_DLM(%a5)   // baudrate H
    move.b  #0x00,UART_DLD(%a5)                     // baudrate D

    bset.b  #6,UART_MCR(%a5)                        // enable tcr/tcl access
    move.b  #0x04,UART_TCR(%a5)                     // auto rts resume/halt levels (0 / 16)
    move.b  #0x11,UART_TLR(%a5)                     // rx/tx fifo trigger levels (4 / 4)
    bclr.b  #6,UART_MCR(%a5)                        // disable tcr/tcl access

    move.b  #0xBF,UART_LCR(%a5)                     // access efr register
    bclr.b  #4,UART_EFR(%a5)                        // latch and disable access

    move.b  #0x03,UART_LCR(%a5)                     // 8 data bits, no parity, 1 stop bit
#if (UART_FLOWCTRL != 0)
    bset.b  #1,UART_MCR(%a5)                        // enable rts
#endif

    // welcome message
    BPRINT  %a5,#msg_boot

    // verify that there is ram
    move.l  #0x00000000,0x00000000
    cmp.l   #0x00000000,0x00000000
    beq.b   .Lvec_boot_ram_ok
    BPRINT  %a5,#msg_ramfail
    stop    #0x2700

.Lvec_boot_ram_ok:

    // exception handlers
    move.l  #vec_berr,0x8                           // bus error
    move.l  #vec_rte, 0x60                          // irq0
    move.l  #vec_rte, 0x64                          // irq1
    move.l  #vec_rte, 0x68                          // irq2
    move.l  #vec_rte, 0x6C                          // irq3
    move.l  #vec_rte, 0x70                          // irq4
    move.l  #vec_rte, 0x74                          // irq5
    move.l  #vec_rte, 0x78                          // irq6
    move.l  #vec_rte, 0x7C                          // irq7

    // init
    jsr     sys_Init

    // reset
    move.w  #0x2700,%sr
    reset
    bra     start


// ----------------------------------------------------------
// dummy vector
// ----------------------------------------------------------
    .type vec_other,@function
vec_other:
    move.l  #RV_PADDR_UART2,%a5
    BPRINT  %a5,#msg_exception
1:
    stop    #0x2700
    bra     1b

    .type vec_berr,@function
vec_berr:
    move.l  #RV_PADDR_UART2,%a5
    BPRINT  %a5,#msg_berr
1:
    stop    #0x2700
    bra     1b

    .type vec_rte,@function
vec_rte:
    rte

// ----------------------------------------------------------
// constants
// ----------------------------------------------------------
    .section .rodata
msg_boot:
    .asciz  "\r\nRAVEN060\r\n"
msg_ramfail:
    .asciz  "ERROR: NO RAM IN SLOT0\r\n"
msg_berr:
    .asciz  "ERROR: ACCESS ERROR\r\n"
msg_exception:
    .asciz  "ERROR: UNEXPECTED EXCEPTION\r\n"
