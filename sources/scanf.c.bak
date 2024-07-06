// =====================================================================================================================
//	IDKFAlcon - Quake engine renderer for Falcon030
// =====================================================================================================================
//
//	msscanf.c
//
//	Lightweight/standalone implementation of sscanf()
//
//	IMPORTANT:
//	- not a 100% replacement for libc version. some things still missing.
//	- may contain exotic bugs or funny corner cases specific to this implementation.
//	- mainly intended to aid parsing of text-based 3D file formats etc...
//
//	depends on: va_??? arg support
//
// =====================================================================================================================
//	d.m.l / 2015
// =====================================================================================================================

#include <stdio.h>
#include <stdarg.h>

// ---------------------------------------------------------------------------------------------------------------------
// return 1 if character is a digit 0-9
// ---------------------------------------------------------------------------------------------------------------------

static __inline int isdigit(int num)
{
    if (num >= '0' && num <= '9')
        return 1;

    return 0;
} // isdigit


// ---------------------------------------------------------------------------------------------------------------------
// return 1 if character is a digit 0-9
// ---------------------------------------------------------------------------------------------------------------------

static __inline int ishexdigit(int num)
{
    if ((num >= '0' && num <= '9') ||
            (num >= 'A' && num <= 'A') ||
            (num >= 'a' && num <= 'a'))
        return 1;

    return 0;
} // ishexdigit


// ---------------------------------------------------------------------------------------------------------------------
// return 1 if end of string
// ---------------------------------------------------------------------------------------------------------------------

static __inline int iseos(const char *p)
{
    return (0 == *p);
} // iseos


// ---------------------------------------------------------------------------------------------------------------------
// return 1 if '%' format specifier
// ---------------------------------------------------------------------------------------------------------------------

static __inline int isfmt(const char *p)
{
    return ('%' == *p);
} // isfmt


// ---------------------------------------------------------------------------------------------------------------------
// return 1 if whitespace
// ---------------------------------------------------------------------------------------------------------------------

static __inline int iswhitespace(const char *p)
{
    // todo: vertical tab '\v' and formfeed '\f' are also whitespace but not likely
    // to be encountered for typical ASCII file sources. therefore omitted!
    return ((' ' == *p) ||
            ('\n' == *p) ||
            ('\r' == *p) ||
            ('\t' == *p));
} // iswhitespace

// ---------------------------------------------------------------------------------------------------------------------
//	raise base by exponent
// ---------------------------------------------------------------------------------------------------------------------

static __inline int power(int _base, int _exponent)
{
    int i;
    int pbase = _base;

    if (_exponent == 0)
        return 1;

    for (i = 1; i < _exponent; ++i)
        pbase *= _base;

    return pbase;
} // power


// ---------------------------------------------------------------------------------------------------------------------
//	 count digits in value
// ---------------------------------------------------------------------------------------------------------------------

static __inline int digitcount(int _value)
{
    int digits = 1;

    while (_value >= 10)
    {
        ++digits;
        _value /= 10;
    }
    return digits;
} // digitcount


// ---------------------------------------------------------------------------------------------------------------------
// scan-match format string against source string until desync
// ---------------------------------------------------------------------------------------------------------------------

static __inline void scan_format(const char **ppf, const char **pps)
{
    const char* ps = *pps;
    const char* pf = *ppf;

    while (!iseos(pf) &&
           !iswhitespace(pf) &&
           !isfmt(pf) &&
           (*pf == *ps))
    {
        ++ps, ++pf;
    }

    *pps = ps;
    *ppf = pf;
} // scan_format


// ---------------------------------------------------------------------------------------------------------------------
// scan across whitespace
// ---------------------------------------------------------------------------------------------------------------------

static __inline const char * scan_ws(const char *ps)
{
    while (iswhitespace(ps))
        ps++;

    return ps;
} // scan_ws


// ---------------------------------------------------------------------------------------------------------------------
//	scan source string to float/double (includes scientific exp. notation)
// ---------------------------------------------------------------------------------------------------------------------

#define P10ZERO (38)						// index of 1e0 in powten[] table
#define P10NUM (38+1+37)					// size of powten[] table
#define P10MAX (P10NUM-1)					// highest valid index of powten[]
#define HUGE_NOR (1.70141183460469228)		// normalised HUGE_VAL
#define HUGE_VAL (1e37)						// !!!! not exactly right - but will do for most stuff

