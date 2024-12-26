#include "sys.h"
#include "lib.h"
#include "hw/cpu.h"
#include "hw/uart.h"

static void putchar_uart(int c) { uart_sendChar(c); }
static int getchar_uart() { return uart_recvChar(); }
static int peekchar_uart() { return uart_rxrdy() ? 1 : 0; }

void (*putchar)(int c) = putchar_uart;
int (*getchar)(void) = getchar_uart;
int (*peekchar)(void) = peekchar_uart;

bool
lib_Init()
{
    putchar = putchar_uart;
    getchar = getchar_uart;
    peekchar = peekchar_uart;
    return true;
}

size_t
strlen (const char *str)
{
    unsigned int n = ~0;
    const char *cp = str;

    __asm__ volatile (
    "1:                 \n"
    "   tst.b (%0)+     \n"
    "   bne.b 1b        \n"
    : "=a" (cp), "=d" (n)
    :  "0" (cp),  "1" (n)
    : "cc");

    return (cp - str) - 1;
}

char *
strcpy (char *to, const char *from)
{
    char *pto = to;
    unsigned int n = 0xFFFF;

    __asm__ volatile (
    "1:                     \n"
    "   move.b (%0)+,(%1)+  \n"
    "   bne.b 1b            \n"
    : "=a" (from), "=a" (pto), "=d" (n)
    :  "0" (from),  "1" (pto), "2" (n)
    : "cc", "memory");
    return to;
}

int
strcmp(const char *s1, const char *s2)
{
    for (;;) {
        char c1 = *s1++;
        char c2 = *s2++;

        if (c1 < c2) {
            return -1;
        }

        if (c1 > c2) {
            return 1;
        }

        if (c1 == 0) {
            return 0;
        }
    }
}

int
strncmp(const char *s1, const char *s2, size_t n)
{
    while (n--) {
        char c1 = *s1++;
        char c2 = *s2++;

        if (c1 < c2) {
            return -1;
        }

        if (c1 > c2) {
            return 1;
        }

        if (c1 == 0) {
            return 0;
        }
    }

    return 0;
}

int
strncasecmp(const char *s1, const char *s2, size_t n)
{
    while (n--) {
        char c1 = toupper(*s1++);
        char c2 = toupper(*s2++);

        c1 = toupper(c1);
        c2 = toupper(c2);

        if (c1 < c2) {
            return -1;
        }

        if (c1 > c2) {
            return 1;
        }

        if (c1 == 0) {
            return 0;
        }
    }

    return 0;
}

char*
strchr(const char *s, char c)
{
    while (*s != '\0')
    {
        if (*s == c) {
            return (char*)s;
        }
        s++;
    }

    return 0;
}

int
memcmp(const void *vs1, const void *vs2, size_t n)
{
    const char *s1 = (const char *)vs1;
    const char *s2 = (const char *)vs2;
    while (n--) {
        char c1 = *s1++;
        char c2 = *s2++;

        if (c1 < c2) {
            return -1;
        }

        if (c1 > c2) {
            return 1;
        }
    }

    return 0;
}

void*
memset(void *b, int c, size_t len)
{
    char* ptr = (char*)b;
    char value = (char)c;
    while(len--) {
        *ptr++ = value;
    }
    return b;
}

void*
memcpy(void *restrict dst, const void *restrict src, size_t len)
{
    char *d = (char *)dst;
    const char *s = (const char *)src;
    while (len--) {
        *d++ = *s++;
    }
    return dst;
}

int
puts(const char *str)
{
    while (*str != '\0') {
        putchar(*str++);
    }
    putchar('\n');
    return 0;
}

