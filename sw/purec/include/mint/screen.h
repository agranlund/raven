
#ifndef _MINT_SCREEN_H
#define _MINT_SCREEN_H 1

#ifndef	_FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

/* Data structures for IOCTL on /dev/screen */

#define SCR_NUM_COLORS	256	/* max possible num. of colors (even more on Falcon) */

/* SCR_ALIGN gives a mask telling how the physical screen must be
   aligned (for the SCRSETP ioctl call. A typical use might be
      newscrn = malloc(size + SCR_ALIGN) & (~SCR_ALIGN)
   If SCR_ALIGN is -1, then the physical screen may not be remapped
*/
#define SCR_ALIGN	0x0ffL

/* define the following only if the physical memory occupied by the
   screen is not accessible to user programs without an operating
   system request to re-map or shar memory (i.e. don't define this
   unless you have a version of Minix with a hardware memory
   manager
*/
/* #define SCR_VIRT_MEM	 */	/* screen lives in virtual memory */


/* physical screen attributes; information returned by the
   SCRGETPARAM ioctl call */

struct scr_param {
	long	scr_base;	/* physical address of screen */
	short	scr_width;	/* width of screen (pixels) */
	short	scr_height;	/* height of screen (pixels) */
	short	scr_depth;	/* no. of bitplanes on screen */
	short	scr_mode;	/* special mode info about screen */
};

/* Each palette entry for the screen colors holds three entries;
   the red, green, and blue values for this color, from 0 (no color)
   to 255 (max. color).
   The actual number of different values recognized by the hardware
   is given by SCR_RGB_VALUES; for example, the atari ST recognizes
   only 8 possible values for each of red, green, and blue, for
   a total of 512 colors; for it, SCR_RGB_VALUES == 8.
*/

#define SCR_RGB_VALUES 16

/* constants for accessing the scr_rgb array */
#define SCR_RED 	0
#define SCR_BLUE 	1
#define SCR_GREEN	2

/* data structure returned by SCRGETCOLOR ioctl call */
struct scr_palette {
	unsigned char scr_rgb[SCR_NUM_COLORS][3];
};


#define SCRGETPARAM     (('S'<<8) | 1)
#define SCRSETPARAM	(('S'<<8) | 2)
#define SCRGETCOLOR	(('S'<<8) | 3)
#define SCRSETCOLOR	(('S'<<8) | 4)

__END_DECLS

#endif /* _MINT_SCREEN_H */
