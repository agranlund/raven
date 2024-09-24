#include "sys.h"
#include "lib.h"
#include "hw/uart.h"

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

int strncasecmp(const char *s1, const char *s2, size_t n)
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
putchar(int c)
{
    uart_sendChar(c);
    return 0;
}

int
getc()
{
    return uart_recvChar();
    do {
        nop();
    } while((IOB(PADDR_UART2, UART_LSR) & (1 << 0)) == 0);
    return IOB(PADDR_UART2, UART_RHR);
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
        int c = getc();

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

static void emits(int (*emit)(int c), const char *s)
{
    while (*s) {
        emit(*s++);
    }
}

static void
emitx(int (*emit)(int c), uint32_t value, size_t len)
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
emitd(int (*emit)(int c), uint32_t value)
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
                uint32_t val = WSELECT(width, *(const volatile uint32_t *)p, *(const volatile uint16_t *)p, *(const volatile uint8_t *)p);
                putx(val, incr);
            }
        }

        putchar(' ');
        putchar(' ');

        for (uint32_t col = 0; col < 16; col++) {
            if ((index + col) < length) {
                const uint8_t *p = (addr + index + col);

                putchar(isprint(*p) ? *p : '.');
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
_fmt(int (*emit)(int c), const char *format, va_list ap)
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
