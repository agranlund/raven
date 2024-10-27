/*
 *	net/route.h for MintNet, (w) 1994, Kay Roemer.
 *
 *	Some RTF_* flags are missing. I used the 4.3 BSD rtentry
 *	structure and added an rt_metric field.
 */

#ifndef _NET_ROUTE_H
#define _NET_ROUTE_H

#ifndef	_FEATURES_H
# include <features.h>
#endif

#ifndef _NET_IF_H
# include <net/if.h>
#endif

__BEGIN_DECLS

/* This structure gets passed by the SIOCADDRT and SIOCDELRT calls. */
struct rtentry {
	unsigned long	rt_hash;	/* hash key */
	struct sockaddr	rt_dst;		/* key */
	struct sockaddr	rt_gateway;	/* value */
	short		rt_flags;	/* up/down?, host/net */
#define RTF_UP		0x0001		/* Route usable.  */
#define RTF_GATEWAY	0x0002		/* Destination is a gateway.  */
#define RTF_HOST	0x0004		/* Host entry (net otherwise).  */
#define RTF_REJECT	0x0008		/* Reject route.  */
#define RTF_STATIC	0x0010		/* Manually injected route.  */
#define RTF_DYNAMIC	0x0020		/* Created dyn. (by redirect).  */
#define RTF_MODIFIED	0x0040	/* Modified dyn. (by redirect).  */
#define RTF_MASK	0x0080
#define RTF_LOCAL	0x0100		

	unsigned long	rt_metric;	/* distance metric */
	short		rt_refcnt;	/* # held references */
	unsigned long	rt_use;		/* raw # packets forwarded */
	struct ifnet	*rt_ifp;	/* interface to use */
};

__END_DECLS

#endif /* _NET_ROUTE_H */
