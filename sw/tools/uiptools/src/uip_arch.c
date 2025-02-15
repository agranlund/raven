/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Adam Dunkels.
 * 4. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: uip_arch.c,v 1.3 2005/02/24 22:04:19 oliverschmidt Exp $
 *
 */


#include "uip.h"
#include "uip_arch.h"
#include <stdint.h>

#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])

/*-----------------------------------------------------------------------------------*/
/* The checksum code has been copied from Freemint project:
   https://github.com/freemint/freemint
   freemint/sys/sockets/inet4/tcputil.c
*/
u16_t
upper_layer_chksum(int proto)
{
	uint8_t* dgram = (uint8_t*)&uip_buf[UIP_IPH_LEN + UIP_LLH_LEN];
	uint16_t len = (((u16_t)(BUF->len[0]) << 8) + BUF->len[1]) - UIP_IPH_LEN;
	uint32_t srcadr = *((uint32_t*)BUF->srcipaddr);
	uint32_t dstadr = *((uint32_t*)BUF->destipaddr);
	uint32_t sum = 0;

	/*
	 * Pseudo IP header checksum
	 */
	__asm__
	(
		"moveq	#0, d0		\n\t"
		"movel	%3, %0		\n\t"
		"addl	%1, %0		\n\t"
		"addxl	%2, %0		\n\t"
		"addxw	%4, %0		\n\t"
		"addxw	d0, %0		\n\t"
		: "=d"(sum)
		: "g"(srcadr), "d"(dstadr), "d"(proto),
		  "d"(len), "0"(sum)
		: "d0"
	);

	/*
	 * TCP datagram & header checksum
	 */
	__asm__
	(
		"clrl	d0		\n\t"
		"movew	%2, d1		\n\t"
		"lsrw	#4, d1		\n\t"
		"beq	4f		\n\t"
		"subqw	#1, d1		\n"	/* clears X bit */
		"1:			\n\t"
		"moveml	%4@+, d0/d2-d4	\n\t"	/* 16 byte loop */
		"addxl	d0, %0		\n\t"	/* ~5 clock ticks per byte */
		"addxl	d2, %0		\n\t"
		"addxl	d3, %0		\n\t"
		"addxl	d4, %0		\n\t"
		"dbra	d1, 1b		\n\t"
		"clrl	d0		\n\t"
		"addxl	d0, %0		\n"
		"4:			\n\t"
		"movew	%2, d1		\n\t"
		"andiw	#0xf, d1	\n\t"
		"lsrw	#2, d1		\n\t"
		"beq	2f		\n\t"
		"subqw	#1, d1		\n"
		"3:			\n\t"
		"addl	%4@+, %0	\n\t"	/* 4 byte loop */
		"addxl	d0, %0		\n\t"	/* ~10 clock ticks per byte */
		"dbra	d1, 3b		\n"
		"2:			\n\t"
		: "=d"(sum), "=a"(dgram)
		: "g"(len), "0"(sum), "1"(dgram)
		: "d0", "d1", "d2", "d3", "d4"
	);

	/*
	 * Add in extra word, byte (if len not multiple of 4).
	 * Convert to short
	 */
	__asm__
	(
		"clrl	d0		\n\t"
		"btst	#1, %2		\n\t"
		"beq	5f		\n\t"
		"addw	%4@+, %0	\n\t"	/* extra word */
		"addxw	d0, %0		\n"
		"5:			\n\t"
		"btst	#0, %2		\n\t"
		"beq	6f		\n\t"
		"moveb	%4@+, d1	\n\t"	/* extra byte */
		"lslw	#8, d1		\n\t"
		"addw	d1, %0		\n\t"
		"addxw	d0, %0		\n"
		"6:			\n\t"
		"movel	%0, d1		\n\t"	/* convert to short */
		"swap	d1		\n\t"
		"addw	d1, %0		\n\t"
		"addxw	d0, %0		\n\t"

		: "=d"(sum), "=a"(dgram)
		: "d"(len), "0"(sum), "1"(dgram)
		: "d0", "d1"
	);

  return sum;
}
/*-----------------------------------------------------------------------------------*/

static u16_t
chksum(u16_t sum16, const u8_t *data, u16_t len)
{
  uint8_t* dgram = (uint8_t*)data;
	uint32_t sum = sum16;

	/*
	 * TCP datagram & header checksum
	 */
	__asm__
	(
		"clrl	d0		\n\t"
		"movew	%2, d1		\n\t"
		"lsrw	#4, d1		\n\t"
		"beq	4f		\n\t"
		"subqw	#1, d1		\n"	/* clears X bit */
		"1:			\n\t"
		"moveml	%4@+, d0/d2-d4	\n\t"	/* 16 byte loop */
		"addxl	d0, %0		\n\t"	/* ~5 clock ticks per byte */
		"addxl	d2, %0		\n\t"
		"addxl	d3, %0		\n\t"
		"addxl	d4, %0		\n\t"
		"dbra	d1, 1b		\n\t"
		"clrl	d0		\n\t"
		"addxl	d0, %0		\n"
		"4:			\n\t"
		"movew	%2, d1		\n\t"
		"andiw	#0xf, d1	\n\t"
		"lsrw	#2, d1		\n\t"
		"beq	2f		\n\t"
		"subqw	#1, d1		\n"
		"3:			\n\t"
		"addl	%4@+, %0	\n\t"	/* 4 byte loop */
		"addxl	d0, %0		\n\t"	/* ~10 clock ticks per byte */
		"dbra	d1, 3b		\n"
		"2:			\n\t"
		: "=d"(sum), "=a"(dgram)
		: "g"(len), "0"(sum), "1"(dgram)
		: "d0", "d1", "d2", "d3", "d4"
	);

	/*
	 * Add in extra word, byte (if len not multiple of 4).
	 * Convert to short
	 */
	__asm__
	(
		"clrl	d0		\n\t"
		"btst	#1, %2		\n\t"
		"beq	5f		\n\t"
		"addw	%4@+, %0	\n\t"	/* extra word */
		"addxw	d0, %0		\n"
		"5:			\n\t"
		"btst	#0, %2		\n\t"
		"beq	6f		\n\t"
		"moveb	%4@+, d1	\n\t"	/* extra byte */
		"lslw	#8, d1		\n\t"
		"addw	d1, %0		\n\t"
		"addxw	d0, %0		\n"
		"6:			\n\t"
		"movel	%0, d1		\n\t"	/* convert to short */
		"swap	d1		\n\t"
		"addw	d1, %0		\n\t"
		"addxw	d0, %0		\n\t"

		: "=d"(sum), "=a"(dgram)
		: "d"(len), "0"(sum), "1"(dgram)
		: "d0", "d1"
	);
  /* Return sum in host byte order. */
  return sum;
}
/*---------------------------------------------------------------------------*/
u16_t
uip_chksum(u16_t *data, u16_t len)
{
  return htons(chksum(0, (u8_t *)data, len));
}
/*---------------------------------------------------------------------------*/

u16_t
uip_ipchksum(void)
{
  u16_t sum = chksum(0, &uip_buf[UIP_LLH_LEN], UIP_IPH_LEN);
  return (sum == 0) ? 0xffff : htons(sum);
}

u16_t
uip_tcpchksum(void)
{
  return upper_layer_chksum(UIP_PROTO_TCP);
}

u16_t
uip_udpchksum(void)
{
  return upper_layer_chksum(UIP_PROTO_UDP);
}