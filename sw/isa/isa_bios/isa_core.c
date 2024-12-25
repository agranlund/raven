
#include "isa_core.h"
#include "mint/basepage.h"
#include "mint/cookie.h"
#include "mint/osbind.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include "stdarg.h"

static char* inf = 0;
static int flog = 0;

static void LoadInf(void)
{
    char fname[64];
    int fhandle;
    int32_t fsize;
    char *src, *dst, *end;
    bool keepspaces;

    /* load config file and convert in-place to key/value pairs separated by null character */
    sprintf(fname, "%c:\\isa_bios.inf", 'a' + (char) (*((volatile uint16_t*)0x446)));
    fhandle = open(fname, 0);
    if (fhandle <= 0) {
        return;
    }

	fsize = lseek(fhandle, 0, SEEK_END);
    if (fsize < 2) {
        close(fhandle);
        return;
    }

    inf = malloc(fsize+1);
    lseek(fhandle, 0, SEEK_SET);
    read(fhandle, inf, fsize);
    inf[fsize] = 0;
	close(fhandle);

    /* skip comments until end of line */
    src = dst = inf;
    end = src + fsize;
    while (src < end) {
        char c = *src++;
        if (c == '#') {
            while ((src < end) && (*src != '\r') && (*src != '\n')) { src++; }
            while ((src < end) && ((*src == '\r') || (*src == '\n'))) { src++; }
        } else {
            *dst++ = c;
        }
    }

    /* convert whitespaces and illegal characters.
       keep spaces when inside quotation marks, but don't keep the
       actual quotation marks themselves. */
    end = dst; src = inf;
    keepspaces = false;
    while (src < end) {
        uint8_t c = *((uint8_t*)src);
        if (c == 0x22) {
            keepspaces = !keepspaces;
            *src = 0;
        } else  if ((c < (keepspaces ? 0x20 : 0x21)) || (c > 0x7E) || (c == '=')) {
            *src = 0;
        }
        src++;
    }

    /* extract strings */
    end = src; src = inf; dst = inf; *end = 0;
    while (src < end) {
        while ((src < end) && (*src == 0)) { src++; }
        while ((src < end) && (*src != 0)) { *dst++ = *src++; }
        *dst++ = 0;
    }
}

static void CloseInf(void)
{
    if (inf) {
        free(inf);
        inf = 0;
    }
}

void CreateLog(void)
{
    char fname[64];
    sprintf(fname, "%c:\\isa_bios.log", 'a' + (char) (*((volatile uint16_t*)0x446)));
    flog = open(fname, O_CREAT | O_TRUNC | O_RDWR);
}

void CloseLog(void)
{
    if (flog > 0) {
        close(flog);
        flog = 0;
    }
}


void OpenFiles(void) {
    LoadInf();
    CreateLog();
}

void CloseFiles(void) {
    CloseInf();
    CloseLog();
}


void Log(char* fmt, ...) {
    if (flog > 0) {
        static char buf[256];
        size_t len;
        va_list args;
        va_start (args, fmt);
#ifdef HAVE_VSNPRINTF        
        len = vsnprintf(buf, sizeof(buf), fmt, args);
#else
        len = vsprintf(buf, fmt, args);
#endif
        va_end (args);
        write(flog, buf, len);
    }
}


const char* IdToStr(uint32_t id) {
    static const char* hextable = "0123456789abcdef";
    static char str[8];
    uint32_t idle = swap32(id);
    uint8_t* d = (uint8_t*)&idle;
	str[0] = '@' + ((d[0] & 0x7c) >> 2);
	str[1] = '@' + (((d[0] & 0x3) << 3) + ((d[1] & 0xe0) >> 5));
	str[2] = '@' + (d[1] & 0x1f);
	str[3] = hextable[(d[2] >> 4)];
	str[4] = hextable[(d[2] & 0xf)];
	str[5] = hextable[(d[3] >> 4)];
	str[6] = hextable[(d[3] & 0xf)];
	str[7] = 0;
	return (const char*)str;
}

uint32_t StrToId(const char* str) {
    if (str) {
        uint32_t s0 = str[0] - '@'; uint32_t s1 = str[1] - '@'; uint32_t s2 = str[2] - '@';
        uint32_t s3 = (str[3] >= 'a') ? (str[3] - 'a') : ((str[3] >= 'A') ? (str[3] - 'A') : (str[3] - '0'));
        uint32_t s4 = (str[4] >= 'a') ? (str[4] - 'a') : ((str[4] >= 'A') ? (str[4] - 'A') : (str[4] - '0'));
        uint32_t s5 = (str[5] >= 'a') ? (str[5] - 'a') : ((str[5] >= 'A') ? (str[5] - 'A') : (str[5] - '0'));
        uint32_t s6 = (str[6] >= 'a') ? (str[6] - 'a') : ((str[6] >= 'A') ? (str[6] - 'A') : (str[6] - '0'));
        uint32_t d =  ((s6 & 0xf) << 0) | ((s5 & 0xf) << 4) | ((s4 & 0xf) << 8) | ((s3 & 0xf) << 12) |
                    ((s2 & 0x1f) << 16) | ((s1 & 0x07) << 21) | ((s1 & 0x18) << 21) | ((s0 & 0x1f) << 26);
        return swap32(d);
    }
    return 0;
}

