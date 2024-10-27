/*  sys/reboot.h -- MiNTLib.
    Copyright (C) 1999 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifndef	_SYS_REBOOT_H
# define _SYS_REBOOT_H 1

#ifndef _FEATURES_H
# include <features.h>
#endif

/* Perform a hard reset now.  A file system check is performed 
   (unless RB_NOSYNC is specified) and the processor is rebooted.  */
#define RB_AUTOBOOT	0x01234567L

/* Halt the system.  A sync of the filesystem is performed unless
   the RB_NOSYNC flag is set.  RB_HALT should be used with caution.  */
#define RB_HALT_SYSTEM 	0xcdef0123L
#define RB_HALT RB_HALT_SYSTEM  /* Compatibility with HP-UX?  */

/* Enable reboot using Ctrl-Alt-Delete keystroke.  Currently fake with
   MiNT.  What should actually happen is:  If reboot() is called with
   the argument RB_ENABLE_CAD the kernel will reboot without syncing
   filesystems as soon as CTRL-ALT-DEL is received (which is the 
   default behavior).  */
#define RB_ENABLE_CAD	0x89abcdefL

/* Disable reboot using Ctrl-Alt-Delete keystroke.  Currently fake with
   MiNT.  What should happen:  If you call reboot() with this argument,
   instead of rebooting the kernel will send a SIGINT to pid 0, usually
   the init(8) process.  */
#define RB_DISABLE_CAD	0x0

/* Stop system and switch power off if possible.  */
#define RB_POWER_OFF	0x4321fedc

/* Suspend system using software suspend.  */
#define RB_SW_SUSPEND	0xd000fce2

/* Reboot system into new kernel.  */
#define RB_KEXEC	0x45584543

__BEGIN_DECLS

/* Reboot the system or enable/disable CTRL-ALT-DEL keystroke.  */
extern int reboot (__mint_int_or_long __howto) __THROW;

__END_DECLS

#endif /* SYS_REBOOT_H */
