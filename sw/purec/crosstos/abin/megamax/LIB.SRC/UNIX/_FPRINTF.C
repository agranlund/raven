
#define MAXBUF	80		/*	maximum buffer size	*/

/* LINTLIBRARY */
#include <stdio.h>
#include <ctype.h>
#include <strings.h>

#ifdef NOVOID
#define void int
#endif

#ifdef ATARI
#define MAXINT 32767	/* Constant folding on old compiler is broken */
#else
#define	MAXINT	((int) ((unsigned) -1 >> 1))
#endif

char *sprintf();

static nconv(type, size, s, val)
int type;
int size; /* 1=long, 0=int */
register char **s;
register unsigned long val;
{
	char buf[20];
	register int base, hexo, d;
	register char *ss = *s, *p;
#ifdef PCREL
	int	sign	= 0;
#endif

	switch (type) {
		case 'd': 
		case 'u': 
			base = 10; 
			break;
		case 'o': 
			base = 8; 
			break;
		case 'X': 
			base = 16; 
			hexo = 0; 
			break;
		case 'x': 
			base = 16; 
			hexo = 'a' - 'A'; 
			break;
	}

	if ((long) val < 0)
		if (type == 'd') {
			*ss++ = '-';
			val = -val; 
		}
		else if (!size)
			val &= 0xffffL;
#ifdef PCREL
		else {
			val &= 0x7fffffffL;
			sign	= 1;
			}
#endif

	p = buf + 20;
	*--p = 0;
	do {
		d = val % base;
		*--p = d + (d >= 10 ? ('A' - 10) + hexo : '0');
	} while (val /= base);
#ifdef PCREL
	if ( sign ) {
		d	+= 8;
		*p = d + (d >= 10 ? ('A' - 10) + hexo : '0');
		}
#endif

	*s = xtrcpy(ss, p);
}

static fconv(type, prec, s, val)
int type;
register int prec;    /* precision */
register char **s;
double val;
{
	register char *ss = *s, *sg;
	register int i;
	char *t;
	int maxprec	= 16;

	short sgn;    /* subrange : 0..1 */
	int exp;
	char sig[MAXBUF];

	if (prec == MAXINT) 
		prec = 6;	  /* defaults to six digits */

	sgn = val < 0;
	fconvert(&val, sig, &exp, prec + 1);

	if (sgn)
		*ss++ = '-';

	if (sig[0] == '0' && !sig[1])
		*ss++ = '0';
	else {
		if (type == 'f') {
			i = exp + prec + 1;
			if (i > maxprec)
				i = maxprec;

			fconvert(&val, sig, &exp, i < 0 ? 1 : i);

			sg = sig;
			for (sg = sig; i && exp >= 0; --i, --exp)
				*ss++ = *sg++;

			while (exp >= 0) {
				*ss++ = '0';
				--exp;
			}

			if (prec)
				*ss++ = '.';

			while (prec && exp < -1) {
				*ss++ = '0';
				++exp;
				--prec;
			}

			while (--prec >= 0) {
				*ss++ = i > 0  ? *sg++ : '0';
				--i;
			}
		}
		else {
			*ss++ = sig[0];
			if (prec)
				*ss++ = '.';

			for (sg = sig + 1; --prec >= 0; *ss++ = *sg++);

			*ss++ = 'E';
			t = ss;
			nconv('d', 0, &t, (long) exp);
			ss = t;
		}
	}
	*s = ss;
}

static int _digits(s)
char **s;
{
	register int result = 0;
	register char *ss = *s;

	while (isdigit(*ss))
		result = result * 10 + *ss++ - '0';

	*s = ss;
	return result;
}

/*VARARGS1*/
int _fprintf(file, format, args)
	register FILE *file;
	register char *format;
	register char *args;
{
	register char *str;
	register int i, j, flen, pad, convchar, len;
	char *s, *temp;
	char buf[MAXBUF];
	int left, minimum, prec, longitem;

	len = 0;
	while (*format) {
		if (str = index(format, '%')) {
			if (i = str - format) {
				len += (j = fwrite(format, 1, i, file));
				if (j != i)
					break;
			}
			format = str + 1;
		}
		else {
			i = strlen(format);
			len += (j = fwrite(format, 1, i, file));
			break;
		}

		left = 0; 
		prec = MAXINT; 
		longitem = 0; 
		pad = ' ';

		if (*format == '-') {
			left = 1; 
			format++; 
		}
		if (*format == '0') {
			pad = '0'; 
			format++; 
		}
	    if (*format == '*') {		/*  added 10/22/87  rpt  */
			format++;
			minimum = *(int *)args;
			args   += sizeof minimum;
	    } else {
			temp = format; 
			minimum = _digits(&temp); 
			format = temp;
		}

		if (*format == '.') {
			format++;
			if (*format == '0') {		/* added 1/1/88  rpt  */
				pad = '0'; 
				format++;
			}
			if (*format == '*') {		/*  added 10/22/87  rpt  */
				format++;
				prec  = *(int *)args;
				args += sizeof prec;
			} else {
				temp = format; 
				prec = _digits(&temp); 
				format = temp;
			}
		}

		if (*format == 'l') {
			longitem = 1; 
			format++; 
		}

		if (!*format)
			break;

		s = str = buf;
		switch (convchar = *format++) {
			case 's':
				str = *(char **)args; 
				args += sizeof str;
				if ((flen = strlen(str)) > prec)
					flen = prec;
				pad = ' ';			/* added 1/1/88 rpt */
				break;

			case 'd':
			case 'o':
			case 'x':
			case 'X':
			case 'u':
				nconv(
					convchar, 
					longitem, 
					&s, 
					(unsigned long)
						(longitem ? 
							*(long *) args : 
							(long) *(int *) args));

				args += longitem ? sizeof(long) : sizeof(int); 

				flen = s - str;
				if (prec != MAXINT)		/* added 1/1/88  rpt */
					minimum = prec;
				break;

			case 'c':
				*s++ = *(args+1); 
				args += sizeof(int); 
				flen = 1;
				break;

			case 'e':
			case 'f':
			case 'g':
				fconv(convchar, prec, &s, *(double *)args);
				args += sizeof(double); 
				flen = s - str;
				break;

			default:
				*s++ = convchar;    /* something like %% */
				flen = 1;
				break;
		}

		if (left) {
			len += (j = fwrite(str, 1, flen, file));
			if (j != flen)
				break;
			for (i = minimum - flen; --i >= 0; len++)
				if (putc(pad, file) == EOF)
					break;

		}
		else {
			for (i = minimum - flen; --i >= 0; len++)
				if (putc(pad, file) == EOF)
					break;
			len += (j = fwrite(str, 1, flen, file));
			if (j != flen)
				break;
		}
	}

	return len;
}
