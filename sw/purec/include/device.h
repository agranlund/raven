/* File to describe user definable devices. */

#ifndef _DEVICE_H
#define _DEVICE_H

#ifndef	_FEATURES_H
# include <features.h>
#endif

#ifndef _SYS_TYPES_H
# include <sys/types.h>
#endif

__BEGIN_DECLS

typedef long (*devfunc_t)();

struct _device {
	char	*dosnm,		/* name under DOS (e.g. "CON:") 	*/
		*unxnm;		/* name under UNIX (e.g. "console") 	*/
	dev_t 	dev;		/* device number   (e.g. 0xffff)	*/
	devfunc_t 	open, close, read, write, ioctl;
	struct _device	*next;
};

/*
 * Pseudo-device interface stuff. New devices can be hooked into the chain
 * that's rooted at __devices. All unx2dos and low level i/o functions use this
 * chain in determining whether files are devices or disk files, as does stat().
 * The major device number of user-supplied devices must *NOT* be 0 or 0xff,
 * both of which are reserved.
 */

#define mkdev(major,minor)  ((dev_t)((((major) << 8) & 0xff00)|((minor) & 0x00ff)))

/* major/minor defined in <types.h> */

extern struct _device *__devices;

extern void _install_device (struct _device *d);
extern struct _device *_dev_fd (int fd);
extern struct _device *_dev_dosname (const char *dosnm);
extern struct _device *_dev_unxname (const char *unm);

__END_DECLS

#endif /* _DEVICE_H */
