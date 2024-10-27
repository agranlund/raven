/*
 *	Definitons for the ioctl() commands on sockets.
 *
 *	09/25/93, kay roemer.
 */

#ifndef _SOCKIOS_H
#define _SOCKIOS_H

/* socket-level I/O control calls */
#define SIOCGLOWAT	(('S' << 8) | 1)
#define SIOCSLOWAT	(('S' << 8) | 2)
#define SIOCGHIWAT	(('S' << 8) | 3)
#define SIOCSHIWAT	(('S' << 8) | 4)
#define SIOCSPGRP	(('S' << 8) | 5)
#define SIOCGPGRP	(('S' << 8) | 6)
#define SIOCATMARK	(('S' << 8) | 7)

/* socket configuration controls */
#define SIOCGIFNAME	(('S' << 8) | 10)	/* get iface name */
#define SIOCSIFLINK	(('S' << 8) | 11)	/* connect iface to device */
#define SIOCGIFCONF	(('S' << 8) | 12)	/* get iface list */
#define SIOCGIFFLAGS	(('S' << 8) | 13)	/* get flags */
#define SIOCSIFFLAGS	(('S' << 8) | 14)	/* set flags */
#define SIOCGIFADDR	(('S' << 8) | 15)	/* get PA address */
#define SIOCSIFADDR	(('S' << 8) | 16)	/* set PA address */
#define SIOCGIFDSTADDR	(('S' << 8) | 17)	/* get remote PA address */
#define SIOCSIFDSTADDR	(('S' << 8) | 18)	/* set remote PA address */
#define SIOCGIFBRDADDR	(('S' << 8) | 19)	/* get broadcast PA address */
#define SIOCSIFBRDADDR	(('S' << 8) | 20)	/* set broadcast PA address */
#define SIOCGIFNETMASK	(('S' << 8) | 21)	/* get network PA mask */
#define SIOCSIFNETMASK	(('S' << 8) | 22)	/* set network PA mask */
#define SIOCGIFMETRIC	(('S' << 8) | 23)	/* get metric */
#define SIOCSIFMETRIC	(('S' << 8) | 24)	/* set metric */
#define SIOCSLNKFLAGS	(('S' << 8) | 25)	/* set link level flags */
#define SIOCGLNKFLAGS	(('S' << 8) | 26)	/* get link level flags */
#define SIOCGIFMTU	(('S' << 8) | 27)	/* get MTU size */
#define SIOCSIFMTU	(('S' << 8) | 28)	/* set MTU size */
#define SIOCGIFSTATS	(('S' << 8) | 29)	/* get interface statistics */
#define SIOCSIFHWADDR	(('S' << 8) | 49)	/* set hardware address */
#define SIOCGIFHWADDR	(('S' << 8) | 50)	/* get hardware address */
#define SIOCGLNKSTATS	(('S' << 8) | 51)	/* get link statistics */
#define SIOCSIFOPT	(('S' << 8) | 52)	/* set interface option */

/* routing table calls */
#define SIOCADDRT	(('S' << 8) | 30)	/* add routing table entry */
#define SIOCDELRT	(('S' << 8) | 31)	/* delete routing table entry */

/* ARP cache control calls */
#define SIOCDARP	(('S' << 8) | 40)	/* delete ARP table entry */
#define SIOCGARP	(('S' << 8) | 41)	/* get ARP table entry */
#define SIOCSARP	(('S' << 8) | 42)	/* set ARP table entry */

#endif /* _SOCKIOS_H */
