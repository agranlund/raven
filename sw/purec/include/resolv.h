/*
 * Adopted to Mint-Net 1994, Kay Roemer.
 */

/*
 * Copyright (c) 1983, 1987, 1989 The Regents of the University of California.
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
 */

#ifndef _RESOLV_H_
#define _RESOLV_H_

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/nameser.h>

/*
 * Resolver configuration file.
 * Normally not present, but may contain the address of the
 * inital name server(s) to query and the domain search list.
 */

#ifndef _PATH_RESCONF
#define _PATH_RESCONF	"/etc/resolv.conf"
#endif

/*
 * Global defines and variables for resolver stub.
 */
#define MAXNS			3	/* max # name servers we'll track */
#define MAXDFLSRCH		3	/* # default domain levels to try */
#define MAXDNSRCH		6	/* max # domains in search path */
#define LOCALDOMAINPARTS	2	/* min levels in name that is "local" */

#define RES_TIMEOUT		5	/* min. seconds between retries */
#define RES_MAXNDOTS		15	/* should reflect bit field size */
#define RES_MAXRETRANS		30	/* only for resolv.conf/RES_OPTIONS */
#define RES_MAXRETRY		5	/* only for resolv.conf/RES_OPTIONS */
#define RES_DFLRETRY		2	/* Default #/tries. */
#define RES_MAXTIME		65535	/* Infinity, in milliseconds. */

__BEGIN_DECLS

#ifndef __res_state_defined
# define __res_state_defined
struct state {
	int	retrans;	 	/* retransmition time interval */
	int	retry;			/* number of times to retransmit */
	unsigned long	options;		/* option flags - see below. */
	int	nscount;		/* number of name servers */
	struct	sockaddr_in nsaddr_list[MAXNS];	/* address of name server */
#define	nsaddr	nsaddr_list[0]		/* for backward compatibility */
	u_short	id;			/* current packet id */
	char	defdname[MAXDNAME];	/* default domain */
	char	*dnsrch[MAXDNSRCH+1];	/* components of domain to search */
};
#endif

/*
 * Revision information.  This is the release date in YYYYMMDD format.
 * It can change every day so the right thing to do with it is use it
 * in preprocessor commands such as "#if (__RES > 19931104)".  Do not
 * compare for equality; rather, use it to determine whether your resolver
 * is new enough to contain a certain feature.
 */

#define	__RES	19991006

struct res_sym {
	int	number;		/* Identifying number, like T_MX */
	char *	name;		/* Its symbolic name, like "MX" */
	char *	humanname;	/* Its fun name, like "mail exchanger" */
};

/*
 * Resolver options (keep these in synch with res_debug.c, please)
 */
#define RES_INIT	0x00000001UL	/* address initialized */
#define RES_DEBUG	0x00000002UL	/* print debug messages */
#define RES_AAONLY	0x00000004UL	/* authoritative answers only */
#define RES_USEVC	0x00000008UL	/* use virtual circuit */
#define RES_PRIMARY	0x00000010UL	/* query primary server only */
#define RES_IGNTC	0x00000020UL	/* ignore trucation errors */
#define RES_RECURSE	0x00000040UL	/* recursion desired */
#define RES_DEFNAMES	0x00000080UL	/* use default domain name */
#define RES_STAYOPEN	0x00000100UL	/* Keep TCP socket open */
#define RES_DNSRCH	0x00000200UL	/* search up local domain tree */
#define	RES_INSECURE1	0x00000400UL	/* type 1 security disabled */
#define	RES_INSECURE2	0x00000800UL	/* type 2 security disabled */
#define	RES_NOALIASES	0x00001000UL	/* shuts off HOSTALIASES feature */
#define	RES_USE_INET6	0x00002000UL	/* use/map IPv6 in gethostbyname() */
#define RES_ROTATE		0x00004000UL	/* rotate ns list after each query */
#define	RES_NOCHECKNAME	0x00008000UL	/* do not check names for sanity (!IMPL) */
#define	RES_KEEPTSIG	0x00010000UL	/* do not strip TSIG records */
#define	RES_BLAST		0x00020000UL	/* blast all recursive servers */
#define RES_USEBSTRING	0x00040000UL	/* IPv6 reverse lookup with byte
					   strings */
#define RES_NOIP6DOTINT	0x00080000UL	/* Do not use .ip6.int in IPv6
					   reverse lookup */
#define RES_USE_EDNS0	0x00100000UL	/* Use EDNS0.  */
#define RES_SNGLKUP	0x00200000	/* one outstanding request at a time */
#define RES_SNGLKUPREOP	0x00400000UL	/* -"-, but open new socket for each
					   request */
#define RES_USE_DNSSEC	0x00800000UL	/* use DNSSEC using OK bit in OPT */
#define RES_NOTLDQUERY	0x01000000UL	/* Do not look up unqualified name
					   as a TLD.  */

#define RES_DEFAULT	(RES_RECURSE | RES_DEFNAMES | RES_DNSRCH)

extern struct state _res;
extern int h_errno;

/* Private routines shared between libc/net, named, nslookup and others. */
#define dn_comp		__dn_comp
#define dn_expand	__dn_expand
#define dn_skipname	__dn_skipname
#define fp_nquery	__fp_nquery
#define fp_query	__fp_query
#define hostalias	__hostalias
#define p_query		__p_query
#define putlong		__putlong
#define putshort	__putshort
#define p_class		__p_class
#define p_time		__p_time
#define p_type		__p_type

int	dn_skipname	(const u_char *__comp_dn, const u_char *__eom);
void	fp_nquery (const u_char *, int, FILE *) __THROW;
void	fp_query (const u_char *, FILE *);
const char	*hostalias	(const char *);
const char	*p_class	(int);
const char	*p_time	(u_long);
const char	*p_type	(int);
void p_query(const unsigned char *msg);

extern void	putshort	(u_short, u_char *);
extern void	putlong	(u_long l, u_char *);

extern int	dn_comp		(const char *__exp_dn, u_char *__comp_dn,
				int __length, u_char **__dnptrs,
				u_char **__lastdnptr);

extern int	dn_expand	(const u_char *__msg, const u_char *__eomorig,
				const u_char *__comp_dn, char *__exp_dn,
				int __length);
				
extern int	res_init 	(void);

extern int	res_mkquery 	(int __opval, const char *__dname,
				int __class, int __type, char *__data,
				int __datalen, struct rrec *__newrr,
				unsigned char *__buf, int __buflen);
				
extern int	res_send 	(const unsigned char *__msg, int __msglen,
				unsigned char *__answer, int __anslen);

extern int	res_query	(const char *__dname, int __class,
				int __type, u_char *__answer, int __anslen);
				
extern int	res_search	(const char *__dname, int __class,
				int __type, u_char *__answer, int __anslen);

extern int	res_querydomain	(const char *__name, const char *__domain,
				int __class, int __type, u_char *__answer,
				int __anslen);

extern void	herror		(const char *__s);

__END_DECLS

#endif /* !_RESOLV_H_ */
