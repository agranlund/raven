/*
 * startup code for test program
 */

    .globl _main
    .globl _exit
    .section .note.GNU-stack,"",%progbits

    .text
    .align 2
    .globl start
    .globl __stack_top
    .type start,@function
start:
    move.w  #0x2700,%sr
    lea     __stack_top,%sp
    jbsr    _main
    jbra    .
