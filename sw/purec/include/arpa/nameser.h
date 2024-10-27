/*
 * Copyright (c) 1983, 1989 Regents of the University of California.
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
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)nameser.h	5.25 (Berkeley) 4/3/91
 */

#ifndef _ARPA_NAMESER_H_
#define _ARPA_NAMESER_H_

#ifndef	_FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

/*
 * Define constants based on RFC 883, RFC 1034, RFC 1035
 */
#define NS_PACKETSZ	512	/*%< default UDP packet size */
#define NS_MAXDNAME	256	/*%< maximum domain name */
#define NS_MAXMSG	65535	/*%< maximum message size */
#define NS_MAXCDNAME	255	/*%< maximum compressed domain name */
#define NS_MAXLABEL	63	/*%< maximum length of domain label */
#define NS_HFIXEDSZ	12	/*%< #/bytes of fixed data in header */
#define NS_QFIXEDSZ	4	/*%< #/bytes of fixed data in query */
#define NS_RRFIXEDSZ	10	/*%< #/bytes of fixed data in r record */
#define NS_INT32SZ	4	/*%< #/bytes of data in a u_int32_t */
#define NS_INT16SZ	2	/*%< #/bytes of data in a u_int16_t */
#define NS_INT8SZ	1	/*%< #/bytes of data in a u_int8_t */
#define NS_INADDRSZ	4	/*%< IPv4 T_A */
#define NS_IN6ADDRSZ	16	/*%< IPv6 T_AAAA */
#define NS_CMPRSFLGS	0xc0	/*%< Flag bits indicating name compression. */
#define NS_DEFAULTPORT	53	/*%< For both TCP and UDP. */

#define PACKETSZ	NS_PACKETSZ
#define MAXDNAME	NS_MAXDNAME
#define MAXCDNAME	NS_MAXCDNAME
#define MAXLABEL	NS_MAXLABEL
#define QFIXEDSZ	NS_QFIXEDSZ
#define RRFIXEDSZ	NS_RRFIXEDSZ
#define HFIXEDSZ	NS_HFIXEDSZ
#define NAMESERVER_PORT	NS_DEFAULTPORT

/*
 * Currently defined opcodes
 */
#define QUERY		0x0		/* standard query */
#define IQUERY		0x1		/* inverse query */
#define STATUS		0x2		/* nameserver status query */
/*#define xxx		0x3		   0x3 reserved */
	/* non standard */
#define UPDATEA		0x9		/* add resource record */
#define UPDATED		0xa		/* delete a specific resource record */
#define UPDATEDA	0xb		/* delete all nemed resource record */
#define UPDATEM		0xc		/* modify a specific resource record */
#define UPDATEMA	0xd		/* modify all named resource record */

#define ZONEINIT	0xe		/* initial zone transfer */
#define ZONEREF		0xf		/* incremental zone referesh */

/*
 * Currently defined response codes
 */
#define NOERROR		0		/* no error */
#define FORMERR		1		/* format error */
#define SERVFAIL	2		/* server failure */
#define NXDOMAIN	3		/* non existent domain */
#define NOTIMP		4		/* not implemented */
#define REFUSED		5		/* query refused */
	/* non standard */
#define NOCHANGE	0xf		/* update failed to change db */

/*
 * Type values for resources and queries
 */
#define T_A		1		/* host address */
#define T_NS		2		/* authoritative server */
#define T_MD		3		/* mail destination */
#define T_MF		4		/* mail forwarder */
#define T_CNAME		5		/* connonical name */
#define T_SOA		6		/* start of authority zone */
#define T_MB		7		/* mailbox domain name */
#define T_MG		8		/* mail group member */
#define T_MR		9		/* mail rename name */
#define T_NULL		10		/* null resource record */
#define T_WKS		11		/* well known service */
#define T_PTR		12		/* domain name pointer */
#define T_HINFO		13		/* host information */
#define T_MINFO		14		/* mailbox information */
#define T_MX		15		/* mail routing information */
#define T_TXT		16		/* text strings */
#define T_RP		17		/* responsible serson */
#define T_AFSDB		18		/* AFS Data Base location */
#define T_X25		19		/* public switched data network id */
#define T_ISDN		20		/* integrated service digital network */
#define T_RT		21		/* route through */
#define T_NSAP		22		/* RFC 1348, RFC 1637 */
#define T_NSAP_PTR	23		/* RFC 1348, RFC 1637 */
#define T_SIG		24		/* signature, RFC 2065 */
#define	T_KEY		25		/* associated key, RFC 2065 */
#define T_PX		26		/* preference, RFC 1664 */
#define T_GPOS		27		/* geographical location, RFC 1712 */
#define T_AAAA		28		/* ipv6 host address */
#define T_LOC		29		/* location information, RFC 1876 */
	/* non standard */
#define T_UINFO		100		/* user (finger) information */
#define T_UID		101		/* user ID */
#define T_GID		102		/* group ID */
#define T_UNSPEC	103		/* Unspecified format (binary data) */
	/* Query type values which do not appear in resource records */
