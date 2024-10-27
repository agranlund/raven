/*  sys/systeminfo.h -- MiNTLib.
    Copyright (C) 1999 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifndef	_SYS_SYSTEMINFO_H
#define	_SYS_SYSTEMINFO_H 1

#ifndef _FEATURES_H
# include <features.h>
#endif

__BEGIN_DECLS

/* Possible values for the first argument of `sysinfo'.  */
enum __sysinfo_command {
  /* Return name of the operating system (e.g. `FreeMiNT').  */
  SI_SYSNAME = 1,
#define SI_SYSNAME SI_SYSNAME
  /* Return the nodename of the present machine.  */
  SI_HOSTNAME = 2,
#define SI_HOSTNAME SI_HOSTNAME
  /* Set the current nodename to the contents of the provided buffer.
   * This command is reserved to the super-user (and is not implemented).
   */
  SI_SET_HOSTNAME = 3,
#define SI_SET_HOSTNAME SI_SET_HOSTNAME
  /* Return the current release of the operating system.  */
  SI_RELEASE = 4,
#define SI_RELEASE SI_RELEASE
  /* Return the version of the operating system.  */
  SI_VERSION = 5,
#define SI_VERSION SI_VERSION
  /* Return the basic instruction set architecture for the present machine,
   * for example `mc68000', `mc68030' etc.
   */
  SI_ARCHITECTURE = 6,
#define SI_ARCHITECTURE SI_ARCHITECTURE
  /* Return the variant instruction set architectures executable on the present
   * machine.
   */
  SI_ISALIST = 7,
#define SI_ISALIST SI_ISALIST
  /* Return the specific model of hardware platform.  */
  SI_PLATFORM = 8,
#define SI_PLATFORM SI_PLATFORM
  /* Return the name of the hardware manufacturer.  */
  SI_HW_PROVIDER = 9,
#define SI_HW_PROVIDER SI_HW_PROVIDER
  /* Return the domain name of the present machine.  */
  SI_DOMAINNAME = 10,
#define SI_DOMAINNAME SI_DOMAINNAME
  /* Set the current domainname name to the contents of the provided buffer.
   * This command is reserved to the super-user (and is not implemented).
   */
  SI_SET_DOMAINNAME = 11,
#define SI_SET_DOMAINNAME SI_SET_DOMAINNAME

  /* Non-standard opcodes for MiNT following.  Their use will result 
   * in non-portable code.
   */
  
  /* Return the kernel build-date in standard C preprocessor notation,
   * i. e. Mmm DD YYYY where `Mmm' is the 3-letter english abbrevation
   * of the month, `DD' is the (zero-padded) day of the month and
   * `YYYY' is the year including the century.
   *
   * This command is not available if the Ssystem () call is not
   * supported.
   */
  _MINT_SI_KERNEL_BUILD_DATE = 1002,
#define _MINT_SI_KERNEL_BUILD_DATE _MINT_SI_KERNEL_BUILD_DATE
    
  /* Return the kernel build-time in standard C preprocessor notation,
   * i. e. HH:MM:SS where `HH' is the two-digit hour, `MM' is the
   * two-digit minute and `SS' is the two-digit second.  All values
   * are padded with leading zeros.
   *
   * This command is not available if the Ssystem () call is not
   * supported.
   */
  _MINT_SI_KERNEL_BUILD_TIME = 1003,
#define _MINT_SI_KERNEL_BUILD_TIME _MINT_SI_KERNEL_BUILD_TIME

  /* Return the current mode of the kernel clock.  Possible 
   * values are `UTC' if the clock is ticking in UTC or
   * `local' if it is ticking in local time.
   */
  _MINT_SI_GET_CLOCK_MODE = 1004,
#define _MINT_SI_GET_CLOCK_MODE _MINT_SI_GET_CLOCK_MODE

  /* Set the kernel clock mode to `UTC' (or `GMT' which is equivalent)
   * or `local'.  Actually, every other string (even an empty string)
   * than `UTC' or `GMT' will set the clock to local mode.  
   *
   * You need super-user privileges to change the system clock mode.
   *
   * Warning: You shouldn't play around with that if there are
   * time-critical processes running since a change of the kernel
   * clock mode will warp the time by the difference between your
   * local timezone and UTC.
   */
  _MINT_SI_SET_CLOCK_MODE = 1005,
#define _MINT_SI_SET_CLOCK_MODE _MINT_SI_SET_CLOCK_MODE
};

/* Retrieve or set the system information as indicated by COMMAND.  For
 * read requests put the requested information into the buffer BUF
 * which has the size BUFSIZE.  The contents of BUF will always be
 * null-terminated.  When setting system information BUF should point
 * to a null-terminated string containing the information.  In any
 * case not more than BUFSIZE + 1 bytes will be copied.
 *
 * You may safely pass a NULL pointer as the BUF and 0L or a negative
 * number for BUFSIZE if you just want to find out the appropriate
 * buffer dimension for your particular request.  This behaviour
 * is probably non-standard and trusting on it will result in
 * non-portable code.
 *
 * The function returns the number of bytes (including the terminating
 * null character) copied for success or -1 for failure with ERRNO
 * describing the kind of error.  The following error conditions
 * are defined:
 *
 *   EFAULT - BUF does not point to a valid address and BUFSIZE is
 *            non-negative.
 *   EINVAL - The argument COMMAND is not within the range of valid
 *            commands as described in the enumeration above.
 *   ENOSYS - The operating system does not support retrieving or
 *            setting the requested type of information.
 *   EPERM  - An attempt was made to set information without 
 *            super-user privileges.
 * 
 * If the return value of the function equals the BUFSIZE argument
 * the entire string has been copied.  If it is greater than BUFSIZE
 * the string has been truncated to BUFSIZE - 1 bytes plus the
 * null-termination byte.
 *
 * If the interpretation of BUF is a pointer to a short value or
 * long value and the provided buffer size is not large enough
 * the required size is returned and nothing gets copied.
 */
extern int sysinfo (enum __sysinfo_command __command, char* __buf,
		    long __bufsize) __THROW;

__END_DECLS

#endif /* _SYS_SYSTEMINFO_H */
