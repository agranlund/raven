/*
stuffbits(ptr, bits)
	char *ptr, *bits;
{
	unsigned mask	 = 0x80;
	unsigned maskcnt = 1;
	unsigned t1      = 0;
	char     c;

	while(c = *bits++) {
		if (c == '1')
			t1 |= mask;

		mask >>= 1;
		if (maskcnt++ > 7) {
			mask    = 0x80;
			maskcnt = 1;
			*ptr++  = t1;
			t1      = 0;
		}
	}
}
*/
stuffbits( ptr, bits )
	register char		*ptr;
	register char		*bits;
	{
	register char		c;
	register int		pos	= 0;
	register char		tmp	= *ptr;
	register int		mask= 0x80;

	while ( c = *bits++ ) {
		if ( c == '1' )
			tmp	|= mask >> pos;
		else
		if ( c == '0' )
			tmp	&= ~(mask >> pos);
		if ( pos++ == 7 ) {
			pos		= 0;
			*ptr++	= tmp;
			tmp		= *ptr;
			}
		}
	*ptr++	= tmp;
	}

