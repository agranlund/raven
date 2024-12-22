/**
 * $Id: sndbind.h,v 1.6 1993/11/14 14:43:52 AGK Rel $
 *
 * Interface to Falcon sound hardware
 *
 * (c) Copyright 1992 Atari Corp.
 * (c) Copyright 1992, 1993 HiSoft
**/

#ifndef _SNDBIND_H
#define _SNDBIND_H

#include <osbind.h>

/**
 * Setbuffer() reg parameter values
**/
#define PLAY	0
#define	RECORD	1

/**
 * Soundcmd() mode parameter values
**/
#define	INQUIRE		-1
#define	LTATTEN		0
#define	RTATTEN		1
#define	LTGAIN		2
#define	RTGAIN		3
#define	ADDERIN		4
#define	ADCINPUT	5
#define	SETPRESCALE	6

/**
 * Sound BIOS errors
**/
#define	SNDNOTLOCK	-128
#define	SNDLOCKED	-129

/**
 * ADDERIN bit masks
**/
#define ADDERIN_ADC	1
#define ADDERIN_MATRIX	2

/**
 * SETPRESCALE values
**/
#define STE_6K		0	/* Invalid... won't do 6.25khz samples */
#define STE_12K		1	/* 12.5KHz STE/TT compatible */
#define STE_25K		2	/* 25KHz STE/TT compatible */
#define STE_50K		3	/* 50KHz STE/TT compatible */

/**
 * Setmode() parameter values
**/
#define	STEREO8		0
#define	STEREO16	1
#define	MONO8		2

/**
 * Buffoper parameter values
**/
#define	PLAY_ENABLE	0x1
#define	PLAY_REPEAT	0x2
#define	RECORD_ENABLE	0x4
#define	RECORD_REPEAT	0x8

/**
 * Dsptristate() parameter values
**/
#define	ENABLE		1
#define	TRISTATE	0

/**
 * Devconnect() source device parameter values
**/
#define	DMAPLAY	0	/* DMA playback */
#define	DSPXMIT	1	/* DSP transmit */
#define	EXTINP	2	/* External input */
#define	ADC	3	/* Microphone/PSG, see Soundcmd(ADCINPUT) */

/**
 * Devconnect() destination device parameter values
**/
#define	DMAREC	1	/* DMA record */
#define	DSPRECV	2	/* DSP receive */
#define	EXTOUT	4	/* External output */
#define	DAC		8	/* Headphone, internal speaker, monitor */

/**
 * Devconnect() clock input parameter values
**/
#define	CLK25M		0	/* internal 25.175 MHz clock */
#define	CLKEXT		1	/* external clock */
#define	CLK32M		2	/* internal 32 MHz */

/**
 * Devconnect() clock prescale parameter values
**/
#define CLK_OLD		0	/* STE/TT prescale values */
#define	CLK50K		1	/* 49170Hz */
#define	CLK33K		2	/* 33880Hz */
#define	CLK25K		3	/* 24585Hz */
#define	CLK20K		4	/* 20770Hz */
#define	CLK16K		5	/* 16490Hz */
#define	CLK12K		7	/* 12292Hz */
#define	CLK10K		9	/* 9834Hz */
#define	CLK8K		11	/* 8195Hz */

#define	ACT_CLK50K	49170
#define	ACT_CLK33K	33880
#define	ACT_CLK25K	24585
#define	ACT_CLK20K	20770
#define	ACT_CLK16K	16490
#define	ACT_CLK12K	12292
#define	ACT_CLK10K	9834
#define	ACT_CLK8K	8195

/**
 * Devconnect() protocol parameter values
**/
#define NO_SHAKE	1
#define HANDSHAKE	0

typedef struct SndBufPtr {
	const char *play;
	char *record;
	long reserve1;
	long reserve2;
} SndBufPtr;
#endif
