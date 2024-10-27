/*  if_ether.h -- MiNTLib.
    Copyright (C) 2002 Frank Naumann <fnaumann@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifndef _NETINET_IF_ETHER_H
#define _NETINET_IF_ETHER_H 1

#ifndef	_FEATURES_H
# include <features.h>
#endif

#ifndef _NET_IF_ETHER_H
# include <net/if_ether.h>
#endif

#ifndef _NET_IF_ARP_H
# include <net/if_arp.h>
#endif

__BEGIN_DECLS

/*
 * Ethernet Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  Structure below is adapted
 * to resolving internet addresses.  Field names used correspond to
 * RFC 826.
 */
struct	ether_arp {
	struct	arphdr ea_hdr;		/* fixed-size header */
	uint8_t arp_sha[ETH_ALEN];	/* sender hardware address */
	uint8_t arp_spa[4];			/* sender protocol address */
	uint8_t arp_tha[ETH_ALEN];	/* target hardware address */
	uint8_t arp_tpa[4];			/* target protocol address */
};
#define	arp_hrd	ea_hdr.ar_hrd
#define	arp_pro	ea_hdr.ar_pro
#define	arp_hln	ea_hdr.ar_hln
#define	arp_pln	ea_hdr.ar_pln
#define	arp_op	ea_hdr.ar_op

/*
 * Macro to map an IP multicast address to an Ethernet multicast address.
 * The high-order 25 bits of the Ethernet address are statically assigned,
 * and the low-order 23 bits are taken from the low end of the IP address.
 */
#define ETHER_MAP_IP_MULTICAST(ipaddr, enaddr) \
	/* struct in_addr *ipaddr; */ \
	/* uint8_t enaddr[ETH_ALEN]; */ \
{ \
	(enaddr)[0] = 0x01; \
	(enaddr)[1] = 0x00; \
	(enaddr)[2] = 0x5e; \
	(enaddr)[3] = ((uint8_t *)ipaddr)[1] & 0x7f; \
	(enaddr)[4] = ((uint8_t *)ipaddr)[2]; \
	(enaddr)[5] = ((uint8_t *)ipaddr)[3]; \
}

__END_DECLS

#endif
