/* LINTLIBRARY */
/*
	A fast quick sort.
*/

static int size, ssize;
static int (*_cmp)(), (*_swap)();

#ifndef	lint
#define	ASSEMBLY
#endif

#ifndef	ASSEMBLY
static bswap(p1, p2)
register char *p1, *p2;
{
	register int sz = ssize;
	register char temp;

	do {
		temp = *p1;
		*p1++ = *p2;
		*p2++ = temp;
	} while (--sz);
}

static wswap(p1, p2)
/* pcc can't do sizeof in #if statements.  This indicates that
   the person responsible for pcc is a fool with shit for brains */
#ifdef	UNIX
register short *p1, *p2;
#else
register int *p1, *p2;
#endif
{
	register int sz = ssize;
	register int temp;

	do {
		temp = *p1;
		*p1++ = *p2;
		*p2++ = temp;
	} while (--sz);
}

static lswap(p1, p2)
register long *p1, *p2;
{
	register int sz = ssize;
	register long temp;

	do {
		temp = *p1;
		*p1++ = *p2;
		*p2++ = temp;
	} while (--sz);
}
#else	/* not lint */

static bswap(p1, p2)
char *p1, *p2;
{
	asm {
		move.l	p1(A6), A0
		move.l	p2(A6), A1
#ifdef PCREL
		move.w	ssize(A4), D0
#else
		move.w	ssize, D0
#endif
		subq.w	#1, D0
	loop:
		move.b	(A0), D1
		move.b	(A1), (A0)+
		move.b	D1, (A1)+
		dbf		D0, loop
	}
}

static wswap(p1, p2)
int *p1, *p2;
{
	asm {
		move.l	p1(A6), A0
		move.l	p2(A6), A1
#ifdef PCREL
		move.w	ssize(A4), D0
#else
		move.w	ssize, D0
#endif
		subq.w	#1, D0
	loop:
		move.w	(A0), D1
		move.w	(A1), (A0)+
		move.w	D1, (A1)+
		dbf		D0, loop
	}
}

static lswap(p1, p2)
long *p1, *p2;
{
	asm {
		move.l	p1(A6), A0
		move.l	p2(A6), A1
#ifdef PCREL
		move.w	ssize(A4), D0
#else
		move.w	ssize, D0
#endif
		subq.w	#1, D0
	loop:
		move.l	(A0), D1
		move.l	(A1), (A0)+
		move.l	D1, (A1)+
		dbf		D0, loop
	}
}
#endif	/* lint */

static _Qsort(m, n)
char *m, *n; /* These should NOT be register variables */
{
	register int sz = size;
	register char *i, *j;
	register int (*cmp)() = _cmp, (*swap)() = _swap;
	register char *pivot;

	if (m < n) {
		/* Check for sorted sublist */
		for (i = m, j = m + sz; i < n && (*cmp)(i, j) <= 0; i = j, j += sz);
		if (i == n)
			return;
		
		/* Check for reversed sublist */
		for (i = m, j = m + sz; i < n && (*cmp)(i, j) >= 0; i = j, j += sz);
		if (i == n) {
			for (i = m, j = n; i < j; i += sz, j -= sz)
				(*swap)(i, j);
			return;
		}

		pivot = m;
		i = m; j = n + sz;
		for (;;) {
			do 
				i += sz;
			while (i <= n && (*cmp)(i, pivot) < 0);
			do
				j -= sz;
			while ((*cmp)(j, pivot) > 0);
			if (i < j)
				(*swap)(i, j);
			else
				break;
		}

		if (j != pivot)
			(*swap)(pivot, j);

		if (j - sz - m > n - (j + sz)) {
			_Qsort(j + sz, n);
			_Qsort(m, j - sz);
		}
		else {
			_Qsort(m, j - sz);
			_Qsort(j + sz, n);
		}
	}
}

qsort(b, n, sz, cmp)
char *b;
int n;
register int sz;
int (*cmp)();
{
	_cmp = cmp;
	size = sz;

	switch (sz & 3) {
		case 0: _swap = lswap; ssize = size >> 2; break;
		case 2: _swap = wswap; ssize = size >> 1; break;
		default: _swap = bswap; ssize = size; break;
	}

	_Qsort(b, b + (n - 1) * sz);
}
