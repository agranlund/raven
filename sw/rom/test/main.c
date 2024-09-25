/*
 * Raven test program.
 */

#include <lib.h>

extern uint8_t __bss_start, __bss_end;

void main(void)
{
    puts("test");
}

void _main(void)
{
	memset(&__bss_start, 0, &__bss_end - &__bss_start);
    main();
}