static char *
gets_internal(char * restrict buf, int bufsize, bool echo)
{
    uint32_t len = 0;

    for (;;) {
        int c = getchar();

        if (c > 0) {
            switch (c) {
            case '\b':
                if (len > 0) {
                    len--;
                    if (echo) {
                        putchar('\b');
                        putchar(' ');
                        putchar('\b');
                    }
                }

                break;

            case 0x03: // ^c
                if (echo) {
                    puts("^C");
                }
                return NULL;

            case 0x0b: // ^k
            case 0x15: // ^u
                while (len > 0) {
                    len--;
                    if (echo) {
                        putchar('\b');
                        putchar(' ');
                        putchar('\b');
                    }
                }

                break;

            case '\r':
            case '\n':
                buf[len] = '\0';
                if (echo) {
                    putchar('\n');
                }
                return buf;

            case ' '...126:
                if ((len + 1) < bufsize) {
                    buf[len++] = c;
                    if (echo) {
                        putchar(c);
                    }
                }

                break;

            default:
                break;
            }
        }
    }
}

char *
gets(char * restrict str, int size)
{
    return gets_internal(str, size, true);
}

static const char *hextab = "0123456789abcdef";

static void emits(void (*emit)(int c), const char *s)
{
    while (*s) {
        emit(*s++);
    }
}

static void
emitx(void (*emit)(int c), uint32_t value, size_t len)
{
    uint32_t shifts = len * 2;
    char buf[shifts + 1];
    char *p = buf + shifts;

    *p = '\0';

    do {
        uint32_t nibble = value & 0xf;
        value >>= 4;
        *--p = hextab[nibble];
    } while (p > buf);

    emits(emit, p);
}

static void
putx(uint32_t value, size_t len)
{
    emitx(putchar, value, len);
}

static void
emitd(void (*emit)(int c), uint32_t value)
{
    if (value == 0) {
        putchar('0');
        return;
    }

    char buf[11];
    char *p = buf + sizeof(buf) - 1;
    *p = '\0';

    while (value > 0) {
        uint32_t digit = value % 10;
        value /= 10;
        *--p = hextab[digit];
    }

    emits(emit, p);
}

// static void
// putd(uint32_t value)
// {
//     emitd(putchar, value);
// }

#define WSELECT(_s, _l, _w, _b) ((_s) == 'l') ? (_l) : ((_s) == 'w') ? (_w) : (_b)

size_t
hexdump(const uint8_t *addr, uint32_t address, size_t length, char width)
{
    uint32_t index;
    uint32_t incr = WSELECT(width, 4, 2, 1);

    length &= ~(incr - 1);

    for (index = 0; index < length; index += 16) {
        putx(address + index, 4);
        putchar(':');

        for (uint32_t col = 0; col < 16; col += incr) {
            putchar(' ');

            if ((index + col) >= length) {
                uint32_t count = WSELECT(width, 8, 4, 2);

                while (count--) {
                    putchar(' ');
                }
            } else {
                const uint8_t *p = (addr + index + col);
                uint32_t val = 0xffffffff;

                switch (width)
                {
                    case 'l':
                        val = cpu_SafeReadLong((uint32_t)p);
                        break;
                    case 'w':
                        val = cpu_SafeReadWord((uint32_t)p);
                        break;
                    default:
                        val = cpu_SafeReadByte((uint32_t)p);
                        break;
                }
                putx(val, incr);
            }
        }

        putchar(' ');
        putchar(' ');

        for (uint32_t col = 0; col < 16; col++) {
            if ((index + col) < length) {
                const uint8_t *p = (addr + index + col);
                const uint8_t v = cpu_SafeReadByte((uint32_t)p);
                putchar(isprint(v) ? v : '.');
            } else {
                putchar(' ');
            }
        }

        putchar('\n');
    }

    return length;
}

/**
 * @brief      printf-style output formatter
 *
 * Supports:
 *  %c      character (char)
 *  %d      signed decimal integer (int)
 *  %u      uint32_t decimal integer (uint32_t int)
 *  %p      pointer (32-bit hex) (const void *)
 *  %b      hex byte (uint8_t)
 *  %w      hex word (uint16_t)
 *  %l      hex long (uint32_t)
 *  %s      string (const char *)
 *
 * @param[in]  format     format string
 * @param[in]  <unnamed>  format string arguments
 */
