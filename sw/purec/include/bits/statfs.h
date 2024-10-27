
#ifndef _SYS_STATFS_H
# error "Never include <bits/statfs.h> directly; use <sys/statfs.h> instead."
#endif

#include <bits/types.h>  /* for __fsid_t */

struct statfs
{
  __fsword_t f_type;		/* type of info, zero for now */
  __fsword_t f_bsize;		/* fundamental file system block size */
  __fsblkcnt_t f_blocks;	/* total blocks in file system */
  __fsblkcnt_t f_bfree;		/* free blocks */
  __fsblkcnt_t f_bavail;	/* free blocks available to non-super-user */
  __fsfilcnt_t f_files;		/* total file nodes in file system */
  __fsfilcnt_t f_ffree;		/* free file nodes in fs */
  __fsid_t f_fsid;	/* file system id */
  __fsword_t f_namelen;
  __fsword_t f_frsize;
  __fsword_t f_flags;
  __fsword_t f_spare[4];	/* spare for later */

/* Tell code we have this member.  */
#define _STATFS_F_NAMELEN
#define _STATFS_F_FRSIZE
#define _STATFS_F_FLAGS
};

#ifdef __USE_LARGEFILE64
struct statfs64
  {
    __fsword_t f_type;
    __fsword_t f_bsize;
    __fsblkcnt64_t f_blocks;
    __fsblkcnt64_t f_bfree;
    __fsblkcnt64_t f_bavail;
    __fsfilcnt64_t f_files;
    __fsfilcnt64_t f_ffree;
    __fsid_t f_fsid;
    __fsword_t f_namelen;
    __fsword_t f_frsize;
    __fsword_t f_flags;
    __fsword_t f_spare[4];
  };
#endif

#ifdef __USE_MISC
int get_fsname(const char *path, char *xfs_name, char *type_name);
#endif
