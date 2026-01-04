
#include "isa_bios.h"
#include "mint/basepage.h"
#include "mint/cookie.h"
#include "mint/osbind.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include "stdarg.h"


/*-----------------------------------------------------------------------------------
 * tsr heap linear allocator
 *---------------------------------------------------------------------------------*/

typedef struct {
    uint32_t start;
    uint32_t total;
    uint32_t used;
} tsrheap_t;
tsrheap_t tsrheap;

void isabios_mem_init(uint32_t size) {
    size = (size + 3) & ~3UL;
    tsrheap.used = 0;
    tsrheap.total = 0;
    tsrheap.start = (uint32_t)Malloc(size + 3);
    if (tsrheap.start) {
        tsrheap.used = 4 - (tsrheap.start & 3);
        tsrheap.total = size;
    }
}

void isabios_mem_close(void) {
    if (tsrheap.used) {
        tsrheap.total = tsrheap.used;
        Mshrink((void*)tsrheap.start, tsrheap.total);
    } else {
        Mfree((void*)tsrheap.start);
        tsrheap.start = 0;
        tsrheap.used = 0;
        tsrheap.total = 0;
    }
}

void* isabios_mem_alloc(uint32_t size) {
    void* ret = 0;
    size = ((size + 3) & ~3UL);
    if ((tsrheap.total - tsrheap.used) >= size) {
        ret = (void*) (tsrheap.start + tsrheap.used);
        tsrheap.used += size;
        memset(ret, 0, size);
    }
    return ret;
}

void* isabios_mem_alloc_temp(uint32_t size) {
    void* ret = 0;
    size = ((size + 3) & ~3UL);
    if ((tsrheap.total - tsrheap.used) >= size) {
        ret = (void*) (tsrheap.start + tsrheap.total - size);
        tsrheap.total -= size;
        memset(ret, 0, size);
    }
    return ret;
}



/*-----------------------------------------------------------------------------------
 * logging
 *---------------------------------------------------------------------------------*/

static char* printbuf;
void(*isabios_logfunc_print)(const char*);
void(*isabios_logfunc_debug)(const char*);

#if ISABIOS_ENABLE_LOG_FILE
static int flog = 0;
static void isabios_logfile_open(void)
{
    int32_t result;
    char fname[32] = "c:\\isa_bios.log";
    fname[0] = 'a' + (char) (*((volatile uint16_t*)0x446));
    result = Fcreate(fname, 0);
    if (result < 0) {
        return;
    }
    flog = (int16_t)result;
}

static void isabios_logfile_close(void)
{
    if (flog > 0) {
        Fclose(flog);
        flog = 0;
    }
}

void isabios_log(char* fmt, ...) {
    if ((flog > 0) && printbuf) {
        size_t len;
        va_list args;
        va_start (args, fmt);
        len = vsprintf(printbuf, fmt, args);
        va_end (args);
        Fwrite(flog, len, printbuf);
    }
}
#endif


#if ISABIOS_ENABLE_LOG_SCREEN
static void logfunc_print(const char* s) {
    Cconws(s);
}
void isabios_print(char* fmt, ...) {
    if (isabios_logfunc_print && printbuf) {
        va_list args;
        va_start (args, fmt);
        vsprintf(printbuf, fmt, args);
        va_end (args);
        isabios_logfunc_print(printbuf);
    }
}
#endif

#if ISABIOS_ENABLE_LOG_DEBUG
static void logfunc_dbg(const char* s) {
    Cconws(s);
}
void isabios_dbg(char* fmt, ...) {
    if (isabios_logfunc_debug && printbuf) {
        va_list args;
        va_start (args, fmt);
        vsprintf(printbuf, fmt, args);
        va_end (args);
        isabios_logfunc_debug(printbuf);
    }
}
#endif

void isabios_log_init(void) {
    printbuf = isabios_mem_alloc_temp(256);
    #if ISABIOS_ENABLE_LOG_FILE
    isabios_logfile_open();
    #endif
    #if ISABIOS_ENABLE_LOG_SCREEN
    isabios_logfunc_print = logfunc_print;
    #endif
    #if ISABIOS_ENABLE_LOG_DEBUG
    isabios_logfunc_debug = logfunc_debug;
    #endif
}

void isabios_log_close(void) {
    #if ISABIOS_ENABLE_LOG_FILE
    isabios_logfile_close();
    #endif
}


/*-----------------------------------------------------------------------------------
 * cookies!
 *---------------------------------------------------------------------------------*/

bool isabios_createcookie(uint32_t id, uint32_t value)
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
        newjar = (uint32_t*)isabios_mem_alloc(newsize);
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

