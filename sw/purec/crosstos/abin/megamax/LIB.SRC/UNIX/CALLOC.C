/*LINTLIBRARY*/
extern char *lmalloc();

char *lcalloc(nelem, size)
register unsigned long nelem, size;
{
	register char *new, *p;
	register unsigned long sz;

	if ( ! (new = lmalloc(sz = nelem * size) ) )
		return new;
	for (p = new; sz--; *p++ = 0);

	return new;
}


char *calloc(nelem, size)
unsigned nelem, size;
{
	return lcalloc((unsigned long) nelem, (unsigned long) size);
}
