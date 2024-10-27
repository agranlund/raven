/*
 *	net/if.h for MintNet, (w) 1994, Kay Roemer.
 *
 *	Note that the structures defined here differ in some aspects
 *	from the `standard' BSD file with the same name, especially
 *	`struct ifnet'.
 *	I have tried to make the best match with the structures MintNet
 *	uses.
 */

#ifndef _NET_IF_H
#define _NET_IF_H 1

#ifndef	_FEATURES_H
# include <features.h>
#endif

#ifndef _SYS_TYPES_H
# include <sys/types.h>
#endif

#ifndef _SYS_SOCKET_H
# include <sys/socket.h>
#endif

__BEGIN_DECLS

/* Standard interface flags. */
enum
  {
    IFF_UP = 0x1,		/* Interface is up.  */
#define IFF_UP	IFF_UP
    IFF_BROADCAST = 0x2,	/* Broadcast address valid.  */
#define IFF_BROADCAST	IFF_BROADCAST
    IFF_DEBUG = 0x4,		/* Turn on debugging.  */
#define IFF_DEBUG	IFF_DEBUG
    IFF_LOOPBACK = 0x8,		/* Is a loopback net.  */
#define IFF_LOOPBACK	IFF_LOOPBACK
    IFF_POINTOPOINT = 0x10,	/* Interface is point-to-point link.  */
#define IFF_POINTOPOINT	IFF_POINTOPOINT
    IFF_NOTRAILERS = 0x20,	/* Avoid use of trailers.  */
#define IFF_NOTRAILERS	IFF_NOTRAILERS
    IFF_RUNNING = 0x40,		/* Resources allocated.  */
#define IFF_RUNNING	IFF_RUNNING
    IFF_NOARP = 0x80		/* No address resolution protocol.  */
#define IFF_NOARP	IFF_NOARP

#if 0
/* Not supported */
    IFF_PROMISC = 0x100,	/* Receive all packets.  */
#define IFF_PROMISC	IFF_PROMISC
    IFF_ALLMULTI = 0x200,	/* Receive all multicast packets.  */
#define IFF_ALLMULTI	IFF_ALLMULTI
    IFF_MASTER = 0x400,		/* Master of a load balancer.  */
#define IFF_MASTER	IFF_MASTER
    IFF_SLAVE = 0x800,		/* Slave of a load balancer.  */
#define IFF_SLAVE	IFF_SLAVE
    IFF_MULTICAST = 0x1000,	/* Supports multicast.  */
#define IFF_MULTICAST	IFF_MULTICAST
    IFF_PORTSEL = 0x2000,	/* Can set media type.  */
#define IFF_PORTSEL	IFF_PORTSEL
    IFF_AUTOMEDIA = 0x4000	/* Auto media select active.  */
#define IFF_AUTOMEDIA	IFF_AUTOMEDIA
#endif
  };

/* The ifaddr structure contains information about one address of an
   interface.  They are maintained by the different address families,
   are allocated and attached when an address is set, and are linked
   together so all addresses for an interface can be located.  */

struct ifaddr
  {
    struct sockaddr ifa_addr;	/* Address of interface.  */
    union
      {
	struct sockaddr	ifu_broadaddr;
	struct sockaddr	ifu_dstaddr;
      } ifa_ifu;
    struct ifnet *ifa_ifp;	/* Back-pointer to interface.  */
    struct ifaddr *ifa_next;	/* Next address for interface.  */
  };

#define	ifa_broadaddr	ifa_ifu.ifu_broadaddr	/* broadcast address	*/
#define	ifa_dstaddr	ifa_ifu.ifu_dstaddr	/* other end of link	*/


#define	IFNAMSIZ	16
#define IF_NAMESIZE	IFNAMSIZ

struct ifnet {
	char	if_name[IFNAMSIZ];	/* name, e.g. ``en'' or ``lo'' */
	short	if_unit;		/* sub-unit for lower level driver */
	short	if_flags;		/* up/down, broadcast, etc. */
	long	if_metric;		/* routing metric (external only) */
	long	if_mtu;			/* maximum transmission unit */
	long	if_timer;		/* time 'til if_watchdog called */
	short	if_type;		/* interface hardware type */
	struct	ifaddr *if_addrlist;	/* linked list of addresses per if */
	struct	ifqueue {
		short	ifq_len;
		short	ifq_maxlen;
		void	*ifq_head;
		void	*ifq_tail;
	} if_snd, if_rcv;
/* procedure handles */
	long	(*if_init)(void);	/* init routine */
	long	(*if_reset)(void);	/* bus reset routine */
	long	(*if_output)(void);	/* output routine (enqueue) */
	long	(*if_ioctl)(void);	/* ioctl routine */
	long	(*if_watchdog)(void);	/* timer routine */

