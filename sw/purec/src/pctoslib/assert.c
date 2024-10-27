#undef NDEBUG
#include <stdio.h>
#include <stdlib.h>

/* FIXME: belongs to pcstdlib.lib
   link in here until pcstdlib.lib is finished */

void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function)
{
	(void)function;
	fprintf(stderr, "\nAssertion failed: %s, file %s, line %u\n",
		assertion, file, line);
	abort();
}
