/* header file for POSIX directory access routines */

#ifndef _SYS_DIRENT_H
# define _SYS_DIRENT_H 1

#ifndef _FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

#include <bits/types.h>

#ifdef __USE_XOPEN
# ifndef __ino_t_defined
#  ifndef __USE_FILE_OFFSET64
typedef __ino_t ino_t;
#  else
typedef __ino64_t ino_t;
#  endif
#  define __ino_t_defined
# endif
# if defined __USE_LARGEFILE64 && !defined __ino64_t_defined
typedef __ino64_t ino64_t;
#  define __ino64_t_defined
# endif
#endif

/* This file defines `struct dirent'.

   It defines the macro `_DIRENT_HAVE_D_NAMLEN' iff there is a `d_namlen'
   member that gives the length of `d_name'.

   It defines the macro `_DIRENT_HAVE_D_RECLEN' iff there is a `d_reclen'
   member that gives the size of the entire directory entry.

   It defines the macro `_DIRENT_HAVE_D_OFF' iff there is a `d_off'
   member that gives the file offset of the next directory entry.

   It defines the macro `_DIRENT_HAVE_D_TYPE' iff there is a `d_type'
   member that gives the type of the file.
 */

#include <bits/dirent.h>

#if (defined __USE_BSD || defined __USE_MISC) && !defined d_fileno
# define d_ino	d_fileno		 /* Backward compatibility.  */
#endif

/* These macros extract size information from a `struct dirent *'.
   They may evaluate their argument multiple times, so it must not
   have side effects.  Each of these may involve a relatively costly
   call to `strlen' on some systems, so these values should be cached.

   _D_EXACT_NAMLEN (DP)	returns the length of DP->d_name, not including
   its terminating null character.

   _D_ALLOC_NAMLEN (DP)	returns a size at least (_D_EXACT_NAMLEN (DP) + 1);
   that is, the allocation size needed to hold the DP->d_name string.
   Use this macro when you don't need the exact length, just an upper bound.
   This macro is less likely to require calling `strlen' than _D_EXACT_NAMLEN.
   */

#ifdef _DIRENT_HAVE_D_NAMLEN
# define _D_EXACT_NAMLEN(d) ((d)->d_namlen)
# define _D_ALLOC_NAMLEN(d) (_D_EXACT_NAMLEN (d) + 1)
#else
# define _D_EXACT_NAMLEN(d) (strlen ((d)->d_name))
# ifdef _DIRENT_HAVE_D_RECLEN
#  define _D_ALLOC_NAMLEN(d) (((char *) (d) + (d)->d_reclen) - &(d)->d_name[0])
# else
#  define _D_ALLOC_NAMLEN(d) (sizeof (d)->d_name > 1 ? sizeof (d)->d_name : \
			      _D_EXACT_NAMLEN (d) + 1)
# endif
#endif


#ifdef __USE_BSD
/* File types for `d_type'.  */
enum
  {
    DT_UNKNOWN = 0,
# define DT_UNKNOWN	DT_UNKNOWN
    DT_SOCK = 1,
# define DT_SOCK	DT_SOCK
    DT_CHR = 2,
# define DT_CHR		DT_CHR
    DT_DIR = 4,
# define DT_DIR		DT_DIR
    DT_BLK = 6,
# define DT_BLK		DT_BLK
    DT_REG = 8,
# define DT_REG		DT_REG
    DT_FIFO = 10,
# define DT_FIFO	DT_FIFO
    DT_WHT = 12,
# define DT_WHT		DT_WHT
    DT_LNK = 14
# define DT_LNK		DT_LNK
  };

/* Convert between stat structure types and directory types.  */
# define IFTODT(mode)	(((mode) & 0170000) >> 12)
# define DTTOIF(dirtype)	((dirtype) << 12)
#endif


/* This is the data type of directory stream objects.
   The actual structure is opaque to users.  */
typedef struct __dirstream DIR;

/* Open a directory stream on NAME.
   Return a DIR stream on the directory, or NULL if it could not be opened.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern DIR *opendir (const char *__name) __nonnull ((1));

#ifdef __USE_XOPEN2K8
/* Same as opendir, but open the stream on the file descriptor FD.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern DIR *fdopendir (int __fd);
#endif

/* Close the directory stream DIRP.
   Return 0 if successful, -1 if not.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern int closedir (DIR *__dirp) __nonnull ((1));

/* Read a directory entry from DIRP.  Return a pointer to a `struct
   dirent' describing the entry, or NULL for EOF or error.  The
   storage returned may be overwritten by a later readdir call on the
   same DIR stream.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern struct dirent *readdir (DIR *__dirp) __nonnull ((1));

#if defined __USE_POSIX || defined __USE_MISC
/* Reentrant version of `readdir'.  Return in RESULT a pointer to the
   next entry.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern int readdir_r (DIR *__restrict __dirp,
		      struct dirent *__restrict __entry,
		      struct dirent **__restrict __result)
     __nonnull ((1, 2, 3));
#endif

/* Rewind DIRP to the beginning of the directory.  */
extern void rewinddir (DIR *__dirp) __THROW __nonnull ((1));

#if defined __USE_BSD || defined __USE_MISC || defined __USE_XOPEN
# include <bits/types.h>

/* Seek to position POS on DIRP.  */
extern void seekdir (DIR *__dirp, long int __pos) __THROW __nonnull ((1));

/* Return the current position of DIRP.  */
extern long int telldir (DIR *__dirp) __THROW __nonnull ((1));
#endif

#if defined __USE_BSD || defined __USE_MISC || defined __USE_XOPEN2K8

/* Return the file descriptor used by DIRP.  */
extern int dirfd (DIR *__dirp) __THROW __nonnull ((1));

/* Scan the directory DIR, calling SELECTOR on each directory entry.
   Entries for which SELECT returns nonzero are individually malloc'd,
   sorted using qsort with CMP, and collected in a malloc'd array in
   *NAMELIST.  Returns the number of entries selected, or -1 on error.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern int scandir (const char *__restrict __dir,
		    struct dirent ***__restrict __namelist,
		    int (*__selector) (const struct dirent *),
		    int (*__cmp) (const struct dirent **, const struct dirent **))
     __nonnull ((1, 2));

/* Function to compare two `struct dirent's alphabetically.  */
extern int alphasort (const struct dirent **__e1, const struct dirent **__e2) __THROW __attribute_pure__ __nonnull ((1, 2));
#endif /* Use BSD or misc or XPG7.  */

#if defined __USE_BSD || defined __USE_MISC
/* Read directory entries from FD into BUF, reading at most NBYTES.
   Reading starts at offset *BASEP, and *BASEP is updated with the new
   position after reading.  Returns the number of bytes read; zero when at
   end of directory; or -1 for errors.  */
extern __ssize_t getdirentries (int __fd, char *__restrict __buf,
				size_t __nbytes,
				__off_t *__restrict __basep)
     __THROW __nonnull ((2, 4));
#endif /* Use BSD or misc.  */

# ifdef __USE_GNU
/* Function to compare two `struct dirent's by name & version.  */
extern int versionsort (const struct dirent **__e1, const struct dirent **__e2) __THROW __attribute_pure__ __nonnull ((1, 2));
# endif

__END_DECLS

#endif /* _SYS_DIRENT_H */
