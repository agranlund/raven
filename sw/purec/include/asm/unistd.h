/* This file is generated, DO NOT EDIT!!! All changes will get
   overwritten.
   Edit syscalls.list in the MiNTLib distribution instead.  */


#ifndef _ASM_MINT_UNISTD_H
#define _ASM_MINT_UNISTD_H

#ifndef __SYSCALL
#define __SYSCALL(a, b)
#endif

/*
 * This file contains the system call numbers.
 *
 * Note: holes are not allowed.
 */
#define __NR_write 0
__SYSCALL(__NR_write, sys_write)
#define __NR_fstat 1
__SYSCALL(__NR_fstat, sys_fstat)
#define __NR_times 2
__SYSCALL(__NR_times, sys_times)
#define __NR_pause 3
__SYSCALL(__NR_pause, sys_pause)
#define __NR_rename 4
__SYSCALL(__NR_rename, sys_rename)
#define __NR_truncate 5
__SYSCALL(__NR_truncate, sys_truncate)
#define __NR_waitpid 6
__SYSCALL(__NR_waitpid, sys_waitpid)
#define __NR_wait 7
__SYSCALL(__NR_wait, sys_wait)
#define __NR_wait3 8
__SYSCALL(__NR_wait3, sys_wait3)
#define __NR_wait4 9
__SYSCALL(__NR_wait4, sys_wait4)
#define __NR_readdir 10
__SYSCALL(__NR_readdir, sys_readdir)
#define __NR_fsync 11
__SYSCALL(__NR_fsync, sys_fsync)
#define __NR_lstat 12
__SYSCALL(__NR_lstat, sys_lstat)
#define __NR_dup2 13
__SYSCALL(__NR_dup2, sys_dup2)
#define __NR_getppid 14
__SYSCALL(__NR_getppid, sys_getppid)
#define __NR_close 15
__SYSCALL(__NR_close, sys_close)
#define __NR_setgid 16
__SYSCALL(__NR_setgid, sys_setgid)
#define __NR_statfs 17
__SYSCALL(__NR_statfs, sys_statfs)
#define __NR_sigaction 18
__SYSCALL(__NR_sigaction, sys_sigaction)
#define __NR_sigblock 19
__SYSCALL(__NR_sigblock, sys_sigblock)
#define __NR_bsd_signal 20
__SYSCALL(__NR_bsd_signal, sys_bsd_signal)
#define __NR_bsd_sigpause 21
__SYSCALL(__NR_bsd_sigpause, sys_bsd_sigpause)
#define __NR_sigpending 22
__SYSCALL(__NR_sigpending, sys_sigpending)
#define __NR_sigreturn 23
__SYSCALL(__NR_sigreturn, sys_sigreturn)
#define __NR_sigsetmask 24
__SYSCALL(__NR_sigsetmask, sys_sigsetmask)
#define __NR_fork 25
__SYSCALL(__NR_fork, sys_fork)
#define __NR_symlink 26
__SYSCALL(__NR_symlink, sys_symlink)
#define __NR_readlink 27
__SYSCALL(__NR_readlink, sys_readlink)
#define __NR_ioctl 28
__SYSCALL(__NR_ioctl, sys_ioctl)
#define __NR_stty 29
__SYSCALL(__NR_stty, sys_stty)
#define __NR_gtty 30
__SYSCALL(__NR_gtty, sys_gtty)
#define __NR_ftruncate 31
__SYSCALL(__NR_ftruncate, sys_ftruncate)
#define __NR_creat 32
__SYSCALL(__NR_creat, sys_creat)
#define __NR_fcntl 33
__SYSCALL(__NR_fcntl, sys_fcntl)
#define __NR_setsid 34
__SYSCALL(__NR_setsid, sys_setsid)
#define __NR_setuid 35
__SYSCALL(__NR_setuid, sys_setuid)
#define __NR_umask 36
__SYSCALL(__NR_umask, sys_umask)
#define __NR_kill 37
__SYSCALL(__NR_kill, sys_kill)
#define __NR_killpg 38
__SYSCALL(__NR_killpg, sys_killpg)
#define __NR_gettimeofday 39
__SYSCALL(__NR_gettimeofday, sys_gettimeofday)
#define __NR_settimeofday 40
__SYSCALL(__NR_settimeofday, sys_settimeofday)
#define __NR_getitimer 41
__SYSCALL(__NR_getitimer, sys_getitimer)
#define __NR_setitimer 42
__SYSCALL(__NR_setitimer, sys_setitimer)
#define __NR_getpriority 43
__SYSCALL(__NR_getpriority, sys_getpriority)
#define __NR_setpriority 44
__SYSCALL(__NR_setpriority, sys_setpriority)
#define __NR_nice 45
__SYSCALL(__NR_nice, sys_nice)
#define __NR_link 46
__SYSCALL(__NR_link, sys_link)
#define __NR_lseek 47
__SYSCALL(__NR_lseek, sys_lseek)
#define __NR_execve 48
__SYSCALL(__NR_execve, sys_execve)
#define __NR_getgid 49
__SYSCALL(__NR_getgid, sys_getgid)
#define __NR_rmdir 50
__SYSCALL(__NR_rmdir, sys_rmdir)
#define __NR_getegid 51
__SYSCALL(__NR_getegid, sys_getegid)
#define __NR_getuid 52
__SYSCALL(__NR_getuid, sys_getuid)
#define __NR_pipe 53
__SYSCALL(__NR_pipe, sys_pipe)
#define __NR_getpid 54
__SYSCALL(__NR_getpid, sys_getpid)
#define __NR_dup 55
__SYSCALL(__NR_dup, sys_dup)
#define __NR_setregid 56
__SYSCALL(__NR_setregid, sys_setregid)
#define __NR_alarm 57
__SYSCALL(__NR_alarm, sys_alarm)
#define __NR_geteuid 58
__SYSCALL(__NR_geteuid, sys_geteuid)
#define __NR_setreuid 59
__SYSCALL(__NR_setreuid, sys_setreuid)
#define __NR_chdir 60
__SYSCALL(__NR_chdir, sys_chdir)
#define __NR_access 61
__SYSCALL(__NR_access, sys_access)
#define __NR_getgroups 62
__SYSCALL(__NR_getgroups, sys_getgroups)
#define __NR_chown 63
__SYSCALL(__NR_chown, sys_chown)
#define __NR_select 64
__SYSCALL(__NR_select, sys_select)
#define __NR_getrusage 65
__SYSCALL(__NR_getrusage, sys_getrusage)
#define __NR_initgroups 66
__SYSCALL(__NR_initgroups, sys_initgroups)
#define __NR_setgroups 67
__SYSCALL(__NR_setgroups, sys_setgroups)
#define __NR_getrlimit 68
__SYSCALL(__NR_getrlimit, sys_getrlimit)
#define __NR_setrlimit 69
__SYSCALL(__NR_setrlimit, sys_setrlimit)
#define __NR_read 70
__SYSCALL(__NR_read, sys_read)
#define __NR_open 71
__SYSCALL(__NR_open, sys_open)
#define __NR_flock 72
__SYSCALL(__NR_flock, sys_flock)
#define __NR_utime 73
__SYSCALL(__NR_utime, sys_utime)
#define __NR_mkdir 74
__SYSCALL(__NR_mkdir, sys_mkdir)
#define __NR_stat 75
__SYSCALL(__NR_stat, sys_stat)
#define __NR_sync 76
__SYSCALL(__NR_sync, sys_sync)
#define __NR_chmod 77
__SYSCALL(__NR_chmod, sys_chmod)
#define __NR_unlink 78
__SYSCALL(__NR_unlink, sys_unlink)
#define __NR_chroot 79
__SYSCALL(__NR_chroot, sys_chroot)
#define __NR_fchmod 80
__SYSCALL(__NR_fchmod, sys_fchmod)
#define __NR_fchown 81
__SYSCALL(__NR_fchown, sys_fchown)
#define __NR_fpathconf 82
__SYSCALL(__NR_fpathconf, sys_fpathconf)
#define __NR_getdtablesize 83
__SYSCALL(__NR_getdtablesize, sys_getdtablesize)
#define __NR_gethostid 84
__SYSCALL(__NR_gethostid, sys_gethostid)
#define __NR_getpagesize 85
__SYSCALL(__NR_getpagesize, sys_getpagesize)
#define __NR_getpgrp 86
__SYSCALL(__NR_getpgrp, sys_getpgrp)
#define __NR_mkfifo 87
__SYSCALL(__NR_mkfifo, sys_mkfifo)
#define __NR_mknod 88
__SYSCALL(__NR_mknod, sys_mknod)
#define __NR_pathconf 89
__SYSCALL(__NR_pathconf, sys_pathconf)
#define __NR_poll 90
__SYSCALL(__NR_poll, sys_poll)
#define __NR_sbrk 91
__SYSCALL(__NR_sbrk, sys_sbrk)
#define __NR_setegid 92
__SYSCALL(__NR_setegid, sys_setegid)
#define __NR_seteuid 93
__SYSCALL(__NR_seteuid, sys_seteuid)
#define __NR_setpgid 94
__SYSCALL(__NR_setpgid, sys_setpgid)
#define __NR_setpgrp 95
__SYSCALL(__NR_setpgrp, sys_setpgrp)
#define __NR_sigprocmask 96
__SYSCALL(__NR_sigprocmask, sys_sigprocmask)
#define __NR_sigsuspend 97
__SYSCALL(__NR_sigsuspend, sys_sigsuspend)
#define __NR_sigvec 98
__SYSCALL(__NR_sigvec, sys_sigvec)
#define __NR_stime 99
__SYSCALL(__NR_stime, sys_stime)
#define __NR_sysinfo 100
__SYSCALL(__NR_sysinfo, sys_sysinfo)
#define __NR_time 101
__SYSCALL(__NR_time, sys_time)
#define __NR_uname 102
__SYSCALL(__NR_uname, sys_uname)
#define __NR_utimes 103
__SYSCALL(__NR_utimes, sys_utimes)
#define __NR_vfork 104
__SYSCALL(__NR_vfork, sys_vfork)
#define __NR_getdomainname 105
__SYSCALL(__NR_getdomainname, sys_getdomainname)
#define __NR_gethostname 106
__SYSCALL(__NR_gethostname, sys_gethostname)
#define __NR_setdomainname 107
__SYSCALL(__NR_setdomainname, sys_setdomainname)
#define __NR_sethostname 108
__SYSCALL(__NR_sethostname, sys_sethostname)
#define __NR_reboot 109
__SYSCALL(__NR_reboot, sys_reboot)
#define __NR_ptrace 110
__SYSCALL(__NR_ptrace, sys_ptrace)
#define __NR_sysctl 111
__SYSCALL(__NR_sysctl, sys_sysctl)
#define __NR_sysv_signal 112
__SYSCALL(__NR_sysv_signal, sys_sysv_signal)
#define __NR_xpg_sigpause 113
__SYSCALL(__NR_xpg_sigpause, sys_xpg_sigpause)
#define __NR_pipe2 114
__SYSCALL(__NR_pipe2, sys_pipe2)
#define __NR_fstatfs 115
__SYSCALL(__NR_fstatfs, sys_fstatfs)
#define __NR_exit 116
__SYSCALL(__NR_exit, sys_exit)
#define __NR_ulimit 117
__SYSCALL(__NR_ulimit, sys_ulimit)
#define __NR_profil 118
__SYSCALL(__NR_profil, sys_profil)
#define __NR_getpgid 119
__SYSCALL(__NR_getpgid, sys_getpgid)
#define __NR_fchdir 120
__SYSCALL(__NR_fchdir, sys_fchdir)
#define __NR_readv 121
__SYSCALL(__NR_readv, sys_readv)
#define __NR_writev 122
__SYSCALL(__NR_writev, sys_writev)
#define __NR_getsid 123
__SYSCALL(__NR_getsid, sys_getsid)
#define __NR_nanosleep 124
__SYSCALL(__NR_nanosleep, sys_nanosleep)

/*
 * The ifdef here should match the redirections in signal.h
 */
#ifdef __USE_BSD
#  define __NR_signal __NR_bsd_signal
#else
#  define __NR_signal __NR_sysv_signal
#endif
#ifdef __FAVOR_BSD
#  define __NR_sigpause __NR_bsd_sigpause
#else
#  define __NR_sigpause __NR_xpg_sigpause
#endif

#endif /* asm/unistd.h */
