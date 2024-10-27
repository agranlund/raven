/*  include/sys/syscall.h -- MiNTLib.
    Copyright (C) 1999 Guido Flohr <guido@freemint.de>

    This file is part of the MiNTLib project, and may only be used
    modified and distributed under the terms of the MiNTLib project
    license, COPYMINT.  By continuing to use, modify, or distribute
    this file you indicate that you have read the license and
    understand and accept it fully.
*/

/* Emulate the syscall() function that is used on real eUnuchs.  
   You shouldn't use these functions if you only write your software
   for native Ataris because the extra wrapper will only cause a
   performance loss.
   
   The syscall() function itself is declared in <unistd.h>.
   
   The general mechanism works like this:  Those functions that are
   actually OS calls under Unix will be defined in the library under
   the same name but with a double underscore prepended.  If your
   linker supports weak symbols then an alias is created (for example
   "open" would be an alias for "__open".  If your linker does not
   support weak symbols then the open function is a wrapper around
   a call to __open (which imposes a minor performance penalty).
   
   The idea behind this is to allow programs or other libraries (for
   example the zlibc) to override the definition for OS calls.  Again,
   with weak symbols this can be done without any overhead or performance
   penalties.  Only if your linker does not support weak symbols then
   in order to avoid link-time naming conflicts an extra function call
   is required.
   
   The result.  Whenever you formerly wrote a line of code like
   
   	read (desc, buf, sizeof buf);
   
   you now have three choices:
   
   	(1) read (desc, buf, sizeof buf);
   	(2) __read (desc, buf, sizeof buf);
   	(3) syscall (SYS_read, desc, buf, sizeof buf);
   
   You wall find the definitions for the non-underscored versions in
   the files "read_wr.c" (as opposed to "read.c").  Method (2) is
   the fastest, method (1) should normally be used and method (3) is
   highly deprecated: It is not only the slowest but will also drag
   a lot of lot of files into the link because the jump table references
   all modules that contain one of the calls listed in syscalls.list.
   Method (3) should only be used if you have no other choice or
   in some special cases.  For example the perl binary will anyway
   include most of the libc into the executable.  Then you can just
   as well profit from syscall().
   
   The main reason for all this hassle is to be able to compile the
   zlibc.  This may seem stupid for one single package but it has
   proven that the zlibc becomes more and more widely used and that
   the same mechanism is used in quite a few other projects as well.
   Reason enough to make it work with the MiNTLib.
   
   BTW, the opcodes SYS_* are pure fantasy.  However, you should not
   change them to preserve binary compatibility with future versions.  
   This list is not complete yet.  As soon as the portlib gets integrated
   in the MiNTLib more opcodes (like for chroot, etc.) will follow.  
   If you add opcodes be sure to update the macro SYS_MAX_OPCODE and
   to recompile syscall.c.  It's also strictly necessary to have the
   list be contiguous.  */
   
#ifndef _SYS_SYSCALL_H
# define _SYS_SYSCALL_H 1

#include <bits/syscall.h>
#include <asm/unistd.h>

#endif /* _SYS_SYSCALL_H */
