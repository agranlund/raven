#ifndef __SYS_STATVFS_H__
#define __SYS_STATVFS_H__

#ifndef	_FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

/*
 * Structure returned by statvfs(2).
 */

#define	FSTYPSZ	16

typedef struct statvfs {
	u_long	f_bsize;	/* fundamental file system block size */
	u_long	f_frsize;	/* fragment size */
	u_long	f_blocks;	/* total # of blocks of f_frsize on fs */
	u_long	f_bfree;	/* total # of free blocks of f_frsize */
	u_long	f_bavail;	/* # of free blocks avail to non-superuser */
	u_long	f_files;	/* total # of file nodes (inodes) */
	u_long	f_ffree;	/* total # of free file nodes */
	u_long	f_favail;	/* # of free nodes avail to non-superuser */
	u_long	f_fsid;		/* file system id (dev for now) */
	char	f_basetype[FSTYPSZ]; /* target fs type name, null-terminated */
	u_long	f_flag;		/* bit-mask of flags */
	u_long	f_namemax;	/* maximum file name length */
	char	f_fstr[32];	/* filesystem-specific string */
	u_long	f_filler[16];	/* reserved for future expansion */
} statvfs_t;

/*
 * Flag definitions.
 */

#define	ST_RDONLY	0x01	/* read-only file system */
#define	ST_NOSUID	0x02	/* does not support setuid/setgid semantics */
#define ST_NOTRUNC	0x04	/* does not truncate long file names */

int statvfs(const char *, struct statvfs *);
int fstatvfs(int, struct statvfs *);

__END_DECLS

#endif	/* __SYS_STATVFS_H__ */
