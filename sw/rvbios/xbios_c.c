/*-------------------------------------------------------------------------------
 * Raven support software
 * (c)2024 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "raven.h"

#define RTC_YEAR_OFFSET	(1968-1980)
#define RTC_NVRAM_START	0x08
#define RTC_NVRAM_END	0x1E
#define RTC_NVRAM_LEN	(RTC_NVRAM_END - RTC_NVRAM_START)

static const uint8_t default_nvram[RTC_NVRAM_LEN] = {
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0 };
	

uint8_t  int_to_bcd(uint8_t v) { return ((v % 10) + ((v / 10) << 4)); }
uint32_t bcd_to_int(uint8_t v) { return ((v & 15) + ((v >> 4) * 10)); }

uint32_t xbc_gettime(void)
{
	uint8_t r[8];
	uint32_t t=0;
	raven()->rtc_Read(0x00, r, 0x07);
	t |= bcd_to_int(r[0] & 0x7f) >>  1;		/* seconds 	*/
	t |= bcd_to_int(r[1] & 0x3f) <<  5;		/* minutes 	*/
	t |= bcd_to_int(r[2] & 0x3f) << 11;		/* hours 	*/
	t |= bcd_to_int(r[4] & 0x3f) << 16;		/* date 	*/
	t |= bcd_to_int(r[5] & 0x1f) << 21;		/* month 	*/
	t |= ((bcd_to_int(r[6] & 0xff) + RTC_YEAR_OFFSET) & 0x7f)  << 25;	/* year */
	return t;
}
void xbc_settime(
	uint32_t t)			/* d0 */
{
	uint8_t r[8];
	uint8_t y = (uint8_t)(((t>>25) - RTC_YEAR_OFFSET) & 0xff);
	r[0] = int_to_bcd((t<< 1) & 0x3f);		/* seconds 	*/
	r[1] = int_to_bcd((t>> 5) & 0x3f);		/* minutes	*/
	r[2] = int_to_bcd((t>>11) & 0x1f);		/* hours	*/
	r[3] = 0;
	r[4] = int_to_bcd((t>>16) & 0x1f);		/* date		*/
	r[5] = int_to_bcd((t>>21) & 0x0f);		/* month	*/
	r[6] = int_to_bcd((y>> 0) & 0x7f);		/* year		*/ 
	r[7] = 0;
	raven()->rtc_Write(0x00, r, 0x07);
}


int16_t xbc_nvmaccess(
	int16_t op,			/* d0 */
	int16_t start,		/* d1 */
	int16_t count,		/* d2 */
	uint8_t* buffer)	/* a0 */
{
	int32_t addr = (int32_t)start + 0x08;
	int32_t len  = (int32_t)count;
	if (addr >= RTC_NVRAM_END)
		return 0;
	if ((addr+len) > RTC_NVRAM_END)
		len = (RTC_NVRAM_END - addr);
		

	switch (op)
	{
		case 0:	/* get */
		{
			raven()->rtc_Read(addr, buffer, len);
		}
		break;
		
		case 1:	/* set */
			raven()->rtc_Write(addr, buffer, len);
		{
		}
		break;
		
		case 2:	/* clear */
		{
			raven()->rtc_Write(RTC_NVRAM_START, default_nvram, RTC_NVRAM_LEN);
		}
		break;
	}

	return 0;
}

void xbc_cache_flush(void) {
    raven()->cache_Flush();
}

void xbc_cache_enable(void) {
    raven()->cache_On();
}

void xbc_cache_disable(void) {
    raven()->cache_Off();
}