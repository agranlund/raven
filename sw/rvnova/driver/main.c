/*-------------------------------------------------------------------------------
 * NOVA generic vga driver
 * (c)2025 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  whthout even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <mint/cookie.h>
#include <mint/osbind.h>

#include "emulator.h"
#include "nova.h"
#include "raven.h"

#if defined(DEBUG) && DEBUG && DEBUGPRINT_UART
static char dbgstr[256];
void dprintf_uart(char* s, ...) {
    va_list args;
    char* buf = dbgstr;
    va_start(args, s);
    vsprintf(buf, s, args);
    va_end(args); 
    while(*buf != 0) {
        while((*((uint8_t*)(RV_PADDR_UART2 + 0x14)) & (1 << 5)) == 0);
        *((char*)(RV_PADDR_UART2 + 0x00)) = *buf++;
    }
}
#endif

void setcookie(uint32_t cookie_id, uint32_t cookie_value) {
    /* find existing cookie, or a free slot */
    int32_t cookies_size = 0;
	int32_t cookies_used = 0;
    int32_t cookies_avail = 0;
	uint32_t* jar = (uint32_t*) *((uint32_t*)0x5a0);
	uint32_t* c = jar;

    while (1) {
		cookies_used++;
		if (c[0] == cookie_id) {
			c[1] = cookie_id;
			return;
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
	jar[(cookies_used<<1)-2] = cookie_id;           /* overwrite end marker */
	jar[(cookies_used<<1)-1] = cookie_value;
	jar[(cookies_used<<1)+0] = 0;                   /* write new end marker */
	jar[(cookies_used<<1)+1] = cookies_size;
}

/*-----------------------------------------------------------------------------*/

extern bool nova_prepare(void);

/*-----------------------------------------------------------------------------*/
long supermain() {
    uint32_t cookie;
    dprintf("Hello world\n");

    /* prepare nova related stuff and things */
    if (!nova_prepare()) {
        return -1;
    }

    setcookie(C_NOVA, (uint32_t)&nova);
    return 0;
}

unsigned long _StkSize = 4096;
int main()
{
    int ret = (int) Supexec(supermain);
    if (ret == 0) {
        Ptermres(_PgmSize, 0);
    }
    return ret;
}

