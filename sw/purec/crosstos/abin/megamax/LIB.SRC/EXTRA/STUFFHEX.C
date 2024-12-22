#include <ctype.h>
/*
	stuffhex - stuffs ascii hex into a real hex number.
*/
stuffhex(ptr, hex)
	register char *ptr, *hex;
{
	register char c;
	register char low_nibble;
	register char high_nibble;

	while(c = *hex++) {
		low_nibble = *ptr;
		high_nibble = (low_nibble >> 4) & 0xf;
		if (isxdigit(c)) {
			if (islower(c))
				c = toupper(c);
			if (c >= 'A')
				c = c - 'A' + 10;
			else
				c = c - '0';
			high_nibble = c;
		}
		if (c = *hex++) {
			if (isxdigit(c)) {
				if (islower(c))
					c = toupper(c);
				if (c >= 'A')
					c = c - 'A' + 10;
				else
					c = c - '0';
				low_nibble = c;
			}
		}
		low_nibble |= high_nibble << 4;
		*ptr++ = low_nibble;
	}
}