// caution: format length [_n] is ignored! will keep reading digits without limit...
static double scan_atof(const char **_s, int _n) /* convert string s to double */
{
    // this could be severely trimmed for floats, and trimmed further if the expected
    // exp range is known/limited in the source data being sscanf'ed.
    // for now its left fully specified in case of accidents...
    // could replace the table with pow() but then it depends on either (slow) libm or an FPU - so the table is preferred.
    // can also replace with power() integer function - but only for smaller exponents
    static const double powten[P10NUM] =
    {
        1e-38,1e-37,1e-36,1e-35,1e-34,1e-33,
        1e-32,1e-31,1e-30,1e-29,1e-28,1e-27,1e-26,1e-25,1e-24,1e-23,1e-22,1e-21,1e-20,1e-19,1e-18,1e-17,
        1e-16,1e-15,1e-14,1e-13,1e-12,1e-11,1e-10,1e-9,1e-8,1e-7,1e-6,1e-5,1e-4,1e-3,1e-2,1e-1,
        // ==P10ZERO
        1e0,
        //
        1e1,1e2,1e3,1e4,1e5,1e6,1e7,1e8,1e9,1e10,1e11,1e12,1e13,1e14,1e15,1e16,
        1e17,1e18,1e19,1e20,1e21,1e22,1e23,1e24,1e25,1e26,1e27,1e28,1e29,1e30,1e31,1e32,
        1e33,1e34,1e35,1e36,1e37
    };

    int j, sign, esign, exp, exs;
    double val;
    int i = 0;

    const char* s = *_s;

    // interpret sign
    sign = 1;
    if (('+' == s[i]) ||
            ('-' == s[i]))
    {
        sign = ('+' == s[i++]) ? 1 : -1;
    }

    // read digits until non-digit
    exp = P10ZERO - 1;
    val = 0.0;
    for (j = P10ZERO; isdigit(s[i]); i++)
    {
        if (j >= 0)
            val += powten[j] * (s[i] - '0');
        j--;
        exp++;
    }

    // sync & absorb decimal point
    if ('.' == s[i])
        i++;

    // continue converting digits if any remain
    for (; isdigit(s[i]); i++)
    {
        if (j >= 0)
            val += powten[j] * (s[i] - '0');
        j--;
    }

    // otherwise exponent
    if (('e' == s[i]) ||
            ('E' == s[i]) ||
            ('d' == s[i]) ||
            ('D' == s[i]))
    {
        i++;

        // interpret sign
        esign = 1;
        if (('+' == s[i]) ||
                ('-' == s[i]))
        {
            esign = ('+' == s[i++]) ? 1 : -1;
        }

        for (exs = 0; isdigit(s[i]); i++)
        {
            exs = (exs * 10) + (s[i] - '0');
        }

        exp += exs * esign;
    }

    // combine exponent
    if (exp < 0)
    {
        val = 0.0;
    }
    else
        if ((exp < P10MAX) ||
                ((exp == P10MAX) && (val < HUGE_NOR)))
        {
            val *= powten[exp];
        }
        else
        {
            val = HUGE_VAL;
        }

    *_s = &s[i];

    return (sign * val);
} // scan_atof


// ---------------------------------------------------------------------------------------------------------------------
//	scan string to integer, until non-digit or [format_length]
// ---------------------------------------------------------------------------------------------------------------------

static int scan_atoi(const char **_s, int _n)
{
    const char* string = *_s;

    int sign = 0;
    int r = 0;

    if ('-' == (*string))
    {
        ++string;
        sign = -1;
    }

    while (isdigit(*string) && (_n > 0))
    {
        r = (r * 10) + (*string - '0');
        ++string;
        --_n;
    }

    if (sign < 0)
        r = -r;

    *_s = string;

    return r;
} // scan_atoi

// ---------------------------------------------------------------------------------------------------------------------
//	scan string to unsigned integer, until non-digit or [format_length]
// ---------------------------------------------------------------------------------------------------------------------

static unsigned int scan_atou(const char **_s, int _n)
{
    unsigned int r = 0;

    const char* string = *_s;

    // ignore sign info, if present
    if ('-' == (*string))
        string++;

    while (isdigit(*string) && (_n > 0))
    {
        r = (r * 10) + (*string - '0');
        ++string;
        --_n;
    }

    *_s = string;

    return r;
} // scan_atou


// ---------------------------------------------------------------------------------------------------------------------
//	scan octal string to integer, until non-digit or [format_length]
// ---------------------------------------------------------------------------------------------------------------------

static unsigned int scan_octal(const char **_s, int _n)
{
    /*Local variables*/
    int digits, c;
    unsigned int value;
    unsigned int r = 0;

    // scan string to integer, updating stringptr
    value = scan_atou(_s, _n);

    digits = digitcount(value);

    // convert oct->dec
    for (c = 0; c < digits; c++)
    {
        r = r + (value % 10) * (unsigned int)power(8, c);
        value /= 10;
    }

    return r;
} // scan_octal

