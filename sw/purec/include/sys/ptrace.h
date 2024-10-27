/*  sys/ptrace.h -- MiNTLib.
    Copyright (C) 2000 Frank Naumann <fnaumann@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

#ifndef	_SYS_PTRACE_H
# define _SYS_PTRACE_H	1

#ifndef _FEATURES_H
# include <features.h>
#endif

#ifndef _SYS_TYPES_H
# include <sys/types.h>
#endif

#include <mint/ptrace.h>

__BEGIN_DECLS

/* Type of the REQUEST argument to `ptrace.'  */
enum __ptrace_request
{
  /* Indicate that the process making this request should be traced.
     All signals received by this process can be intercepted by its
     parent, and its parent can use the other `ptrace' requests.  */
  PTRACE_TRACEME = PT_TRACE_ME,
#define PTRACE_TRACEME PTRACE_TRACEME

  /* Return the word in the process's text space at address ADDR.  */
  PTRACE_PEEKTEXT = PT_READ_I,
#define PTRACE_PEEKTEXT PTRACE_PEEKTEXT

  /* Return the word in the process's data space at address ADDR.  */
  PTRACE_PEEKDATA = PT_READ_D,
#define PTRACE_PEEKDATA PTRACE_PEEKDATA

  /* Write the word DATA into the process's text space at address ADDR.  */
  PTRACE_POKETEXT = PT_WRITE_I,
#define PTRACE_POKETEXT PTRACE_POKETEXT

  /* Write the word DATA into the process's data space at address ADDR.  */
  PTRACE_POKEDATA = PT_WRITE_D,
#define PTRACE_POKEDATA PTRACE_POKEDATA

  /* Continue the process.  */
  PTRACE_CONT = PT_CONTINUE,
#define PTRACE_CONT PTRACE_CONT

  /* Kill the process.  */
  PTRACE_KILL = PT_KILL,
#define PTRACE_KILL PTRACE_KILL

  /* Single step the process.
     This is not supported on all machines.  */
  PTRACE_SINGLESTEP = PT_STEP,
#define PTRACE_SINGLESTEP PTRACE_SINGLESTEP

  /* Get the process's registers (not including floating-point registers)
     and put them in the `struct reg' (see <arch/register.h>) at ADDR.  */
  PTRACE_GETREGS = PT_GETREGS,
#define PTRACE_GETREGS PTRACE_GETREGS

  /* Set the process's registers (not including floating-point registers)
     to the contents of the `struct reg' (see <arch/register.h>) at ADDR.  */
  PTRACE_SETREGS = PT_SETREGS,
#define PTRACE_SETREGS PTRACE_SETREGS

  /* Get the process's floating point registers and put them
     in the `struct fpreg' (see <arch/register.h>) at ADDR.  */
  PTRACE_GETFPREGS = PT_GETFPREGS,
#define PTRACE_GETFPREGS PTRACE_GETFPREGS

  /* Set the process's floating point registers to the contents
     of the `struct fpreg' (see <arch/register.h>) at ADDR.  */
  PTRACE_SETFPREGS = PT_SETFPREGS,
#define PTRACE_SETFPREGS PTRACE_SETFPREGS

  /* Attach to a process that is already running. */
  PTRACE_ATTACH = PT_ATTACH,
#define PTRACE_ATTACH PTRACE_ATTACH

  /* Detach from a process attached to with PTRACE_ATTACH.  */
  PTRACE_DETACH = PT_DETACH,
#define PTRACE_DETACH PTRACE_DETACH

  /* Continue and stop at the next (entry to) syscall.  */
  PTRACE_SYSCALL = PT_SYSCALL,
#define PTRACE_SYSCALL PTRACE_SYSCALL

  PTRACE_BASEPAGE = PT_BASEPAGE
};

extern long ptrace (int _request, pid_t _pid, void *_addr, __int32_t _data) __THROW;

__END_DECLS

#endif	/* ptrace.h  */
