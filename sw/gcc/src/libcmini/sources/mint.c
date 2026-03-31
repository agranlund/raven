#include <mint/cookie.h>
#include "lib.h"

int __mint = -1;

int __is_mint(void)
{
	if (__mint < 0)
	{
		long v;

		if (Getcookie(C_MiNT, &v) == C_FOUND)
			__mint = v;
		else
			__mint = 0;
	}
	return __mint;
}
