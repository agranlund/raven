#ifndef __SYS_ULIMIT_H__
#define __SYS_ULIMIT_H__

/*
 * The following are codes which can be
 * passed to the ulimit system call. (Xenix compatible.)
 */

#define UL_GFILLIM	1	/* get file limit */
#define UL_SFILLIM	2	/* set file limit */
#define UL_GMEMLIM	3	/* get process size limit */
#define UL_GDESLIM	4	/* get file descriptor limit */
#define UL_GTXTOFF	64	/* get text offset */

/*
 * The following are symbolic constants required for
 * X/Open Conformance.   They are the equivalents of
 * the constants above.
 */

#define UL_GETFSIZE	UL_GFILLIM	/* get file limit */
#define UL_SETFSIZE	UL_SFILLIM	/* set file limit */

#define __UL_GETOPENMAX	UL_GDESLIM		/* Return the maximum number of files
					   that the calling process can open.*/

#endif        /* __SYS_ULIMIT_H__ */