void
_fmt(void (*emit)(int c), const char *format, va_list ap)
{
    char c;
    bool dofmt = false;

    while ((c = *format++) != 0) {
        if (!dofmt) {
            if (c == '%') {
                dofmt = true;
            } else {
                emit(c);
            }

            continue;
        }

        dofmt = false;

        switch (c) {
        case 'c': {
                char c = va_arg(ap, int);
                emit(c);
                break;
            }

        case 'd': {
                int v = va_arg(ap, int);

                if (v < 0) {
                    emit('-');
                    v = -v;
                }

                emitd(emit, v);
                break;
            }

        case 'u': {
                uint32_t v = va_arg(ap, uint32_t);
                emitd(emit, v);
                break;
            }

        case 'p': {
                void *v = va_arg(ap, void *);
                emits(emit, "0x");
                emitx(emit, (uint32_t)v, sizeof(v));
                break;
            }

        case 'b': {
                uint8_t v = va_arg(ap, uint32_t);
                emitx(emit, v, sizeof(v));
                break;
            }

        case 'w': {
                uint16_t v = va_arg(ap, uint32_t);
                emitx(emit, v, sizeof(v));
                break;
            }

        case 'l': {
                uint32_t v = va_arg(ap, uint32_t);
                emitx(emit, v, sizeof(v));
                break;
            }

        case 's': {
                const char *v = va_arg(ap, const char *);
                emits(emit, v);
                break;
            }

        case '%': {
            emit('%');
            break;
        }

        default:
            emit('%');
            emit(c);
            break;
        }
    }
}

void
fmt(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _fmt(putchar, format, ap);
    va_end(ap);
}

static int
scan_decval(char c)
{
    if ((c >= '0') && (c <= '9')) {
        return c - '0';
    }

    return -1;
}

static int
scan_hexval(char c)
{
    if ((c >= '0') && (c <= '9')) {
        return c - '0';
    }

    if ((c >= 'a') && (c <= 'f')) {
        return c - 'a' + 10;
    }

    if ((c >= 'A') && (c <= 'F')) {
        return c - 'A' + 10;
    }

    return -1;
}

static int
scan_digits(const char **bp, uint32_t *result)
{
    int (*scanner)(char c);
    const char *p = *bp;
    *result = 0;
    int scaler = 0;

    // auto-detect hex vs. decimal
    if ((strlen(p) >= 3) &&
        !strncmp(p, "0x", 2) &&
        (scan_hexval(*(p + 2)) >= 0)) {
        scanner = scan_hexval;
        scaler = 16;
        p += 2;
    } else if ((strlen(p) >= 2) &&
               (*p == '$') &&
               (scan_hexval(*(p + 1)) >= 0)) {
        scanner = scan_hexval;
        scaler = 16;
        p += 1;
    } else if (scan_decval(*p) >= 0) {
        scanner = scan_decval;
        scaler = 10;
    } else {
        return -1;
    }

    int digit;

    while ((digit = scanner(*p)) >= 0) {
        *result *= scaler;
        *result += digit;
        p++;
    }

    *bp = p;
    return 0;
}

/**
 * @brief      scanf-style input scanner
 *
 * Supports:
 *  %c      character (char *)
 *  %l      uint32_t number, decimal or hex with preceding 0x or $ (uint32_t *)
 *  %s      string, needs 2 args, pointer & max length (char *, size_t)
 *
 * @param[in]  buf        buffer to scan
 * @param[in]  format     format string
 * @param[in]  <unnamed>  format string arguments
 *
 * @return     the number of arguments converted, or -1 on error
 */
