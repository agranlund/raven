/*
 * common startup code for srec programs
 */

    .globl main
    .globl exit
    .globl lib_Init
    .section .note.GNU-stack,"",%progbits

    .text
    .align 2
    .globl start
    .globl __stack_top
    .type start,@function

start:
    move.w  %sr,-(%sp)                  //; save interrupts
    movem.l %d2-%d7/%a2-%a6,-(%sp)      //; save registers
    move.l  %sp,__stack_top-4           //; save stack pointer

    move.w  #0x2700,%sr                 //; disable interrupts
    lea     __stack_top-4,%sp           //; initialize stack

    move.l  #__bss_start,%a0            //; clear bss
    move.l  #__bss_end,%d0
    sub.l   %a0,%d0
    bra.b   1f
0:  move.b  #0,(%a0)+
1:  dbra    %d0,0b

    jbsr    lib_Init                    //; init tiny c library

    jbsr    main                        //; call programm

exit:
    move.l  __stack_top-4,%sp           //; restore stack pointer
    movem.l (%sp)+,%d2-%d7/%a2-%a6      //; restore registers
    move.w  (%sp)+,%sr                  //; restore interrupts
    rts                                 //; return to monitor
