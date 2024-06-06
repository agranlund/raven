
#ifndef _SYS_STAT_H
#define _SYS_STAT_H 1

/* The Single Unix specification says that some more types are
   available here.  */
#ifndef _SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef __USE_XOPEN
# define __need_time_t
# include <time.h>		/* For time_t.  */
#endif

__BEGIN_DECLS

#include <bits/stat.h>

#if defined __USE_BSD || defined __USE_MISC || defined __USE_XOPEN
# define S_IFMT		__S_IFMT
# define S_IFDIR	__S_IFDIR
# define S_IFCHR	__S_IFCHR
# define S_IFBLK	__S_IFBLK
# define S_IFREG	__S_IFREG
# define S_IFIFO	__S_IFIFO
# define S_IFLNK	__S_IFLNK
# if (defined __USE_BSD || defined __USE_MISC || defined __USE_XOPEN2K)
#  define S_IFSOCK	__S_IFSOCK
# endif
#endif

/* Test macros for file types.	*/

#define	__S_ISTYPE(mode, mask)	(((mode) & __S_IFMT) == (mask))

#define	S_ISDIR(mode)	 __S_ISTYPE((mode), __S_IFDIR)
#define	S_ISCHR(mode)	 __S_ISTYPE((mode), __S_IFCHR)
#define	S_ISBLK(mode)	 __S_ISTYPE((mode), __S_IFBLK)
#define	S_ISREG(mode)	 __S_ISTYPE((mode), __S_IFREG)
#define S_ISFIFO(mode)	 __S_ISTYPE((mode), __S_IFIFO)
#define S_ISLNK(mode)	 __S_ISTYPE((mode), __S_IFLNK)
#define S_ISMEM(mode)	 __S_ISTYPE((mode), __S_IFMEM)

#ifdef	__USE_BSD
# define S_ISSOCK(mode) __S_ISTYPE((mode), __S_IFSOCK)
#endif

/* These are from POSIX.1b.  If the objects are not implemented using separate
   distinct file types, the macros always will evaluate to zero.  Unlike the
   other S_* macros the following three take a pointer to a `struct stat'
   object as the argument.  */
#ifdef	__USE_POSIX199309
# define S_TYPEISMQ(buf) __S_TYPEISMQ(buf)
# define S_TYPEISSEM(buf) __S_TYPEISSEM(buf)
# define S_TYPEISSHM(buf) __S_TYPEISSHM(buf)
#endif


/* Protection bits.  */

#define	S_ISUID __S_ISUID	/* Set user ID on execution.  */
#define	S_ISGID	__S_ISGID	/* Set group ID on execution.  */

#if defined __USE_BSD || defined __USE_MISC || defined __USE_XOPEN
/* Save swapped text after use (sticky bit).  This is pretty well obsolete.  */
# define S_ISVTX	__S_ISVTX
# define S_ISTXT	__S_ISVTX
#endif

#define	S_IRUSR	__S_IREAD	/* Read by owner.  */
#define	S_IWUSR	__S_IWRITE	/* Write by owner.  */
#define	S_IXUSR	__S_IEXEC	/* Execute by owner.  */
/* Read, write, and execute by owner.  */
#define	S_IRWXU	(__S_IREAD|__S_IWRITE|__S_IEXEC)

#if defined __USE_MISC && defined __USE_BSD
# define S_IREAD	S_IRUSR
# define S_IWRITE	S_IWUSR
# define S_IEXEC	S_IXUSR
#endif

#define	S_IRGRP	(S_IRUSR >> 3)	/* Read by group.  */
#define	S_IWGRP	(S_IWUSR >> 3)	/* Write by group.  */
#define	S_IXGRP	(S_IXUSR >> 3)	/* Execute by group.  */
/* Read, write, and execute by group.  */
#define	S_IRWXG	(S_IRWXU >> 3)

#define	S_IROTH	(S_IRGRP >> 3)	/* Read by others.  */
#define	S_IWOTH	(S_IWGRP >> 3)	/* Write by others.  */
#define	S_IXOTH	(S_IXGRP >> 3)	/* Execute by others.  */
/* Read, write, and execute by others.  */
#define	S_IRWXO	(S_IRWXG >> 3)


#ifdef	__USE_BSD
/* Macros for common mode bit masks.  */
#define	ACCESSPERMS (S_IRWXU|S_IRWXG|S_IRWXO) /* 0777 */
#define	ALLPERMS (S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO)/* 07777 */
#define	DEFFILEMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)/* 0666 */

#define S_BLKSIZE		512	/* Block size for `st_blocks'.  */
#endif


/* Get file attributes for FILE and put them in BUF.  */
extern int stat (__const char *__restrict __file,
		 struct stat *__restrict __buf) __THROW;
extern int __stat (__const char *__restrict __file,
		   struct stat *__restrict __buf) __THROW;

/* Get file attributes for the file, device, pipe, or socket
   that file descriptor FD is open on and put them in BUF.  */
extern int fstat (int __fd, struct stat *__buf) __THROW;
extern int __fstat (int __fd, struct stat *__buf) __THROW;

#if defined __USE_BSD || defined __USE_XOPEN_EXTENDED
/* Get file attributes about FILE and put them in BUF.
   If FILE is a symbolic link, do not follow it.  */
extern int lstat (__const char *__restrict __file,
		  struct stat *__restrict __buf) __THROW;
extern int __lstat (__const char *__restrict __file,
		  struct stat *__restrict __buf) __THROW;
#endif

/* Set file access permissions for FILE to MODE.
   This takes an `int' MODE argument because that
   is what `mode_t's get widened to.  */
extern int chmod (__const char *__file, __mode_t __mode) __THROW;
extern int __chmod (__const char *__file, __mode_t __mode) __THROW;

/* Set file access permissions of the file FD is open on to MODE.  */
#if defined __USE_BSD || defined __USE_XOPEN_EXTENDED
extern int fchmod (int __fd, __mode_t __mode) __THROW;
extern int __fchmod (int __fd, __mode_t __mode) __THROW;
#endif


/* Set the file creation mask of the current process to MASK,
   and return the old creation mask.  */
extern __mode_t umask (__mode_t __mask) __THROW;
extern __mode_t __umask (__mode_t __mask) __THROW;

#ifdef	__USE_GNU
/* Get the current `umask' value without changing it.
   This function is only available under the GNU Hurd.  */
extern __mode_t getumask (void) __THROW;
#endif

/* Create a new directory named PATH, with permission bits MODE.  */
extern int mkdir (__const char *__path, __mode_t __mode) __THROW;
extern int __mkdir (__const char *__path, __mode_t __mode) __THROW;

/* Create a device file named PATH, with permission and special bits MODE
   and device number DEV (which can be constructed from major and minor
   device numbers with the `makedev' macro above).  
   
   This is theory.  In fact this doesn't work yet with MiNT and this
   function is a dummy.  */
#if defined __USE_MISC || defined __USE_BSD || defined __USE_XOPEN_EXTENDED
extern int mknod (__const char *__path, __mode_t __mode, __dev_t __dev)
     __THROW;
extern int __mknod (__const char *__path, __mode_t __mode, __dev_t __dev)
     __THROW;
#endif


/* Create a new FIFO named PATH, with permission bits MODE.  */
extern int mkfifo (__const char *__path, __mode_t __mode) __THROW;
extern int __mkfifo (__const char *__path, __mode_t __mode) __THROW;


__END_DECLS

#endif /* _STAT_H */