#define T_AXFR		252		/* transfer zone of authority */
#define T_MAILB		253		/* transfer mailbox records */
#define T_MAILA		254		/* transfer mail agent records */
#define T_ANY		255		/* wildcard match */

/*
 * Values for class field
 */

#define C_IN		1		/* the arpa internet */
#define C_CHAOS		3		/* for chaos net at MIT */
#define C_HS		4		/* for Hesiod name server at MIT */
	/* Query class values which do not appear in resource records */
#define C_ANY		255		/* wildcard match */

/*
 * Status return codes for T_UNSPEC conversion routines
 */
#define CONV_SUCCESS 0
#define CONV_OVERFLOW -1
#define CONV_BADFMT -2
#define CONV_BADCKSUM -3
#define CONV_BADBUFLEN -4

#include <endian.h>
#ifndef BYTE_ORDER
# define BYTE_ORDER __BYTE_ORDER
#endif
#ifndef BIG_ENDIAN
# define BIG_ENDIAN __BIG_ENDIAN
#endif
#ifndef LITTLE_ENDIAN
# define LITTLE_ENDIAN __LITTLE_ENDIAN
#endif
/*
 * Structure for query header, the order of the fields is machine and
 * compiler dependent, in our case, the bits within a byte are assigned
 * least significant first, while the order of transmition is most
 * significant first.  This requires a somewhat confusing rearrangement.
 */

/*
 * Pure-C does not allow to use anything else but
 * int to be used as bitfield. But for other compilers,
 * int might be too large for correct alignment of following members
 */
#ifndef __bitfield_type
#ifdef __PUREC__
#define __bitfield_type unsigned int
#else
#define __bitfield_type unsigned char
#endif
#endif

typedef struct {
	unsigned short	id;		/* query identification number */
#if BYTE_ORDER == BIG_ENDIAN
			/* fields in third byte */
	__bitfield_type	qr:1;		/* response flag */
	__bitfield_type	opcode:4;	/* purpose of message */
	__bitfield_type	aa:1;		/* authoritive answer */
	__bitfield_type	tc:1;		/* truncated message */
	__bitfield_type	rd:1;		/* recursion desired */
			/* fields in fourth byte */
	__bitfield_type	ra:1;		/* recursion available */
	__bitfield_type	pr:1;		/* primary server required (non standard) */
	__bitfield_type	unused:2;	/* unused bits */
	__bitfield_type	rcode:4;	/* response code */
#endif
#if BYTE_ORDER == LITTLE_ENDIAN || BYTE_ORDER == PDP_ENDIAN
			/* fields in third byte */
	__bitfield_type	rd:1;		/* recursion desired */
	__bitfield_type	tc:1;		/* truncated message */
	__bitfield_type	aa:1;		/* authoritive answer */
	__bitfield_type	opcode:4;	/* purpose of message */
	__bitfield_type	qr:1;		/* response flag */
			/* fields in fourth byte */
	__bitfield_type	rcode:4;	/* response code */
	__bitfield_type	unused:2;	/* unused bits */
	__bitfield_type	pr:1;		/* primary server required (non standard) */
	__bitfield_type	ra:1;		/* recursion available */
#endif
			/* remaining bytes */
	unsigned short	qdcount;	/* number of question entries */
	unsigned short	ancount;	/* number of answer entries */
	unsigned short	nscount;	/* number of authority entries */
	unsigned short	arcount;	/* number of resource entries */
} HEADER;

/*
 * Defines for handling compressed domain names
 */
#define INDIR_MASK	0xc0

/*
 * Structure for passing resource records around.
 */
struct rrec {
	short	r_zone;			/* zone number */
	short	r_class;		/* class number */
	short	r_type;			/* type number */
	unsigned long	r_ttl;			/* time to live */
	long	r_size;			/* size of data area */
					/* kr: was int */
	char	*r_data;		/* pointer to data */
};

extern	unsigned short	_getshort(const unsigned char *);
extern	unsigned long	_getlong(const unsigned char *);

__END_DECLS

/*
 * Inline versions of get/put short/long.
 * Pointer is advanced; we assume that both arguments
 * are lvalues and will already be in registers.
 * cp MUST be unsigned char *.
 */
#define GETSHORT(s, cp) { \
	(s) = *(cp)++ << 8; \
	(s) |= *(cp)++; \
}

#define GETLONG(l, cp) { \
	(l) = *(cp)++ << 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; \
}


#define PUTSHORT(s, cp) { \
	*(cp)++ = (s) >> 8; \
	*(cp)++ = (s); \
}

/*
 * Warning: PUTLONG destroys its first argument.
 */
#define PUTLONG(l, cp) { \
	(cp)[3] = l; \
	(cp)[2] = (l >>= 8); \
	(cp)[1] = (l >>= 8); \
	(cp)[0] = l >> 8; \
	(cp) += sizeof(unsigned long); \
}

#endif /* !_ARPA_NAMESER_H_ */
