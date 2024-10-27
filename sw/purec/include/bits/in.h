/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* Modified for MiNTLib by Frank Naumann <fnaumann@freemint.de>.  */

#ifndef _NETINET_IN_H
# error "Never use <bits/in.h> directly; include <netinet/in.h> instead."
#endif

#if 0
/* not yet supported */
/* Link numbers.  */
#define	IMPLINK_IP		155
#define	IMPLINK_LOWEXPER	156
#define	IMPLINK_HIGHEXPER	158
#endif


/* Options for use with `getsockopt' and `setsockopt' at the IP level.
   The first word in the comment at the right is the data type used;
   "bool" means a boolean value stored in an `int'.  */
#define IP_OPTIONS                 1	/* ip_opts; IP per-packet options.  */
#define IP_HDRINCL                 2	/* int; Header is included with data.  */
#define IP_TOS                     3	/* int; IP type of service and precedence.  */
#define IP_TTL                     4	/* int; IP time to live.  */
#define IP_RECVOPTS                5	/* bool; Receive all IP options w/datagram.  */
#define IP_RECVRETOPTS             6	/* bool; Receive IP options for response.  */
#define IP_RECVDSTADDR             7	/* bool; Receive IP dst addr w/datagram.  */
#define IP_RETOPTS                 8	/* ip_opts; Set/get IP per-packet options.  */
#define IP_MULTICAST_IF            9	/* in_addr; set/get IP multicast i/f */
#define IP_MULTICAST_TTL          10	/* u_char; set/get IP multicast ttl */
#define IP_MULTICAST_LOOP         11	/* i_char; set/get IP multicast loopback */
#define IP_ADD_MEMBERSHIP         12	/* ip_mreq; add an IP group membership */
#define IP_DROP_MEMBERSHIP        13	/* ip_mreq; drop an IP group membership */
#if 0 /* not yet supported or defined */
#define IP_UNBLOCK_SOURCE         14	/* ip_mreq_source: unblock data from source */
#define IP_BLOCK_SOURCE           15	/* ip_mreq_source: block data from source */
#define IP_ADD_SOURCE_MEMBERSHIP  16	/* ip_mreq_source: join source group */
#define IP_DROP_SOURCE_MEMBERSHIP 17	/* ip_mreq_source: leave source group */
#define IP_MSFILTER               18
#ifdef __USE_MISC
# define MCAST_JOIN_GROUP         19	/* group_req: join any-source group */
# define MCAST_BLOCK_SOURCE       20	/* group_source_req: block from given group */
# define MCAST_UNBLOCK_SOURCE     21	/* group_source_req: unblock from given group*/
# define MCAST_LEAVE_GROUP        22	/* group_req: leave any-source group */
# define MCAST_JOIN_SOURCE_GROUP  23	/* group_source_req: join source-spec gr */
# define MCAST_LEAVE_SOURCE_GROUP 24	/* group_source_req: leave source-spec gr*/
# define MCAST_MSFILTER           25
# define IP_MULTICAST_ALL         26	/* bool */
# define IP_UNICAST_IF            27

# define MCAST_EXCLUDE   0
# define MCAST_INCLUDE   1
#endif
#endif

#define IP_ROUTER_ALERT	          28	/* bool */
#define IP_PKTINFO	              29	/* bool */
#define IP_PKTOPTIONS	          30
#define IP_PMTUDISC	              31	/* obsolete name? */
#define IP_MTU_DISCOVER	          32	/* int; see below */
#ifdef __linux__
#define IP_RECVERR	              33	/* bool */
#endif
#define IP_RECVTTL	              34	/* bool */
#define IP_RECVTOS	              35	/* bool */
#define IP_MTU		              35	/* int */
#define IP_FREEBIND	              36
#define IP_IPSEC_POLICY           37
#define IP_XFRM_POLICY	          38
#define IP_PASSSEC	              39
#define IP_TRANSPARENT	          40

/* TProxy original addresses */
#define IP_ORIGDSTADDR            41
#define IP_RECVORIGDSTADDR        IP_ORIGDSTADDR

#define IP_MINTTL                 42
#define IP_NODEFRAG               43
#define IP_CHECKSUM               44
#define IP_BIND_ADDRESS_NO_PORT   45
#define IP_RECVFRAGSIZE           46

/* IP_MTU_DISCOVER arguments.  */
#define IP_PMTUDISC_DONT   0	/* Never send DF frames.  */
#define IP_PMTUDISC_WANT   1	/* Use per route hints.  */
#define IP_PMTUDISC_DO     2	/* Always DF.  */
#define IP_PMTUDISC_PROBE  3	/* Ignore dst pmtu.  */
/* Always use interface mtu (ignores dst pmtu) but don't set DF flag.
   Also incoming ICMP frag_needed notifications will be ignored on
   this socket to prevent accepting spoofed ones.  */
#define IP_PMTUDISC_INTERFACE           4
/* Like IP_PMTUDISC_INTERFACE but allow packets to be fragmented.  */
#define IP_PMTUDISC_OMIT		5

/* To select the IP level.  */
#define SOL_IP	0

#define IP_DEFAULT_MULTICAST_TTL        1
#define IP_DEFAULT_MULTICAST_LOOP       1
#define IP_MAX_MEMBERSHIPS              20

#ifdef __USE_MISC
/* Structure used to describe IP options for IP_OPTIONS and IP_RETOPTS.
   The `ip_dst' field is used for the first-hop gateway when using a
   source route (this gets put into the header proper).  */
