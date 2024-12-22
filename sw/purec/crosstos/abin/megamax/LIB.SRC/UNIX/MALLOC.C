#include <stdio.h>
#include <strings.h>

typedef int ALIGN;

union header {
    struct {
	union header *ptr;
	unsigned long size;
    } s;
    ALIGN x;
  };

typedef union header HEADER;

static HEADER base;
static HEADER *allocp = 0;

#define NALLOC 1048

static HEADER *morecore(nu)
unsigned long nu;
{
    char *lsbrk();
    long getptrsize();

    register char *cp;
    register HEADER *up;
    register unsigned long rnu;

    rnu = NALLOC * ((nu+NALLOC-1) / NALLOC);
    cp = lsbrk(rnu * sizeof(HEADER));
    if (cp == (char *)-1L)
	return(NULL);
    up = (HEADER *)cp;
    up->s.size = rnu;
    free((char *)(up+1));
    return(allocp);
}

char *lmalloc(nbytes)
unsigned long nbytes;
{
    register HEADER *p, *q;
    register unsigned long nunits;

    nunits = 1+(nbytes+sizeof(HEADER)-1)/sizeof(HEADER);

    if ((q = allocp) == NULL) {
	base.s.ptr = allocp = q = &base;
	base.s.size = 0;
    }
    for (p=q->s.ptr; ; q=p, p=p->s.ptr) {
	if (p->s.size >= nunits) {
	    if (p->s.size == nunits)
		q->s.ptr = p->s.ptr;
	    else {
		p->s.size -= nunits;
		p += p->s.size;
		p->s.size = nunits;
	    }
	    allocp = q;
	    return (char *)(p+1);
	}
	if (p == allocp)
	    if ((p = morecore(nunits)) == NULL)
		return(NULL);
    }
}

char *malloc(amount)
unsigned amount;
{
	return lmalloc((unsigned long) amount);
}

free(ap)
char *ap;
{
    register HEADER *p, *q;

    if (!ap)
	return;

    p = (HEADER *)ap - 1;
    for (q=allocp; !(p > q && p < q->s.ptr); q=q->s.ptr)
	if (q >= q->s.ptr && (p > q || p < q->s.ptr))
	    break;

    if (p+p->s.size == q->s.ptr) {
	p->s.size += q->s.ptr->s.size;
	p->s.ptr = q->s.ptr->s.ptr;
    } else
	p->s.ptr = q->s.ptr;
    if (q+q->s.size == p) {
	q->s.size += p->s.size;
	q->s.ptr = p->s.ptr;
    } else
	q->s.ptr = p;
    allocp = q;
}
