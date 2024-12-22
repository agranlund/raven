/**
 * $Header: e:/lc/examples/bell/tsr\bell.c,v 1.7 1993/11/09 14:38:32 AGK Exp $
 *
 * TSR portion of HiSoft Bell Sample CPX
 *
 * $Author: AGK $
 * $Revision: 1.7 $
 *
 * (c) Copyright 1991, 1992, 1993 HiSoft
**/

#include <dos.h>
#include <string.h>
#include <cookie.h>
#include "tsr.h"

#define SAMPLEVERSION	"\r\n\033p       Sample Allocator v1.07       \033q\r\n"
#define COPYRIGHT		"      Copyright ½ 1991, HiSoft.        \r\n"

void main(BASEPAGE *bp);

#define LEAVE	16384;

__saveds __stdargs void
start(BASEPAGE *bp)
{
	long shrinklen = (long)bp->p_hitpa - (long)bp->p_lowtpa - LEAVE;

	cputs(SAMPLEVERSION COPYRIGHT);
	if (shrinklen > bp->p_tlen + bp->p_dlen + bp->p_blen + 0x100) {
		putreg(REG_A7, (long)bp->p_lowtpa + shrinklen);
		Mshrink(bp->p_lowtpa, shrinklen);
		main(bp);
	}
	cputs("Not installed\r\n");
	Pterm(1);
}

#define	sndbase	((volatile short *)0xffff8900)

#define	sndbasehi	((volatile short *)0xffff8902)
#define	sndbasemid	((volatile short *)0xffff8904)
#define	sndbaselo	((volatile short *)0xffff8906)

#define	sndendhi	((volatile short *)0xffff890e)
#define	sndendmid	((volatile short *)0xffff8910)
#define	sndendlo	((volatile short *)0xffff8912)

#define	sndmode	((volatile short *)0xffff8920)

#define	bell_hook	((void (**)(void))0x5ac)

extern struct config __far config;	/* forward references needed */

void
snd_wait(void)
{
	while ((*sndbase) & 0xff) ;
}

void
snd_kill(void)
{
	*sndbase = 0;
}

void
snd_play(void *buf, long length, short rate)
{
	if (buf) {
		snd_kill();
		*sndbasehi = (short)((unsigned long)buf >> 16);
		*sndbasemid = (short)((unsigned long)buf >> 8);
		*sndbaselo = (short)buf;

		buf = (short *)((unsigned long)buf + length);
		*sndendhi = (short)((unsigned long)buf >> 16);
		*sndendmid = (short)((unsigned long)buf >> 8);
		*sndendlo = (short)buf;

		*sndmode = rate;
		*sndbase = 1;			/* start single play mode */
	}
	else
		config.old_bell();
}

struct config __far config = {
	NULL,
	"#:\\SOUNDS\\*.AVR",
	"",
	0,
	0,
	"#:\\AUTO\\BELL.PRG",
	snd_play,
	snd_wait,
	0,
	snd_kill,
};

void
bell(void)
{
	if (config.snd_cur[0])
		snd_play((short *)(&config + 1), config.snd_length, config.snd_rate);
	else
		snd_play(NULL, 0, 0);
}

long
change_bell(void)
{
	config.old_bell = *bell_hook;
	*bell_hook = bell;
	return 0;
}

long
get_bootdev(void)
{
	return (long)*(short *)0x446;
}

void
main(BASEPAGE * bp)
{
	static struct FILEINFO info;
	long size = 0;
	long value;

	if (getcookie(_SND, &value) &&	/* must have an _SND cookie */
	  (value & 2) &&			/* DMA sound must be available */
	  !getcookie(HSBL_COOKIE, NULL)) {	/* and not already installed */
		Fsetdta(&info);
		if (config.snd_path[0] == '#') {
			/* never saved before */
			config.snd_path[0] = Supexec(get_bootdev) + 'A';
		}
		if (config.snd_tsr[0] == '#') {
			/* never saved before */
			config.snd_tsr[0] = config.snd_path[0];
		}
		if (!Fsfirst(config.snd_path, 0)) {
			do
				if (info.size > size)
					size = info.size;
			while (!Fsnext()) ;
		}
		if (size < getreg(REG_A7) - (long)(&config + 1)) {
			/* size of sample buffer */
			config.snd_maxlen = size;
			if (putcookie(HSBL_COOKIE, (long)&config)) {
				Supexec(change_bell);
				Ptermres(size + (long)(&config + 1) - (long)bp->p_lowtpa, 0);
			}
		}
	}
}
