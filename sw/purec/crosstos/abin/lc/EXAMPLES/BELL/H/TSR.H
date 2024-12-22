/*
 * tsr.h - interface between TSR and CPX
 *
 * Started 4/4/91 Alex G. Kiernan
 *
 * Copyright (c) 1991 HiSoft
 */
 
struct config
{
	void (*old_bell)(void);
	char snd_path[FMSIZE];
	char snd_cur[FNSIZE];
	long snd_length;
	short snd_rate;
	char snd_tsr[FMSIZE];
	void (*snd_play)(short *,long,short);
	void (*snd_wait)(void);
	long snd_maxlen;
	void (*snd_kill)(void);
};

#define HSBL_COOKIE 'HSBL'