struct ip_opts
  {
    struct in_addr ip_dst;	/* First hop; zero without source route.  */
    char ip_opts[40];		/* Actually variable in size.  */
  };

/* Like `struct ip_mreq' but including interface specification by index.  */
struct ip_mreqn
  {
    struct in_addr imr_multiaddr;	/* IP multicast address of group */
    struct in_addr imr_address;		/* local IP address of interface */
    int	imr_ifindex;			/* Interface index */
  };

/* Structure used for IP_PKTINFO.  */
struct in_pktinfo
  {
    int ipi_ifindex;			/* Interface index  */
    struct in_addr ipi_spec_dst;	/* Routing destination address  */
    struct in_addr ipi_addr;		/* Header destination address  */
  };
#endif

/* not yet supported in MiNT */

/* Options for use with `getsockopt' and `setsockopt' at the IPv6 level.
   The first word in the comment at the right is the data type used;
   "bool" means a boolean value stored in an `int'.  */
#define IPV6_ADDRFORM		1
#define IPV6_2292PKTINFO	2
#define IPV6_RXINFO         IPV6_2292PKTINFO
#define IPV6_2292HOPOPTS	3
#define IPV6_RXHOPOPTS		IPV6_2292HOPOPTS
#define IPV6_2292DSTOPTS	4
#define IPV6_RXDSTOPTS		IPV6_2292DSTOPTS
#define IPV6_2292RTHDR		5
#define IPV6_RTHDR          IPV6_2292RTHDR
#define IPV6_2292PKTOPTIONS	6
#define IPV6_PKTOPTIONS		IPV6_2292PKTOPTIONS
#define IPV6_CHECKSUM		7
#define IPV6_2292HOPLIMIT	8
#define IPV6_HOPLIMIT		IPV6_2292HOPLIMIT

#define IPV6_TXINFO		IPV6_RXINFO
#define SCM_SRCINFO		IPV6_TXINFO
#define SCM_SRCRT		IPV6_RXSRCRT

#define IPV6_NEXTHOP		9
#define IPV6_AUTHHDR		10
#define IPV6_UNICAST_HOPS	16
#define IPV6_MULTICAST_IF	17
#define IPV6_MULTICAST_HOPS	18
#define IPV6_MULTICAST_LOOP	19
#define IPV6_JOIN_GROUP		20
#define IPV6_LEAVE_GROUP	21
#define IPV6_ROUTER_ALERT	22
#define IPV6_MTU_DISCOVER	23
#define IPV6_MTU		24
#define IPV6_RECVERR		25
#define IPV6_V6ONLY		26
#define IPV6_JOIN_ANYCAST	27
#define IPV6_LEAVE_ANYCAST	28
#define IPV6_IPSEC_POLICY	34
#define IPV6_XFRM_POLICY	35
#define IPV6_HDRINCL		36

/* Advanced API (RFC3542) (1).  */
#define IPV6_RECVPKTINFO	49
#define IPV6_PKTINFO		50
#define IPV6_RECVHOPLIMIT	51
/* #define IPV6_HOPLIMIT		52 */
#define IPV6_RECVHOPOPTS	53
#define IPV6_HOPOPTS		54
#define IPV6_RTHDRDSTOPTS	55
#define IPV6_RECVRTHDR		56
/* #define IPV6_RTHDR		57 */
#define IPV6_RECVDSTOPTS	58
#define IPV6_DSTOPTS		59
#define IPV6_RECVPATHMTU	60
#define IPV6_PATHMTU		61
#define IPV6_DONTFRAG		62

/* Advanced API (RFC3542) (2).  */
#define IPV6_RECVTCLASS		66
#define IPV6_TCLASS		67

#define IPV6_AUTOFLOWLABEL	70

/* RFC5014.  */
#define IPV6_ADDR_PREFERENCES	72

/* RFC5082.  */
#define IPV6_MINHOPCOUNT	73

#define IPV6_ORIGDSTADDR	74
#define IPV6_RECVORIGDSTADDR	IPV6_ORIGDSTADDR
#define IPV6_TRANSPARENT	75
#define IPV6_UNICAST_IF		76
#define IPV6_RECVFRAGSIZE	77

/* Obsolete synonyms for the above.  */
#if !__USE_KERNEL_IPV6_DEFS
# define IPV6_ADD_MEMBERSHIP	IPV6_JOIN_GROUP
# define IPV6_DROP_MEMBERSHIP	IPV6_LEAVE_GROUP
#endif

/* IPV6_MTU_DISCOVER values.  */
#define IPV6_PMTUDISC_DONT	0	/* Never send DF frames.  */
#define IPV6_PMTUDISC_WANT	1	/* Use per route hints.  */
#define IPV6_PMTUDISC_DO	2	/* Always DF.  */
#define IPV6_PMTUDISC_PROBE	3	/* Ignore dst pmtu.  */
#define IPV6_PMTUDISC_INTERFACE	4	/* See IP_PMTUDISC_INTERFACE.  */
#define IPV6_PMTUDISC_OMIT	5	/* See IP_PMTUDISC_OMIT.  */

/* Socket level values for IPv6.  */
#define SOL_IPV6        41
#define SOL_ICMPV6      58

/* Routing header options for IPv6.  */
#define IPV6_RTHDR_LOOSE	0	/* Hop doesn't need to be neighbour. */
#define IPV6_RTHDR_STRICT	1	/* Hop must be a neighbour.  */

#define IPV6_RTHDR_TYPE_0	0	/* IPv6 Routing header type 0.  */
