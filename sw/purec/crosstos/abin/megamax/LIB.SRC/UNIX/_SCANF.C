/* LINTLIBRARY */
#include <stdio.h>
#include <ctype.h>

#ifdef NOVOID
#define void int
#endif

static FILE *scanstream;
static char *scanstr;
static int file;
static int scanferr;

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

static int nextch()
{
	return file ? fgetc(scanstream) : *scanstr++;
}

static int skipwhite()
{
	register int ch;

	while (isspace(ch = nextch()));

	return ch;
}

static int val(ch, base)
register int ch;
int base;
{
	register int result = -1;

	if (isdigit(ch)) 
		result = ch - '0';
	else 
	if (ch >= 'a') 
		result = ch + (10 - 'a');
	else 
	if (ch >= 'A') 
		result = ch + (10 - 'A');

	if (result >= base)
		result = -1;

	if (result == -1)
		scanferr = 1;

	return result;
}

static long nconv(convchar, firstch, width)
int convchar, firstch;
register int width;
{
	register int base, ch = firstch;
	int temp, neg = 0, atleast1 = 0;
	register long result = 0;

	switch (convchar) {
		case 'd': base = 10; break;
		case 'o': base = 8; break;
		case 'x': base = 16; break;
		case 'h': base = 10; break;
	}
	if (ch == '-')
		neg = 1, ch = nextch();

	switch (base) {
		case 10:
			while (width-- && (temp = val(ch, base)) != -1) {
				result = (result << 3) + (result << 1) + temp;
				atleast1 = 1;
				ch = nextch();
			}
			break;
		case 8:
			while (width-- && (temp = val(ch, base)) != -1) {
				result = (result << 3) + temp;
				atleast1 = 1;
				ch = nextch();
			}
			break;
		case 16:
			while (width-- && (temp = val(ch, base)) != -1) {
				result = (result << 4) + temp;
				atleast1 = 1;
				ch = nextch();
			}
			break;
	}
	if (atleast1 && scanferr)	/* not an error if read at least one digit */
		scanferr = 0;
	if (file)
		(void) ungetc(ch, scanstream);
	else
		scanstr--;
	return neg ? -result : result;
}

static double fconv(ch, width)
register int ch;
register int width;
{
	double dten = 10;
	short sgn;
	register int dexp;
	int pastdot = 0, paste = 0, exp = 0, expsgn = 0, leading = 1, atleast1 = 0;
	int digcnt = 19;   /* don't accumulate more than 19 digits */
	double retval = 0;

	if (ch == '-') {
		sgn = 1;
		ch = nextch();
	}
	else
		sgn = 0;
	dexp = 0;
	while (width) {
		if (ch == '-' && paste)
			expsgn = 1;
		else if (isdigit(ch))
			if (paste)
				exp = exp*10 + ch-'0';
			else {
				if (ch != '0')
					leading = 0;
				if (!digcnt)
					dexp++;
				else {
					if (pastdot)
						dexp--;
					if (!leading) {
						retval = retval*dten + ch-'0';
						digcnt--;
					}
				}
			}
		else if (ch == '.' && !pastdot)
			pastdot = 1;
		else if ((ch == 'E' || ch == 'e') && !paste)
			paste = 1;
		else 
			break;

		atleast1 = 1;
		ch = nextch();
		width--;
	}
	if (!atleast1)
		scanferr = 1;

	if (expsgn)
		dexp -= exp;
	else
		dexp += exp;

	while (dexp)
		if (dexp < 0) {
			retval /= dten;
			dexp++;
		}
		else {
			retval *= dten;
			dexp--;
		}

	if (sgn)
		retval *= -1;

	if (file)
		(void) ungetc(ch, scanstream);
	else
		scanstr--;
	return retval;
}

/*VARARGS*/
int _scanf(_file, _scanstr, _scanstream, args)
int _file;
char *_scanstr;
FILE *_scanstream;
char **args;
{
	register int nitems = 0;	/* number of successfully converted items */
	register char *format, *sptr; 
	char *tp;
	register int fch, sch, convchar;
	int supress, longitem, width;
	double real;
	long longint;

	file = _file;
	scanstr = _scanstr;
	scanstream = _scanstream;

	scanferr = 0;
	format = *args++;
	while (*format) {
		fch = *format++;

		if (isspace(fch)) {
			sch = skipwhite();
			if (sch != EOF)
				if (file)
					(void) ungetc(sch, scanstream);
				else
					scanstr--;
			continue;	
		}

		if (fch != '%') {
			sch = skipwhite();
			if (sch == EOF)
				return EOF;
			if (sch != fch) {
				if (file)
					(void) ungetc(sch, scanstream);
				else
					scanstr--;
				return nitems;
			}
		}
		else {
			longitem = supress = 0; width = 256;
			if (*format == '*') {
				supress = 1;
				format++;
			}
			if (isdigit(*format))
				{tp = format; width = _digits(&tp); format = tp;}
			if (*format == 'l') {
				longitem = 1;
				format++; }
			convchar = *format++;

			if (convchar == 'c') {
				if ((sch = nextch()) == EOF)
					return EOF;
				if (!supress)
					**args++ = sch;
				nitems++;
			} else
			if (convchar == 'd' || convchar == 'o' || convchar == 'x' ||
				convchar == 'h') {
				if ((sch = skipwhite()) == EOF)
					return EOF;
				longint = nconv(convchar, sch, width);
				if (scanferr)
					return nitems;
				if (!supress)
					if (longitem)
						*(long *)(*args++) = longint;
					else if (convchar == 'h')
						*(char *)(*args++) = (char)longint;
					else
						*(int *)(*args++) = (int)longint;
				nitems++;
			} else
			if (convchar == 'e' || convchar == 'f') {
				if ((sch = skipwhite()) == EOF)
					return EOF;
				real = fconv(sch, width);
				if (scanferr)
					return nitems;
				if (!supress)
					if (longitem)
						*(double *)(*args++) = real;
					else
						*(float *)(*args++) = (float)real;
				nitems++;
			} else
			if (convchar == 's') {
				if ((sch = skipwhite()) == EOF)
					return EOF;
				if (!supress)
					sptr = *args++;
				while (width && sch && !isspace(sch)) {
					if (!supress)
						*sptr++ = sch;
					if ((sch = nextch()) == EOF)
						return EOF;
					width--;
				}
				*sptr++ = 0;	/* must terminate with a null */
				if (file)
					(void) ungetc(sch, scanstream);
				else
					scanstr--;
				nitems++;
			}
		}
	}
	return nitems;
}