	void	*if_data;		/* private data */
/* generic interface statistics */
	long	if_ipackets;		/* packets received on interface */
	long	if_ierrors;		/* input errors on interface */
	long	if_opackets;		/* packets sent on interface */
	long	if_oerrors;		/* output errors on interface */
	long	if_collisions;		/* collisions on csma interfaces */
/* end statistics */
	struct	ifnet *if_next;
};


struct	ifstat {
	unsigned long	in_packets;	/* # input packets */
	unsigned long	in_errors;	/* # input errors */
	unsigned long	out_packets;	/* # output packets */
	unsigned long	out_errors;	/* # output errors */
	unsigned long	collisions;	/* # collisions */
};

/*
 * Interface request structure used for socket
 * ioctl's.  All interface ioctl's must have parameter
 * definitions which begin with ifr_name.  The
 * remainder may be interface specific.
 */
struct	ifreq {
	char	ifr_name[IFNAMSIZ];		/* if name, e.g. "en0" */
	union {
		struct	sockaddr ifru_addr;
		struct	sockaddr ifru_hwaddr;
		struct	sockaddr ifru_dstaddr;
		struct	sockaddr ifru_broadaddr;
		struct	sockaddr ifru_netmask;
		short	ifru_flags;
		long	ifru_metric;
		long	ifru_ivalue;
		long	ifru_mtu;
		struct	ifstat ifru_stats;
		void	*ifru_data;
	} ifr_ifru;
#define	ifr_addr	ifr_ifru.ifru_addr	/* address */
#define	ifr_hwaddr	ifr_ifru.ifru_hwaddr	/* hardware address */
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-to-p link */
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address */
#define	ifr_netmask	ifr_ifru.ifru_netmask	/* netmask */
#define	ifr_flags	ifr_ifru.ifru_flags	/* flags */
#define	ifr_metric	ifr_ifru.ifru_metric	/* metric */
#define ifr_mtu		ifr_ifru.ifru_mtu	/* mtu */
#define ifr_stats	ifr_ifru.ifru_stats	/* statistics */
#define	ifr_data	ifr_ifru.ifru_data	/* for use by interface */
#define ifr_ifindex	ifr_ifru.ifru_ivalue    /* interface index      */
};

/*
 * Structure used in SIOCGIFCONF request.
 * Used to retrieve interface configuration
 * for machine (useful for programs which
 * must know all networks accessible).
 */
struct	ifconf {
	short	ifc_len;		/* size of associated buffer */
	union {
		void *ifcu_buf;
		struct	ifreq *ifcu_req;
	} ifc_ifcu;
#define	ifc_buf	ifc_ifcu.ifcu_buf	/* buffer address */
#define	ifc_req	ifc_ifcu.ifcu_req	/* array of structures returned */
};


/* structure used with SIOCSIFLINK */
struct iflink {
	char	ifname[16];	/* interface to link device to without unit
				 * number, eg 'sl'. On successful return
				 * the actual interface to which the device
				 * was linked, eg 'sl0', can be found here. */
	char	device[128];	/* device name, eg '/dev/ttya' */
};

/* structure used with SIOCSIFOPT */
struct ifopt {
	char 	option[32];	/* option name */
	short	valtype;	/* value type */
	short	vallen;		/* value length */
	union {			/* value */
		long v_long;
		char v_string[128];
	} ifou;
#define ifo_long	ifou.v_long
#define ifo_string	ifou.v_string
};

/* value types for ifopt.valtype */
#define IFO_INT		0	/* integer, uses v_long */
#define IFO_STRING	1	/* string, uses v_string */
#define IFO_HWADDR	2	/* hardware address, v_string[0..5] */

/*
 * sockaddr carrying a hardware address
 */
struct sockaddr_hw {
	unsigned short	shw_family;	/* AF_LINK */
	unsigned short	shw_type;	/* hardware type */
	unsigned short	shw_len;	/* address length */
	unsigned char	shw_addr[8];	/* address */
};

__END_DECLS

#endif /* _NET_IF_H */
