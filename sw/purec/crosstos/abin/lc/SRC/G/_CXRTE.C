/**
 * $Header: e:/src/gem/aes/util\_cxrte.c,v 1.2 1993/04/12 16:43:20 AGK Exp $
 *
 * Run time error handler (GEM AES version)
 *
 * (c) Copyright 1993 HiSoft
**/

#include <aes.h>
#include <mintbind.h>
#include <string.h>

void __regargs
(_CXRTE)(const char *s)
{
	extern int __mint;
	static char rte[45];
	char *p;

	memcpy(&rte[0], "[3][", strlen("[3]["));
	p = &rte[strlen("[3][")];
	while (*p++ = *s++)
		;
	memcpy(&p[-1], "][ Abort ]", strlen("][ Abort ]"));

	if (_AESglobal[0] >= 0x400 && __mint >= 98)
		Salert(rte);
	else {
		appl_init();
		form_alert(1, rte);
		appl_exit();
	}
	if (__mint)
		Pkill(Pgetpid(), 6);
	Pterm(-39);
}