extern int StrToInt(const char* s) {
    return s ? atoi(s) : 0;
}

extern uint32_t StrToHex(const char* s) {
    int i; uint32_t result = 0;
    if (s) {
        /* skip hex designators */
        if (s[0] == '$') {
            s += 1;
        } else if (s[0] == '0' && s[1] == 'x') {
            s += 2;
        }
        /* parse number */
        for (i=0; i<8 && *s; i++) {
            char c = *s++;
            result <<= 4;
            if (c >= '0' && c <= '9') {
                result |= (0 + (c - '0'));
            } else if (c >= 'a' && c <= 'f') {
                result |= (10 + (c - 'a'));
            } else if (c >= 'A' && c <= 'F') {
                result |= (10 + (c - 'A'));
            } else {
                i = 8;
            }
        }
    }
    return result;
}

const char* GetInfStr(const char* key) {
    if (inf) {
        const char* src = inf;
        while (*src != 0) {
            int32_t len = strlen(src);
            const char* s = strstr(src, key);
            src += (len + 1);
            if (s) {
                return src;
            }
        }
    }
    return 0;
}

const char* GetInfCommand(const char* find, const char* start, char** outc, int* outs) {
    static char buf[128]; int i; const char* src;
    int maxs = *outs; *outs = 0;
    
    src = start ? start : inf;
    for (i=0; i<maxs; i++) {
        outc[i] = 0;
    }
    while (*src != 0) {
        const char* next = src + strlen(src) + 1;
        if (strstr(src, find) == src) {
            char* token; int count;
            strncpy(buf, src, 127); buf[127] = 0;
            token = strtok(buf, ".");
            count = 0;
            while (token) {
                if (count > 0) {
                    outc[count - 1] = token;
                }
                count += 1;
                token = strtok(0, ".");
            }
            *outs = count - 1;
            return next;
        }
        src = next;
    }
    return 0;
}

bool GetInfInt(const char* key, int* val) {
    const char* str = GetInfStr(key);
    if (str) {
        *val = StrToInt(str);
        return true;
    }
    return false;
}

bool GetInfHex(const char* key, uint32_t* val) {
    const char* str = GetInfStr(key);
    if (str) {
        *val = StrToHex(str);
        return true;
    }
    return false;
}

bool Createcookie(uint32_t id, uint32_t value)
{
    /* find free slot */
    int32_t cookies_size = 0;
	int32_t cookies_used = 0;
    int32_t cookies_avail = 0;
	uint32_t* jar = (uint32_t*) *((uint32_t*)0x5a0);
	uint32_t* c = jar;
	while (1) {
		cookies_used++;
		if (c[0] == id) {
			c[1] = value;
			return true;
		}
		else if (c[0] == 0) {
            cookies_size = c[1];
			break;
		}
		c += 2;
	}

    /* grow jar when necessary */
    cookies_avail = cookies_size - cookies_used;
	if (cookies_avail <= 0) {
        uint32_t* newjar;
        int32_t oldsize = (2*4*(cookies_size + 0));
        int32_t newsize = (2*4*(cookies_size + 8));
        cookies_size += 8;
        newjar = (uint32_t*)malloc(newsize);
        memcpy(newjar, jar, oldsize);
        *((uint32_t*)0x5a0) = (uint32_t)newjar;
        jar = newjar;
	}

	/* install cookie */
	jar[(cookies_used<<1)-2] = id;                  /* overwrite end marker */
	jar[(cookies_used<<1)-1] = value;
	jar[(cookies_used<<1)+0] = 0;                   /* write new end marker */
	jar[(cookies_used<<1)+1] = cookies_size;
	return true;    
}

void ExitTsr(void)
{
    extern uint32_t _PgmSize;
    uint32_t size = _PgmSize;
    Ptermres(size, 0);
}

#if defined(__GNUC__)
static void nop(void) { __asm__ volatile ( "nop\n\t" : : : ); }
#else
static void nop(void) 0x4E71;
#endif


#define get200hz() *((volatile uint32_t*)0x4ba)

static uint32_t delayus_count = 0;
uint32_t delayus_calibrate(void) {
    uint32_t tick_start = get200hz();
    uint32_t tick_end = tick_start;
    uint32_t counter = 0;
    do {
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
        tick_end = get200hz();
        counter++;
        if (counter > 1000000UL) {
            return 0xffffffffUL;
        }
    } while ((tick_end - tick_start) <= 50);
    return counter;
}

static void delayms(uint32_t ms)
{
	uint32_t cycles = ms / 5;
	uint32_t start  = get200hz();
    cycles = (cycles < 1) ? 1 : cycles;
    while (1) {
        volatile uint32_t now = get200hz();
        if (now < start) {
            start = now;
        } else if ((now - start) >= cycles) {
            break;
        }
    }
}

void delayus(uint32_t us)
{
    if (delayus_count == 0) {
        delayus_count = delayus_calibrate();
        return;
    }
    if ((us < 1000) && (delayus_count != 0xffffffffUL)) {
        uint32_t i; uint32_t loops = 1 + ((4 * delayus_count * us) / (1000 * 1000UL));
        for (i=0; i<=loops; i++) {
            nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
            nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); 
        }
        return;
    }
    delayms(us / 1000);
}