int
scan(const char *buf, const char *format, ...)
{
    char c;
    va_list ap;
    int ret = 0;
    bool dofmt = 0;

    va_start(ap, format);

    while ((c = *format++) != 0) {
        // input string exhausted; success if we made any conversions
        if (*buf == 0) {
            return ret > 0 ? ret : -1;
        }

        if (!dofmt) {
            // any space in the format discards space in the buffer
            if (isspace(c)) {
                while (isspace(*buf)) {
                    buf++;
                }

                continue;
            }

            // any non-space in the format must match in the buffer
            if (c != '%') {
                if (*buf++ != c) {
                    return -1;
                }
            } else {
                dofmt = true;
            }

            continue;
        }

        dofmt = false;

        // leading whitespace before conversions is always discarded
        while (isspace(*buf)) {
            buf++;
        }

        void *vvp = va_arg(ap, void *);

        if (vvp == 0) {
            return -1;
        }

        switch (c) {
        case 'c': {
                *(char *)vvp = *buf++;
                ret++;
                break;
            }

        case 'l': {
                uint32_t *vp = (uint32_t *)vvp;

                if (scan_digits(&buf, vp) < 0) {
                    return -1;
                }

                ret++;
                break;
            }

        case 's': {
                char *vp = (char *)vvp;
                size_t len = va_arg(ap, size_t);

                if (len < 1) {
                    return -1;
                }

                uint32_t index = 0;

                for (;;) {
                    if ((*buf == 0) || isspace(*buf)) {
                        break;
                    }

                    char c = *buf++;

                    if ((index + 1) < len) {
                        vp[index++] = c;
                    }
                }

                vp[index] = '\0';
                ret++;
                break;
            }

        default:
            if (*buf++ != c) {
                return -1;
            }

            break;
        }
    }

    return ret;
}

// -------------------------------------------------------------------------------------------

int setjmp(jmp_buf buf)
{
	register long *a0 __asm__("%a0") = buf;
	register void *a1 __asm__("%a1") = __builtin_return_address(0);
	__asm__ __volatile__(
		"\tmovem.l	%%d2-%%d7/%%a1-%%a7,(%[regs])\n"
#if 0
#ifdef __mcffpu__
		"\tfmovem%.d %%fp0-%%fp7,52(%[regs])\n"
#endif
#ifdef __HAVE_68881__
		"\tfmovem%.x %%fp0-%%fp7,52(%[regs])\n"
#endif
#endif
		:							/* output */
		: [regs] "a" (a0), "a"(a1)	/* input */
		: "memory"
	);
	return 0;
}

void longjmp(jmp_buf buf, int val)
{
	register int d0 __asm__("%d0") = val ? val : 1;
	register long *a0 __asm__("%a0") = buf;

	__asm__ __volatile__(
		"\tmovem.l	(%[a0]),%%d2-%%d7/%%a1-%%a7\n"
#if 0
#ifdef __mcffpu__
		"\tfmovem%.d 52(%[a0]),%%fp0-%%fp7\n"
#endif
#ifdef __HAVE_68881__
		"\tfmovem%.x 52(%[a0]),%%fp0-%%fp7\n"
#endif
#endif
		"\taddq.l #4,%%a7\n"		/* pop return pc of setjmp() call */
		"\tjmp (%%a1)\n"
		:							/* output */
		: [a0]"a"(a0), "d" (d0)
		: /* not reached; so no need to declare any clobbered regs */
	);
	__builtin_unreachable();
}

// -------------------------------------------------------------------------------------------


/********************************************************************/

typedef struct
{
    int dest;
    void (*func)(char);
    char *loc;
} PRINTK_INFO;

#define DEST_CONSOLE    (1)
#define DEST_STRING     (2)

#define FLAGS_MINUS     (0x01)
#define FLAGS_PLUS      (0x02)
#define FLAGS_SPACE     (0x04)
#define FLAGS_ZERO      (0x08)
#define FLAGS_POUND     (0x10)

#define IS_FLAG_MINUS(a)    (a & FLAGS_MINUS)
#define IS_FLAG_PLUS(a)     (a & FLAGS_PLUS)
#define IS_FLAG_SPACE(a)    (a & FLAGS_SPACE)
#define IS_FLAG_ZERO(a)     (a & FLAGS_ZERO)
#define IS_FLAG_POUND(a)    (a & FLAGS_POUND)

#define LENMOD_h        (0x01)
#define LENMOD_l        (0x02)
#define LENMOD_L        (0x04)

#define IS_LENMOD_h(a)  (a & LENMOD_h)
#define IS_LENMOD_l(a)  (a & LENMOD_l)
#define IS_LENMOD_L(a)  (a & LENMOD_L)

