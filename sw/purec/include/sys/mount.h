#ifndef __SYS_MOUNT_H__
#define __SYS_MOUNT_H__

#include <sys/statfs.h>

#ifndef MOUNT_UFS
#define MOUNT_UFS 0
#define MOUNT_NFS 1
#define MOUNT_PC  2
#define MOUNT_MFS 3
#define MOUNT_LO  4
#define MOUNT_TFS 5
#define MOUNT_TMP 6
#define MOUNT_PROC 7
#define MOUNT_FD  8

#define MNT_NOWAIT 0

#endif

#endif /* __SYS_MOUNT_H__ */