// ---------------------------------------------------------------------------------------------------------------------
//	scan hex string to integer, until non-digit or [format_length]
// ---------------------------------------------------------------------------------------------------------------------

static unsigned int scan_hexa(const char **_s, int _n)
{
    int digit = -1;
    unsigned int value = 0;
    unsigned int sum = 0;
    const char *string;
    const char *pscan;

    string = *_s;

    // skip optional '0x' hex qualifier
    if (('x' == string[1]) ||
            ('X' == string[1]))
    {
        string += 2;
    }

    // count digits
    pscan = string;
    while (ishexdigit(*pscan) && (_n > 0))
    {
        pscan++;
        digit++;
        --_n;
    }

    while (digit >= 0)
    {
        char q = *string;
        if (isdigit(q))
        {
            value = q - '0';
        }
        else
        {
            if ((q >= 'a') && (q <= 'f'))
                q -= ('a' - 'A');

            if ((q >= 'A') && (q <= 'F'))
            {
                value = (q - 'A') + 10;
            }
            else
            {
                printf("sscanf: error - '%c' is not hex digit\n", *string);
                value = 0;
            }
        }

        sum += (unsigned int)power(16, digit) * value;
        string++;
        digit--;
    }

    *_s = string;

    return sum;
} // scan_hexa

// ---------------------------------------------------------------------------------------------------------------------
//	scan string fragment to output string, until [eos], [whitespace] or [format_length]
// ---------------------------------------------------------------------------------------------------------------------

static void scan_str(const char **_s, char *_dst, int _n)
{
    const char* ps = *_s;
    char* po = _dst;

    while (!iseos(ps) &&
           !iswhitespace(ps) &&
           (_n > 0))
    {
        *po++ = *ps++;
        --_n;
    }
    // must also terminate
    *po++ = 0;

    *_s = ps;
} // scan_str


// ---------------------------------------------------------------------------------------------------------------------
//	_sscanf (implementation details)
// ---------------------------------------------------------------------------------------------------------------------

#define MFLAG_ABSORB (1 << 0)
#define MFLAG_LONG (1 << 1)
#define MFLAG_LONGDOUBLE (1 << 2)