#define FMT_d   (0x0001)
#define FMT_o   (0x0002)
#define FMT_x   (0x0004)
#define FMT_X   (0x0008)
#define FMT_u   (0x0010)
#define FMT_c   (0x0020)
#define FMT_s   (0x0040)
#define FMT_p   (0x0080)
#define FMT_n   (0x0100)

#define IS_FMT_d(a)     (a & FMT_d)
#define IS_FMT_o(a)     (a & FMT_o)
#define IS_FMT_x(a)     (a & FMT_x)
#define IS_FMT_X(a)     (a & FMT_X)
#define IS_FMT_u(a)     (a & FMT_u)
#define IS_FMT_c(a)     (a & FMT_c)
#define IS_FMT_s(a)     (a & FMT_s)
#define IS_FMT_p(a)     (a & FMT_p)
#define IS_FMT_n(a)     (a & FMT_n)

static void board_putchar(char c) {
    if (c != '\r') {
        putchar((int)c);
    }
}

/********************************************************************/
static void printk_putc(char c, int *count, PRINTK_INFO *info)
{
    switch (info->dest)
    {
        case DEST_CONSOLE:
            info->func(c);
            break;
        case DEST_STRING:
            *(info->loc) = (unsigned char)c;
            ++(info->loc);
            break;
        default:
            break;
    }
    *count += 1;
}

/********************************************************************/
static int printk_mknumstr(char *numstr, void *nump, int neg, int radix)
{
    int a,b,c;
    unsigned int ua,ub,uc;

    int nlen;
    char *nstrp;

    nlen = 0;
    nstrp = numstr;
    *nstrp++ = '\0';

    if (neg)
    {
        a = *(int *)nump;
        if (a == 0)
        {
            *nstrp = '0';
            ++nlen;
            goto done;
        }
        while (a != 0)
        {
            b = (int)a / (int)radix;
            c = (int)a - ((int)b * (int)radix);
            if (c < 0)
            {
                c = ~c + 1 + '0';
            }
            else
            {
                c = c + '0';
            }
            a = b;
            *nstrp++ = (char)c;
            ++nlen;
        }
    }
    else
    {
        ua = *(unsigned int *)nump;
        if (ua == 0)
        {
            *nstrp = '0';
            ++nlen;
            goto done;
        }
        while (ua != 0)
        {
            ub = (unsigned int)ua / (unsigned int)radix;
            uc = (unsigned int)ua - ((unsigned int)ub * (unsigned int)radix);
            if (uc < 10)
            {
                uc = uc + '0';
            }
            else
            {
                uc = uc - 10 + 'A';
            }
            ua = ub;
            *nstrp++ = (char)uc;
            ++nlen;
        }
    }
    done:
    return nlen;
}

/********************************************************************/
static void printk_pad_zero(int curlen, int field_width, int *count, PRINTK_INFO *info)
{
    int i;

    for (i = curlen; i < field_width; i++)
    {
        printk_putc('0',count, info);
    }
}

/********************************************************************/
static void printk_pad_space(int curlen, int field_width, int *count, PRINTK_INFO *info)
{
    int i;

    for (i = curlen; i < field_width; i++)
    {
        printk_putc(' ',count, info);
    }
}

