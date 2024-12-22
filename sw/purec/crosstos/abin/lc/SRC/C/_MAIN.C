/*
 * _main.c - start program running under Lattice C 5 ST
 *
 * Started 28/8/89 Alex G. Kiernan, based on Lattice source
 *
 * Unlike 3.04 this module no longer performs command line parsing and
 * I/O redirection, which is done by _setargv. This is to allow support
 * for the extended command line. The TINY macro has also disappeared,
 * to create a similar effect, declare your main as _main.
 *
 * $Log: _main.c,v $
 * Revision 1.2  1993/11/05  11:04:40  AGK
 * Added extra underscore
 *
 * Revision 1.1  1991/06/17  17:49:00  AGK
 * Initial revision
 *
 * Copyright (c) 1989 HiSoft and Lattice, Inc.
 */

#include <stdio.h>
#include <fcntl.h>
#include <ios1.h>
#include <stdlib.h>
#include <dos.h>
#include <osbind.h>

#define	prn 3
#define	aux 4

extern char **environ;

/* introduce prototype here to force regargs */
int main(int, char *[],char **);

int _main(int argc, char *argv[])
{
	int i;
	
	_ufbs[0].ufbfh = 0;
	_ufbs[0].ufbflg = UFB_RA;
	stdin->_file = 0;
	stdin->_flag = _IOREAD;
	if (_disatty(0))
		stdin->_flag |= _IOLBF;
		
	stdout->_file = 1;
	_ufbs[1].ufbfh = 1;
	if (_disatty(1))
	{
		stdout->_flag |= _IORW | _IONBF;
		_ufbs[1].ufbflg = UFB_RA | UFB_WA;
	}
	else
	{
		stdout->_flag = _IOWRT | _IOLBF;
		_ufbs[1].ufbflg = UFB_WA;
	}
		
	stderr->_file = 2;
	_ufbs[2].ufbfh = 2;
	if (_disatty(2))
	{
		stderr->_flag = _IORW | _IONBF;
		_ufbs[2].ufbflg = UFB_RA | UFB_WA;
	}
	else
	{
		stderr->_flag = _IOWRT | _IOLBF;
		_ufbs[2].ufbflg = UFB_WA;
	}

	for (i=0;i<=2;i++)
		if (!_disatty(i))
			_ufbs[i].ufbflg |= UFB_FH;

	Fforce(aux,-2);		/* create an aux: */
	
	_ufbs[3].ufbfh = aux;
	_ufbs[3].ufbflg = UFB_RA | UFB_WA;
	_ufbs[4].ufbfh = prn;
	_ufbs[4].ufbflg = UFB_WA;
	stdaux->_file = 3;
	stdaux->_flag = _IORW | _IOLBF;
	stdprt->_file = 4;
	stdprt->_flag = _IOWRT | _IOLBF;

	if(__fmode)
	{
		_ufbs[0].ufbflg |= O_RAW;
		_ufbs[1].ufbflg |= O_RAW;
		_ufbs[2].ufbflg |= O_RAW;
		_ufbs[3].ufbflg |= O_RAW;
		_ufbs[4].ufbflg |= O_RAW;
	}

	exit(main(argc,argv,environ));
	return 0;
}