static __inline int _sscanf(const char *_pstring, const char *_pformat, va_list _ap)
{
    int cf; //, cs;
    int insync;
    int modeflags;
    int format_length;
    int format_recovery = 0;
    int found_items = 0;

    const char *pformat = _pformat;
    const char *pstring = _pstring;

    while (!iseos(pformat))
    {
        do
        {
            //
            if (!format_recovery)
            {
                // skip [whitespace] if not recovering from '%%' special case
                pformat = scan_ws(pformat);
                pstring = scan_ws(pstring);
            }
            format_recovery = 0;

            // match [string] until % or [whitespace]
            scan_format(&pformat, &pstring);

            // if [whitespace] consume it and repeat
            insync = iswhitespace(pstring);

        } while (insync);

        modeflags = 0;

        // caution: can be arbitrarily large, but this is a fair size for sane strings in typical
        // file format readers. best rely on format length specifiers in any case.
        format_length = 256;

        if ('%' == (*pformat++))
        {
            // is [%format]

modifier_recover:

            cf = *pformat++;

            switch (cf)
            {
            // special:
            case '%':
                // back up & continue match-scanning from real '%' character
                pformat--;
                format_recovery = 1;
                break;

                // format modifiers:
            case '*': // %*...
            {
                // absorb the next parameter without assigning it to a va_arg
                modeflags |= MFLAG_ABSORB;
                // yuck
                goto modifier_recover;
            }
                break;
            case 'l': // %l...
            {
                // parameter should be interpreted as long int or double
                modeflags |= MFLAG_LONG;
                // yuck
                goto modifier_recover;
            }
                break;
                /* pointless to support on small targets?
            case 'L': // %L...
                {
                    // parameter should be interpreted as long double
                    modeflags |= MFLAG_LONGDOUBLE;
                    // yuck
                    goto modifier_recover;
                }
                break;
*/
                // note: case range is a GCC compiler extension
            case '0' ... '9': // %(format_length)
            {
                // backup & read the value
                pformat--;
                // arbitrary format_length limit of 4 digits imposed here!
                format_length = scan_atoi(&pformat, 4);
                // yuck
                goto modifier_recover;
            }
                break;

                // formats
            case 's': // %s (terminated)
            {
                if (!(modeflags & MFLAG_ABSORB))
                {
                    char *ps = va_arg(_ap, char*);
                    if (ps)
                        scan_str(&pstring, ps, format_length);
                }
                else
                {
                    char dummy[256];
                    // caution: risk of buffer overflow if large string is absorbed
                    // so risk is converted into partially-absorbed string instead.
                    // increase size of 'dummy' if required...
                    scan_str(&pstring, dummy, sizeof(dummy)-1);
                }
            }
                break;
            case 'i': // %i - decimal, hex, octal detection
            {
                if (('x' == pstring[1]) ||
                        ('X' == pstring[1]))
                {
                    unsigned int iv = scan_hexa(&pstring, format_length);
                    // todo: wasteful duplication of code
                    if (!(modeflags & MFLAG_ABSORB))
                    {
                        unsigned int *piv = va_arg(_ap, unsigned int*);
                        if (piv)
                            *piv = iv;
                    }
                    break;
                }
                else
                    if ('0' == pstring[0])
                    {
                        unsigned int iv = scan_octal(&pstring, format_length);
                        // todo: wasteful duplication of code
                        if (!(modeflags & MFLAG_ABSORB))
                        {
                            unsigned int *piv = va_arg(_ap, unsigned int*);
                            if (piv)
                                *piv = iv;
                        }
                        break;
                    }
            }
                // *** fall-through! ***
            case 'd': // %d - ok
            {
                int iv = scan_atoi(&pstring, format_length);
                // todo: wasteful duplication of code
                if (!(modeflags & MFLAG_ABSORB))
                {
                    int *piv = va_arg(_ap, int*);
                    if (piv)
                        *piv = iv;
                }
            }
                break;
            case 'u': // %u
            {
                unsigned int iv = scan_atou(&pstring, format_length);
                if (!(modeflags & MFLAG_ABSORB))
                {
                    unsigned int *piv = va_arg(_ap, unsigned int*);
                    if (piv)
                        *piv = iv;
                }
            }
                break;
            case 'a': // %a
            case 'e': // %e
            case 'f': // %f
            case 'g': // %g
            {
                double fv = scan_atof(&pstring, format_length);
                if (!(modeflags & MFLAG_ABSORB))
                {
                    /* pointless to support on small targets?
                        if (modeflags & MFLAG_LONGDOUBLE)
                        {
                            long double *pfv = va_arg(_ap, long double*);
                            if (pfv)
                                *pfv = fv;
                        }
                        else
*/						if (modeflags & MFLAG_LONG)
                    {
                        double *pfv = va_arg(_ap, double*);
                        if (pfv)
                            *pfv = fv;
                    }
                    else
                    {
                        float *pfv = va_arg(_ap, float*);
                        if (pfv)
                            *pfv = fv;
                    }
                }
            }
                break;
            case 'o': // %o - octal (optinal leading 0)
            {
                unsigned int iv = scan_octal(&pstring, format_length);
                if (!(modeflags & MFLAG_ABSORB))
                {
                    unsigned int *piv = va_arg(_ap, unsigned int*);
                    if (piv)
                        *piv = iv;
                }
            }
                break;
            case 'x': // %x - hex (optional leading 0x)
            {
                unsigned int iv = scan_hexa(&pstring, format_length);
                if (!(modeflags & MFLAG_ABSORB))
                {
                    unsigned int *piv = va_arg(_ap, unsigned int*);
                    if (piv)
                        *piv = iv;
                }
            }
                break;
            case 'c': // %c - single character
            {
                int iv = 0;
                if (!iseos(pstring))
                    iv = *pstring++;

                if (!(modeflags & MFLAG_ABSORB))
                {
                    char *piv = va_arg(_ap, char*);
                    if (piv)
                        *piv = iv;
                }
            }
                break;

            default:
                // following %format codes are not supported
            case 'p':	// not that useful...
            case 'n':	// not that useful...
            case '[':	// todo: better support this one
                printf("sscanf: error - unsupported format pattern '%%%c'\n", cf);
                return found_items;
                break;
            }
        }
        else
        {
            // not [whitespace], not [string], not [%format] = failure
            printf("sscanf: error - format:string desync\n");
            return found_items;
        }
        found_items++;
    }

    return found_items;
} // _sscanf


// ---------------------------------------------------------------------------------------------------------------------
//	sscanf - public export
// ---------------------------------------------------------------------------------------------------------------------

int sscanf(const char *ibuf, const char *fmt, ...)
{
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = _sscanf(ibuf, fmt, ap);
    va_end(ap);

    return ret;
} // sscanf

// ---------------------------------------------------------------------------------------------------------------------

