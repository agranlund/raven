/*
 *	netinet/udp.h for MintNet, (w) '94, Kay Roemer.
 */

#ifndef _NETINET_UDP_H
#define _NETINET_UDP_H

#ifndef	_FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

struct udphdr {
	unsigned short	uh_sport;	/* source port */
	unsigned short	uh_dport;	/* destination port */
	unsigned short	uh_ulen;	/* datagram length */
	unsigned short	uh_sum;		/* checksum */
};

/* UDP socket options */
#define UDP_CORK	1	/* Never send partially complete segments.  */
#define UDP_ENCAP	100	/* Set the socket to accept
				   encapsulated packets.  */
#define UDP_NO_CHECK6_TX 101	/* Disable sending checksum for UDP
				   over IPv6.  */
#define UDP_NO_CHECK6_RX 102	/* Disable accepting checksum for UDP
				   over IPv6.  */

/* UDP encapsulation types */
#define UDP_ENCAP_ESPINUDP_NON_IKE 1	/* draft-ietf-ipsec-nat-t-ike-00/01 */
#define UDP_ENCAP_ESPINUDP	2	/* draft-ietf-ipsec-udp-encaps-06 */
#define UDP_ENCAP_L2TPINUDP	3	/* rfc2661 */
#define UDP_ENCAP_GTP0		4	/* GSM TS 09.60 */
#define UDP_ENCAP_GTP1U		5	/* 3GPP TS 29.060 */

#define SOL_UDP            17      /* sockopt level for UDP */

__END_DECLS

#endif /* _NETINET_UDP_H */
