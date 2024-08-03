#include "isa_core.h"
#include "mint/basepage.h"
#include "mint/cookie.h"
#include "mint/osbind.h"
#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include "ext.h"

extern long _stksize;
static char* inf = 0;
static int32 flog = 0;


static void LoadInf()
{
    // load config file and convert in-place to key/value pairs separated by null character
    char fname[64];
    sprintf(fname, "%c:\\isa_bios.inf", 'a' + (char) (*((volatile uint16*)0x446)));
    int32 fhandle = open(fname, 0);
    if (fhandle <= 0) {
        return;
    }

	int32 fsize = lseek(fhandle, 0, SEEK_END);
    if (fsize < 2) {
        close(fhandle);
        return;
    }

    inf = malloc(fsize+1);
    lseek(fhandle, 0, SEEK_SET);
    int r = read(fhandle, inf, fsize);
    inf[fsize] = 0;
	close(fhandle);

    // skip comments until end of line
    char* src = inf; char* dst = inf;
    char* end = src + fsize;
    while (src < end) {
        char c = *src++;
        if (c == '#') {
            while ((src < end) && (*src != '\r') && (*src != '\n')) { src++; }
            while ((src < end) && ((*src == '\r') || (*src == '\n'))) { src++; }
        } else {
            *dst++ = c;
        }
    }

    // convert whitespaces and illegal characters.
    // keep spaces when inside quotation marks, but don't keep the
    // actual quotation marks themselves.
    end = dst; src = inf;
    bool keepspaces = false;
    while (src < end) {
        uint8 c = *((uint8*)src);
        if (c == 0x22) {
            keepspaces = !keepspaces;
            *src = 0;
        } else  if ((c < (keepspaces ? 0x20 : 0x21)) || (c > 0x7E) || (c == '=')) {
            *src = 0;
        }
        src++;
    }

    // extract strings
    end = src; src = inf; dst = inf; *end = 0;
    while (src < end) {
        while ((src < end) && (*src == 0)) { src++; }
        while ((src < end) && (*src != 0)) { *dst++ = *src++; }
        *dst++ = 0;
    }
}

static void CloseInf()
{
    if (inf) {
        free(inf);
        inf = 0;
    }
}

void CreateLog()
{
    char fname[64];
    sprintf(fname, "%c:\\isa_bios.log", 'a' + (char) (*((volatile uint16*)0x446)));
    flog = open(fname, O_CREAT | O_TRUNC | O_RDWR);
}

void CloseLog()
{
    if (flog > 0) {
        close(flog);
        flog = 0;
    }
}


void OpenFiles() {
    LoadInf();
    CreateLog();
}

void CloseFiles() {
    CloseInf();
    CloseLog();
}

void Log(char* fmt, ...) {
    if (flog <= 0)
        return;

    static char buf[256];
    va_list args;
    va_start (args, fmt);
    size_t len = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end (args);
    write(flog, buf, len);
}


