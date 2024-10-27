/*
 * Adopted to MintNet 1994, Kay Roemer.
 */

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 *	@(#)tcp.h	7.7 (Berkeley) 6/28/90
 */

#ifndef _NETINET_TCP_H
#define _NETINET_TCP_H

#ifndef	_FEATURES_H
# include <features.h>
#endif

#ifndef _SYS_TYPES_H
# include <sys/types.h>
#endif

/*
 * User-settable options (used with setsockopt).
 */
#define	TCP_NODELAY		 1  /* Don't delay send to coalesce packets  */
#define	TCP_MAXSEG		 2  /* Set maximum segment size  */
#define TCP_CORK		 3  /* Control sending of partial frames  */
#define TCP_KEEPIDLE		 4  /* Start keeplives after this period */
#define TCP_KEEPINTVL		 5  /* Interval between keepalives */
#define TCP_KEEPCNT		 6  /* Number of keepalives before death */
#define TCP_SYNCNT		 7  /* Number of SYN retransmits */
#define TCP_LINGER2		 8  /* Life time of orphaned FIN-WAIT-2 state */
#define TCP_DEFER_ACCEPT	 9  /* Wake up listener only when data arrive */
#define TCP_WINDOW_CLAMP	 10 /* Bound advertised window */
#define TCP_INFO		 11 /* Information about this connection. */
#define	TCP_QUICKACK		 12 /* Bock/reenable quick ACKs.  */
#define TCP_CONGESTION		 13 /* Congestion control algorithm.  */
#define TCP_MD5SIG		 14 /* TCP MD5 Signature (RFC2385) */
#define TCP_COOKIE_TRANSACTIONS	 15 /* TCP Cookie Transactions */
#define TCP_THIN_LINEAR_TIMEOUTS 16 /* Use linear timeouts for thin streams*/
#define TCP_THIN_DUPACK		 17 /* Fast retrans. after 1 dupack */
#define TCP_USER_TIMEOUT	 18 /* How long for loss retry before timeout */
#define TCP_REPAIR		 19 /* TCP sock is under repair right now */
#define TCP_REPAIR_QUEUE	 20 /* Set TCP queue to repair */
#define TCP_QUEUE_SEQ		 21 /* Set sequence number of repaired queue. */
#define TCP_REPAIR_OPTIONS	 22 /* Repair TCP connection options */
#define TCP_FASTOPEN		 23 /* Enable FastOpen on listeners */
#define TCP_TIMESTAMP		 24 /* TCP time stamp */
#define TCP_NOTSENT_LOWAT	 25 /* Limit number of unsent bytes in
				       write queue.  */
#define TCP_CC_INFO		 26 /* Get Congestion Control
				       (optional) info.  */
#define TCP_SAVE_SYN		 27 /* Record SYN headers for new
				       connections.  */
#define TCP_SAVED_SYN		 28 /* Get SYN headers recorded for
				       connection.  */
#define TCP_REPAIR_WINDOW	 29 /* Get/set window parameters.  */
#define TCP_FASTOPEN_CONNECT	 30 /* Attempt FastOpen with connect.  */
#define TCP_ULP			 31 /* Attach a ULP to a TCP connection.  */
#define TCP_MD5SIG_EXT		 32 /* TCP MD5 Signature with extensions.  */

__BEGIN_DECLS

#ifdef __USE_MISC
# include <sys/types.h>
# include <sys/socket.h>
# include <stdint.h>

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

typedef	uint32_t tcp_seq;
/*
 * TCP header.
 * Per RFC 793, September, 1981.
 */
struct tcphdr {
	u_short	th_sport;		/* source port */
	u_short	th_dport;		/* destination port */
	tcp_seq	th_seq;			/* sequence number */
	tcp_seq	th_ack;			/* acknowledgement number */
# if __BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
	__bitfield_type	th_flags:8;
	__bitfield_type th_x2:4;	/* (unused) */
	__bitfield_type th_off:4;	/* data offset */
# endif
# if __BYTE_ORDER == __ORDER_BIG_ENDIAN__
	__bitfield_type	th_off:4;		/* data offset */
	__bitfield_type	th_x2:4;		/* (unused) */
	__bitfield_type	th_flags:8;
# endif
#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
	uint16_t	th_win;			/* window */
	uint16_t	th_sum;			/* checksum */
	uint16_t	th_urp;			/* urgent pointer */
};

enum
{
  TCP_ESTABLISHED = 1,
  TCP_SYN_SENT,
  TCP_SYN_RECV,
  TCP_FIN_WAIT1,
  TCP_FIN_WAIT2,
  TCP_TIME_WAIT,
  TCP_CLOSE,
  TCP_CLOSE_WAIT,
  TCP_LAST_ACK,
  TCP_LISTEN,
  TCP_CLOSING   /* now a valid state */
};

# define	TCPOPT_EOL	0
# define	TCPOPT_NOP	1
# define	TCPOPT_MAXSEG	2
# define TCPOLEN_MAXSEG		4
# define TCPOPT_WINDOW		3
# define TCPOLEN_WINDOW		3
# define TCPOPT_SACK_PERMITTED	4		/* Experimental */
# define TCPOLEN_SACK_PERMITTED	2
# define TCPOPT_SACK		5		/* Experimental */
# define TCPOPT_TIMESTAMP	8
# define TCPOLEN_TIMESTAMP	10
# define TCPOLEN_TSTAMP_APPA	(TCPOLEN_TIMESTAMP+2) /* appendix A */

# define TCPOPT_TSTAMP_HDR	\
    (TCPOPT_NOP<<24|TCPOPT_NOP<<16|TCPOPT_TIMESTAMP<<8|TCPOLEN_TIMESTAMP)

/*
 * Default maximum segment size for TCP.
 * With an IP MSS of 576, this is 536,
 * but 512 is probably more convenient.
 * This should be defined as MIN(512, IP_MSS - sizeof (struct tcpiphdr)).
 */
#define	TCP_MSS	512

#define	TCP_MAXWIN	65535		/* largest value for window */

# define TCP_MAX_WINSHIFT	14	/* maximum window shift */

# define SOL_TCP		6	/* TCP level */

/*
 * TCP states.
 */
#define TCPS_CLOSED		0
#define TCPS_LISTEN		1
#define TCPS_SYNSENT		2
#define TCPS_SYNRCVD		3
#define TCPS_ESTABLISHED	4
#define TCPS_FINWAIT1		5
#define TCPS_FINWAIT2		6
#define TCPS_CLOSEWAIT		7
#define TCPS_LASTACK		8
#define TCPS_CLOSING		9
#define TCPS_TIMEWAIT		10

# define TCPI_OPT_TIMESTAMPS	1
# define TCPI_OPT_SACK		2
# define TCPI_OPT_WSCALE	4
# define TCPI_OPT_ECN		8  /* ECN was negociated at TCP session init */
# define TCPI_OPT_ECN_SEEN	16 /* we received at least one packet with ECT */
# define TCPI_OPT_SYN_DATA	32 /* SYN-ACK acked data in SYN sent or rcvd */

/* Values for tcpi_state.  */
enum tcp_ca_state
{
  TCP_CA_Open = 0,
  TCP_CA_Disorder = 1,
  TCP_CA_CWR = 2,
  TCP_CA_Recovery = 3,
  TCP_CA_Loss = 4
};

struct tcp_info
{
  uint8_t	tcpi_state;
  uint8_t	tcpi_ca_state;
  uint8_t	tcpi_retransmits;
  uint8_t	tcpi_probes;
  uint8_t	tcpi_backoff;
  uint8_t	tcpi_options;
  __bitfield_type tcpi_snd_wscale : 4;
  __bitfield_type tcpi_rcv_wscale : 4;

  uint32_t	tcpi_rto;
  uint32_t	tcpi_ato;
  uint32_t	tcpi_snd_mss;
  uint32_t	tcpi_rcv_mss;

  uint32_t	tcpi_unacked;
  uint32_t	tcpi_sacked;
  uint32_t	tcpi_lost;
  uint32_t	tcpi_retrans;
  uint32_t	tcpi_fackets;

  /* Times. */
  uint32_t	tcpi_last_data_sent;
  uint32_t	tcpi_last_ack_sent;	/* Not remembered, sorry.  */
  uint32_t	tcpi_last_data_recv;
  uint32_t	tcpi_last_ack_recv;

  /* Metrics. */
  uint32_t	tcpi_pmtu;
  uint32_t	tcpi_rcv_ssthresh;
  uint32_t	tcpi_rtt;
  uint32_t	tcpi_rttvar;
  uint32_t	tcpi_snd_ssthresh;
  uint32_t	tcpi_snd_cwnd;
  uint32_t	tcpi_advmss;
  uint32_t	tcpi_reordering;

  uint32_t	tcpi_rcv_rtt;
  uint32_t	tcpi_rcv_space;

  uint32_t	tcpi_total_retrans;
};


/* For TCP_MD5SIG socket option.  */
#define TCP_MD5SIG_MAXKEYLEN	80

/* tcp_md5sig extension flags for TCP_MD5SIG_EXT.  */
#define TCP_MD5SIG_FLAG_PREFIX	1 /* Address prefix length.  */

struct tcp_md5sig
{
  struct sockaddr_storage tcpm_addr;		/* Address associated.  */
  uint8_t	tcpm_flags;			/* Extension flags.  */
  uint8_t	tcpm_prefixlen;			/* Address prefix.  */
  uint16_t	tcpm_keylen;			/* Key length.  */
  uint32_t	__tcpm_pad;			/* Zero.  */
  uint8_t	tcpm_key[TCP_MD5SIG_MAXKEYLEN];	/* Key (binary).  */
};

/* For socket repair options.  */
struct tcp_repair_opt
{
  uint32_t	opt_code;
  uint32_t	opt_val;
};

/* Queue to repair, for TCP_REPAIR_QUEUE.  */
enum
{
  TCP_NO_QUEUE,
  TCP_RECV_QUEUE,
  TCP_SEND_QUEUE,
  TCP_QUEUES_NR,
};

/* For cookie transactions socket options.  */
#define TCP_COOKIE_MIN		8		/*  64-bits */
#define TCP_COOKIE_MAX		16		/* 128-bits */
#define TCP_COOKIE_PAIR_SIZE	(2*TCP_COOKIE_MAX)

/* Flags for both getsockopt and setsockopt */
#define TCP_COOKIE_IN_ALWAYS	(1 << 0)	/* Discard SYN without cookie */
#define TCP_COOKIE_OUT_NEVER	(1 << 1)	/* Prohibit outgoing cookies,
						 * supercedes everything. */

/* Flags for getsockopt */
#define TCP_S_DATA_IN		(1 << 2)	/* Was data received? */
#define TCP_S_DATA_OUT		(1 << 3)	/* Was data sent? */

#define TCP_MSS_DEFAULT		 536U	/* IPv4 (RFC1122, RFC2581) */
#define TCP_MSS_DESIRED		1220U	/* IPv6 (tunneled), EDNS0 (RFC3226) */

struct tcp_cookie_transactions
{
  uint16_t	tcpct_flags;
  uint8_t	__tcpct_pad1;
  uint8_t	tcpct_cookie_desired;
  uint16_t	tcpct_s_data_desired;
  uint16_t	tcpct_used;
  uint8_t	tcpct_value[TCP_MSS_DEFAULT];
};

/* For use with TCP_REPAIR_WINDOW.  */
struct tcp_repair_window
{
  uint32_t snd_wl1;
  uint32_t snd_wnd;
  uint32_t max_window;
  uint32_t rcv_wnd;
  uint32_t rcv_wup;
};

#endif /* Misc.  */

__END_DECLS

#endif /* _NETINET_TCP_H */