/********************************************************************/
int printk(PRINTK_INFO *info, const char *fmt, va_list ap)
{
    /* va_list ap; */
    char *p;
    char c;

    static char vstr[33];
    char *vstrp;
    int vlen;

    int done;
    int count = 0;

    int flags_used;
    int field_width;
#if 0
    int precision_used;
    int precision_width;
    int length_modifier;
#endif

    int ival;
    char schar, dschar;
    int *ivalp;
    char *sval;
    char cval;
    unsigned int uval;

    /*
     * Start parsing apart the format string and display appropriate
     * formats and data.
     */
    for (p = (char *)fmt; (c = *p) != 0; p++)
    {
        /*
         * All formats begin with a '%' marker.  Special chars like
         * '\n' or '\t' are normally converted to the appropriate
         * character by the __compiler__.  Thus, no need for this
         * routine to account for the '\' character.
         */
        if (c != '%')
        {
            /*
             * This needs to be replaced with something like
             * 'board_putchar()' or call an OS routine.
             */
#if 0 // #ifndef UNIX_DEBUG
            if (c != '\n')
            {
                printk_putc(c, &count, info);
            }
            else
            {
                printk_putc(0x0D /* CR */, &count, info);
                printk_putc(0x0A /* LF */, &count, info);
            }
#else
            printk_putc(c, &count, info);
#endif

            /*
             * By using 'continue', the next iteration of the loop
             * is used, skipping the code that follows.
             */
            continue;
        }

        /*
         * First check for specification modifier flags.
         */
        flags_used = 0;
        done = FALSE;
        while (!done)
        {
            switch (/* c = */ *++p)
            {
                case '-':
                    flags_used |= FLAGS_MINUS;
                    break;
                case '+':
                    flags_used |= FLAGS_PLUS;
                    break;
                case ' ':
                    flags_used |= FLAGS_SPACE;
                    break;
                case '0':
                    flags_used |= FLAGS_ZERO;
                    break;
                case '#':
                    flags_used |= FLAGS_POUND;
                    break;
                default:
                    /* we've gone one char too far */
                    --p;
                    done = TRUE;
                    break;
            }
        }

        /*
         * Next check for minimum field width.
         */
        field_width = 0;
        done = FALSE;
        while (!done)
        {
            switch (c = *++p)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    field_width = (field_width * 10) + (c - '0');
                    break;
                default:
                    /* we've gone one char too far */
                    --p;
                    done = TRUE;
                    break;
            }
        }

        /*
         * Next check for the width and precision field separator.
         */
        if (/* (c = *++p) */ *++p == '.')
        {
            /* precision_used = TRUE; */

            /*
             * Must get precision field width, if present.
             */
            /* precision_width = 0; */
            done = FALSE;
            while (!done)
            {
                switch (/* c = uncomment if used below */ *++p)
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
#if 0
                        precision_width = (precision_width * 10) +
                            (c - '0');
#endif
                        break;
                    default:
                        /* we've gone one char too far */
                        --p;
                        done = TRUE;
                        break;
                }
            }
        }
        else
        {
            /* we've gone one char too far */
            --p;
#if 0
            precision_used = FALSE;
            precision_width = 0;
#endif
        }

        /*
         * Check for the length modifier.
         */
        /* length_modifier = 0; */
        switch (/* c = */ *++p)
        {
            case 'h':
                /* length_modifier |= LENMOD_h; */
                break;
            case 'l':
                /* length_modifier |= LENMOD_l; */
                break;
            case 'L':
                /* length_modifier |= LENMOD_L; */
                break;
            default:
                /* we've gone one char too far */
                --p;
                break;
        }

        /*
         * Now we're ready to examine the format.
         */
        switch (c = *++p)
        {
            case 'd':
            case 'i':
                ival = (int)va_arg(ap, int);
                vlen = printk_mknumstr(vstr,&ival,TRUE,10);
                vstrp = &vstr[vlen];

                if (ival < 0)
                {
                    schar = '-';
                    ++vlen;
                }
                else
                {
                    if (IS_FLAG_PLUS(flags_used))
                    {
                        schar = '+';
                        ++vlen;
                    }
                    else
                    {
                        if (IS_FLAG_SPACE(flags_used))
                        {
                            schar = ' ';
                            ++vlen;
                        }
                        else
                        {
                            schar = 0;
                        }
                    }
                }
                dschar = FALSE;
            
                /*
                 * do the ZERO pad.
                 */
                if (IS_FLAG_ZERO(flags_used))
                {
                    if (schar)
                        printk_putc(schar, &count, info);
                    dschar = TRUE;
            
                    printk_pad_zero (vlen, field_width, &count, info);
                    vlen = field_width;
                }
                else
                {
                    if (!IS_FLAG_MINUS(flags_used))
                    {
                        printk_pad_space (vlen, field_width, &count, info);
            
                        if (schar)
                            printk_putc(schar, &count, info);
                        dschar = TRUE;
                    }
                }
            
                /* the string was built in reverse order, now display in */
                /* correct order */
                if (!dschar && schar)
                {
                    printk_putc(schar, &count, info);
                }
                goto cont_xd;

            case 'x':
            case 'X':
                uval = (unsigned int)va_arg(ap, unsigned int);
                vlen = printk_mknumstr(vstr,&uval,FALSE,16);
                vstrp = &vstr[vlen];

                dschar = FALSE;
                if (IS_FLAG_ZERO(flags_used))
                {
                    if (IS_FLAG_POUND(flags_used))
                    {
                        printk_putc('0', &count, info);
                        printk_putc('x', &count, info);
                        /*vlen += 2;*/
                        dschar = TRUE;
                    }
                    printk_pad_zero (vlen, field_width, &count, info);
                    vlen = field_width;
                }
                else
                {
                    if (!IS_FLAG_MINUS(flags_used))
                    {
                        if (IS_FLAG_POUND(flags_used))
                        {
                            vlen += 2;
                        }
                        printk_pad_space (vlen, field_width, &count, info);
                        if (IS_FLAG_POUND(flags_used))
                        {
                            printk_putc('0', &count, info);
                            printk_putc('x', &count, info);
                            dschar = TRUE;
                        }
                    }
                }

                if ((IS_FLAG_POUND(flags_used)) && !dschar)
                {
                    printk_putc('0', &count, info);
                    printk_putc('x', &count, info);
                    vlen += 2;
                }
                goto cont_xd;

            case 'o':
                uval = (unsigned int)va_arg(ap, unsigned int);
                vlen = printk_mknumstr(vstr,&uval,FALSE,8);
                goto cont_u;
            case 'b':
                uval = (unsigned int)va_arg(ap, unsigned int);
                vlen = printk_mknumstr(vstr,&uval,FALSE,2);
                goto cont_u;
            case 'p':
                uval = (unsigned int)va_arg(ap, void *);
                vlen = printk_mknumstr(vstr,&uval,FALSE,16);
                goto cont_u;
            case 'u':
                uval = (unsigned int)va_arg(ap, unsigned int);
                vlen = printk_mknumstr(vstr,&uval,FALSE,10);

                cont_u:
                    vstrp = &vstr[vlen];

                    if (IS_FLAG_ZERO(flags_used))
                    {
                        printk_pad_zero (vlen, field_width, &count, info);
                        vlen = field_width;
                    }
                    else
                    {
                        if (!IS_FLAG_MINUS(flags_used))
                        {
                            printk_pad_space (vlen, field_width, &count, info);
                        }
                    }

                cont_xd:
                    while (*vstrp)
                        printk_putc(*vstrp--, &count, info);

                    if (IS_FLAG_MINUS(flags_used))
                    {
                        printk_pad_space (vlen, field_width, &count, info);
                    }
                break;

            case 'c':
                cval = (char)va_arg(ap, unsigned int);
                printk_putc(cval,&count, info);
                break;
            case 's':
                sval = (char *)va_arg(ap, char *);
                if (sval)
                {
                    vlen = strlen(sval);
                    if (!IS_FLAG_MINUS(flags_used))
                    {
                        printk_pad_space (vlen, field_width, &count, info);
                    }
                    while (*sval)
                        printk_putc(*sval++,&count, info);
                    if (IS_FLAG_MINUS(flags_used))
                    {
                        printk_pad_space (vlen, field_width, &count, info);
                    }
                }
                break;
            case 'n':
                ivalp = (int *)va_arg(ap, int *);
                *ivalp = count;
                break;
            default:
                printk_putc(c,&count, info);
                break;
        }
    }
    return count;
}

/********************************************************************/
void printf(const char *format, ...)
{
    va_list ap;
    //int rvalue;
    PRINTK_INFO info;
    info.dest = DEST_CONSOLE;
    info.func = &board_putchar;
    va_start(ap, format);
    /*rvalue =*/ printk(&info, format, ap);
    va_end(ap);
}