const char* IdToStr(uint32 id) {
    static const char* hextable = "0123456789abcdef";
    static char str[8];
    uint32 idle = swap32(id);
    uint8* d = (uint8*)&idle;
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

uint32 StrToId(const char* str) {
    if (str) {
        uint32 s0 = str[0] - '@'; uint32 s1 = str[1] - '@'; uint32 s2 = str[2] - '@';
        uint32 s3 = (str[3] >= 'a') ? (str[3] - 'a') : ((str[3] >= 'A') ? (str[3] - 'A') : (str[3] - '0'));
        uint32 s4 = (str[4] >= 'a') ? (str[4] - 'a') : ((str[4] >= 'A') ? (str[4] - 'A') : (str[4] - '0'));
        uint32 s5 = (str[5] >= 'a') ? (str[5] - 'a') : ((str[5] >= 'A') ? (str[5] - 'A') : (str[5] - '0'));
        uint32 s6 = (str[6] >= 'a') ? (str[6] - 'a') : ((str[6] >= 'A') ? (str[6] - 'A') : (str[6] - '0'));
        uint32 d =  ((s6 & 0xf) << 0) | ((s5 & 0xf) << 4) | ((s4 & 0xf) << 8) | ((s3 & 0xf) << 12) |
                    ((s2 & 0x1f) << 16) | ((s1 & 0x07) << 21) | ((s1 & 0x18) << 21) | ((s0 & 0x1f) << 26);
        return swap32(d);
    }
    return 0;
}

extern int StrToInt(const char* s) {
    return s ? atoi(s) : 0;
}

extern uint32 StrToHex(const char* s) {
    uint32 result = 0;
    if (s) {
        // skip hex designators
        if (s[0] == '$') {
            s += 1;
        } else if (s[0] == '0' && s[1] == 'x') {
            s += 2;
        }
        // parse number
        for (int i=0; i<8 && *s; i++) {
            result <<= 4;
            char c = *s++;
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
            int len = strlen(src);
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
    static char buf[128];
    int maxs = *outs; *outs = 0;
    for (int i=0; i<maxs; i++) {
        outc[i] = 0;
    }
    const char* src = start ? start : inf;
    while (*src != 0) {
        const char* next = src + strlen(src) + 1;
        if (strstr(src, find) == src) {
            strncpy(buf, src, 127); buf[127] = 0;
            char* token = strtok(buf, ".");
            int count = 0;
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

bool GetInfHex(const char* key, uint32* val) {
    const char* str = GetInfStr(key);
    if (str) {
        *val = StrToHex(str);
        return true;
    }
    return false;
}

bool Createcookie(uint32 id, uint32 value)
{
    // find free slot
	int cookies_used = 0;
	int cookies_avail = 0;
	uint32* jar = (uint32*) *((uint32*)0x5a0);
	uint32* c = jar;
	while (1) {
		cookies_used++;
		if (c[0] == id) {
			c[1] = value;
			return true;
		}
		else if (c[0] == 0) {
			cookies_avail = c[1] - cookies_used;
			break;
		}
		c += 2;
	}

    // grow jar when necessary
	if (cookies_avail <= 0) {
        cookies_avail = cookies_used + 8;
        int oldsize = cookies_used * 2 * 4;
        int newsize = cookies_avail * 2 * 4;
        uint32* newjar = (uint32*)malloc(newsize);
        memcpy(newjar, jar, oldsize);
        jar = newjar;
	}

	// install cookie
	jar[(cookies_used<<1)-2] = id;
	jar[(cookies_used<<1)-1] = value;
	jar[(cookies_used<<1)+0] = 0;
	jar[(cookies_used<<1)+1] = cookies_avail - 1;
	return true;    
}

static uint32 TsrSize()
{
    BASEPAGE* bp = _base;
    uint32 size = 0x100 + bp->p_tlen + bp->p_dlen + bp->p_blen + _stksize;
    return size;
}

void ExitTsr()
{
    uint32 size = Supexec(TsrSize);
    Ptermres(size, 0);
}

static uint32 delayus_count = 0;
uint32 delayus_calibrate() {
    uint32 tick_start = *((volatile uint32*)0x4ba);
    uint32 tick_end = tick_start;
    uint32 counter = 0;
    do {
        __asm__ volatile ( "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t" : : : );
        __asm__ volatile ( "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t" : : : );
        tick_end = *((volatile uint32*)0x4ba);
        counter++;
        if (counter > 1000000) {
            return 0xffffffff;
        }
    } while ((tick_end - tick_start) <= 50);
    return counter;
}

void delayus(uint32 us)
{
    if (delayus_count == 0) {
        delayus_count = delayus_calibrate();
        return;
    }
    if ((us < 1000) && (delayus_count != 0xffffffff)) {
        uint32 loops = 1 + ((4 * delayus_count * us) / (1000 * 1000));
        for (uint32 i=0; i<=loops; i++) {
            __asm__ volatile ( "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t" : : : );
            __asm__ volatile ( "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t" : : : );
        }
        return;
    }

    // libcmini sleep resolution if 5ms
    unsigned int msec = us / 1000;
    msec = (msec > 5) ? msec : 5;
    delay(msec);
}
