/*
 * This file belongs to FreeMiNT.  It's not in the original MiNT 1.12
 * distribution.  See the file Changes.MH for a detailed log of changes.
 */

/*
 * include/mint/random.h
 *
 * Include file for the random number generator.
 */

#ifndef _MINT_RANDOM_H
#define _MINT_RANDOM_H 1

/* ioctl()'s for the random number generator.  They will hopefully not conflict. */

/* Get the entropy count. */
#define RNDGETENTCNT	(('R' << 8) | 0)

/* Add to (or subtract from) the entropy count.  (Superuser only.) */
#define RNDADDTOENTCNT	(('R' << 8) | 1)

/* Get the contents of the entropy pool.  (Superuser only.) */
#define RNDGETPOOL	(('R' << 8) | 2)

/* 
 * Write bytes into the entropy pool and add to the entropy count.
 * (Superuser only.)
 */
#define RNDADDENTROPY	(('R' << 8) | 3)

/* Clear entropy count to 0.  (Superuser only.) */
#define RNDZAPENTCNT	(('R' << 8) | 4)

/* Clear the entropy pool and associated counters.  (Superuser only.) */
#define RNDCLEARPOOL	(('R' << 8) | 5)

struct rand_pool_info {
	long	entropy_count;
	long	buf_size;
	ulong	buf[0];
};

/* Exported functions */

#ifdef __KERNEL__

#include "file.h"

extern void rand_initialize P_((void));
extern void rand_initialize_irq P_((int irq));
extern void rand_initialize_blkdev P_((int major));

extern void add_keyboard_randomness P_((unsigned short scancode));
extern void add_mouse_randomness P_((ulong mouse_data));
extern void add_interrupt_randomness P_((int irq));
extern void add_blkdev_randomness P_((int major));

extern void get_random_bytes P_((void *buf, unsigned long nbytes));

#ifdef DO_IT_LATER
/* These functions could be exported if need be.  */
extern ulong secure_tcp_sequence_number(ulong saddr, ulong daddr,
					ushort sport, ushort dport);
extern ulong secure_tcp_syn_cookie(ulong saddr, ulong daddr,
				   ushort sport, ushort dport,
				   ulong sseq, ulong count,
				   ulong data);
extern ulong check_tcp_syn_cookie(ulong cookie, ulong saddr,
				  ulong daddr, ushort sport,
				  ushort dport, ulong sseq,
				  ulong count, ulong maxdiff);
#endif

extern DEVDRV random_device, urandom_device;
extern void checkrandom P_((void));

#endif /* __KERNEL___ */

#endif /* _MINT_RANDOM_H */
