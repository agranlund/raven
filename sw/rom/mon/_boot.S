    #include "raven.h"
    #include "hw/uart.h"

//
// todo: settings stored in rom so they can be reflashed without recompiling
//       or from motherboard jumper on rev.A2 boards
//

//    #define UART_BAUDR      921600
//    #define UART_BAUDR      460800
    #define UART_BAUDR      115200

    #define UART_RTS_FLOW   1

    .global start

    .extern sys_Init
    .extern _ravenBios
    .extern __stack_top

    .section .note.GNU-stack,"",%progbits

// ----------------------------------------------------------
// 0x40000000, 1k
// early vector table
// ----------------------------------------------------------
    .section    .vectors
    .align      4
rom_vectors:
    .dcb.l     1,ravenBios          //   0 / 0x00 : initial sp (bios ptr)
    .dcb.l     1,start              //   1 / 0x04 : initial pc
    .dcb.l     1,vec_berr           //   2 / 0x08 : bus errror
    .dcb.l   253,vec_other          //  --------- 3-255

// ----------------------------------------------------------
// 0x40000400, 1k
// toc
// ----------------------------------------------------------
    .section    .toc
rom_toc:
    .dcb.l  1,0x5F524F4D    // '_ROM'
    .dcb.l  1,BIOS_ROM
    .dcb.l  1,0x00000000
    .dcb.l  1,0x00000000

    .dcb.l  1,0x5F415049    // '_API'
    .dcb.l  1,__export_start
    .dcb.l  1,__export_size
    .dcb.l  1,0x00000000

    .dcb.l  1,0x5F434647    // '_CFG'
    .dcb.l  1,__config_start
    .dcb.l  1,__config_size
    .dcb.l  1,0x00000000


// ----------------------------------------------------------
// 0x40000800, 2k
// bios exports
// ----------------------------------------------------------
    .section    .export
rom_export:
    // export.c : const raven_t ravenBios

// ----------------------------------------------------------
// 0x40001000, 4k
// user config
// ----------------------------------------------------------
    .section    .config
rom_config:

    //.dcb.l   255,0xDEADBEEF


// ----------------------------------------------------------
// 0x40002000
// rom start
// ----------------------------------------------------------
    .section    .text

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

    // setup cpu
    move.w  #0x2700,%sr                             // no interrupts
    nop
    reset                                           // reset peripherals
    movec.l %pcr,%d0                                // no superscalar
    bclr.l  #0,%d0
    nop
    movec.l %d0,%pcr
    nop
    move.l  #rom_vectors,%d0                        // use rom vectors
    nop
    movec   %d0,%vbr
    moveq.l #0,%d0
    nop
    movec   %d0,%vbr                                // reset vbr
    nop
    movec   %d0,%tc                                 // disable pmmu
    movec   %d0,%dtt0
    movec   %d0,%itt0
    movec   %d0,%dtt1
    movec   %d0,%itt1
    nop
    pflusha
    nop
    movec   %d0,%cacr                               // disable caches
    nop
    cinva   %bc                                     // invalidate caches
    nop
    reset                                           // reset peripherals
    nop

    // setup stack, but do _not_ use it or any other memory until
    // we're done with early boot messages and memory detect.
    move.l  #__stack_top,%sp
    
    // make room for persistent hidden variables.
    // these are currently used to detect warmboot
    sub.l   #16,%sp

    // setup uart2
    move.l  #RV_PADDR_UART2,%a5
    move.b  #0x00,UART_LCR(%a5)                     // access normal
    move.b  #0x00,UART_IER(%a5)                     // no interrupts
    move.b  #0x01,UART_FCR(%a5)                     // fifo on
    move.b  #0x07,UART_FCR(%a5)                     // clear fifo buffers
    move.b  #0x00,UART_SPR(%a5)                     // clear scratch reg
    move.b  #0x00,UART_MCR(%a5)                     // modem control

    move.b  #0xBF,UART_LCR(%a5)                     // access efr register
    bset.b  #4,UART_EFR(%a5)                        // enable dld and mcr[7:5] access
#if (UART_RTS_FLOW != 0)
    bset.b  #6,UART_EFR(%a5)                        // auto rts
#endif

    move.l  #UART_BAUDR,%d0                         // todo: get baudrate from user config

    move.b  #0x80,UART_LCR(%a5)                     // access baud registers
    move.l  #UART_CLK,%d1                           // set DLL and DLM
    move.l  %d1,%d2
    divul.l %d0,%d2
    lsr.l   #4,%d2                                  // d2 = xx.xx.mm.ll       
    move.b  %d2,UART_DLL(%a5)
    lsr.l   #8,%d2
    move.b  %d2,UART_DLM(%a5)
    move.l  %d1,%d2                                 // set DLD
    lsl.l   #1,%d2
    divul.l %d0,%d2
    addq.l  #1,%d2
    lsr.l   #1,%d2
    and.b   #0x0f,%d2                               // d2 = xx.xx.xx.dd
    move.b  %d2,UART_DLD(%a5)

    bset.b  #6,UART_MCR(%a5)                        // enable tcr/tcl access
    move.b  #0x0f,UART_TCR(%a5)                     // auto rts resume/halt levels (0 / 64)
    move.b  #0x0f,UART_TLR(%a5)                     // rx/tx fifo trigger levels (0 / 64)
    bclr.b  #6,UART_MCR(%a5)                        // latch and disable tcr/tcl access

    move.b  #0xBF,UART_LCR(%a5)                     // access efr register
    bclr.b  #4,UART_EFR(%a5)                        // latch and disable access to dld and mcr[7:5]

    move.b  #0x03,UART_LCR(%a5)                     // 8 data bits, no parity, 1 stop bit
#if (UART_RTS_FLOW != 0)
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

    // setup default vectors
    move.l  #rom_vectors,%a0
    move.l  #0x00000000,%a1
    move.w  #255,%d0
.Lcopyvecs:
    move.l  (%a0)+,(%a1)+
    dbra.w  %d0,.Lcopyvecs

    move.l  #vec_berr,0x8                           // bus error
    move.l  #vec_rte, 0x60                          // irq0
    move.l  #vec_rte, 0x64                          // irq1
    move.l  #vec_rte, 0x68                          // irq2
    move.l  #vec_rte, 0x6C                          // irq3
    move.l  #vec_rte, 0x70                          // irq4
    move.l  #vec_rte, 0x74                          // irq5
    move.l  #vec_rte, 0x78                          // irq6
    move.l  #vec_rte, 0x7C                          // irq7

    moveq.l #0,%d0
    nop
    movec   %d0,%vbr
    nop

    // init
    jsr     sys_Init

    // reset
    bra     start


// ----------------------------------------------------------
// vectors
// ----------------------------------------------------------
    .type vec_other,@function
vec_other:
    move.l  #RV_PADDR_UART2,%a5
    BPRINT  %a5,#msg_exception
1:  stop    #0x2700
    bra     1b

    .type vec_berr,@function
vec_berr:
    move.l  #RV_PADDR_UART2,%a5
    BPRINT  %a5,#msg_berr
1:  stop    #0x2700
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
