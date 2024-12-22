#include <stdio.h>

typedef int ALIGN;

union header {
    struct {
		union header *ptr;
		unsigned long size;
    } s;
    ALIGN x;
  };

typedef union header HEADER;

static fmove(p, q, n)
register char *p, *q;
register unsigned long n;
{
	{
		register long *pp = (long *) p, *qq = (long *) q;

		for (; n >= sizeof(long); n -= sizeof(long)) {
			*qq++ = *pp++;
		}
		q = (char *) qq;
		p = (char *) pp;
	}

	for (; n--; *q++ = *p++);
}

char *lrealloc(p, size)
char *p;
register unsigned long size;
{
	register union header *it, *it_base;
	register char *new;
	register unsigned long itunits, nunits, bytes;

	it = (union header *)p;
	it_base = it - 1;
    nunits = 1+(size+sizeof(HEADER)-1)/sizeof(HEADER);
	itunits = it_base->s.size;
	if (itunits >= nunits) {
		if (itunits > nunits) {
			it_base->s.size = nunits;
			it_base += nunits;
			it_base->s.size = itunits - nunits;
			free(it_base+1);
		}

		return (char *)it;
	}

	/* Size is bigger, allocate more space and copy */
	new = lmalloc(size);
 	bytes = it_base->s.size * sizeof(HEADER);
	
	fmove((char *) it, new, bytes);
	free((char *)it);

	return new;
}

char *realloc(it, size)
char *it;
unsigned size;
{
	return lrealloc(it, (unsigned long)size);
}
