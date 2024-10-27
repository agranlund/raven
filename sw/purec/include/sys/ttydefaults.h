/*-
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ttydefaults.h	8.4 (Berkeley) 1/21/94
 */

/*
 * System wide defaults for terminal state.  MiNT version.
 */
#ifndef _SYS_TTYDEFAULTS_H_
#define	_SYS_TTYDEFAULTS_H_

#ifndef _TERMIOS_H
# include <termios.h>
#endif

/*
 * Defaults on "first" open.
 */
#define	TTYDEF_IFLAG	(IGNPAR | ICRNL | IXON)
#define TTYDEF_OFLAG	(OPOST)
#define TTYDEF_LFLAG	(ECHO | ECHOE | ECHOK | ECHOCTL | ICANON | ISIG | IEXTEN)
#define TTYDEF_CFLAG	(CREAD | CLOCAL)
#define TTYDEF_SPEED	(B9600)

/* PROTECTED INCLUSION ENDS HERE */
#endif /* !_SYS_TTYDEFAULTS_H_ */

/*
 * #define TTYDEFCHARS to include an array of default control characters.
 */
#ifdef TTYDEFCHARS
cc_t	ttydefchars[NCCS] = {
	CEOF,	CEOL,	CEOL,	CERASE, CWERASE, CKILL, CREPRINT,
	_POSIX_VDISABLE, CINTR,	CQUIT,	CSUSP,	CDSUSP,	CSTART,	CSTOP,	CLNEXT,
#ifdef _POSIX_VDISABLE
	CDISCARD, CMIN,	CTIME,  CSTATUS, _POSIX_VDISABLE
#else
	CDISCARD, CMIN,	CTIME,  CSTATUS, '\0'
#endif
};
#undef TTYDEFCHARS
#endif
