/*
 * _setargv.c - argument set up for Lattice C 5 programs run from desktop
 *
 * _setargv is called by the initial startup routine, when an ARGV
 * environment variable could not be located. It is up to this routine
 * to parse the command line, building the argv[] vector, and doing any
 * other environmental setup the user requires when a shell is not present.
 *
 * This module must be compiled without stack checking, this is because
 * _setargv is called prior to the stack base variable being
 * initialised, or memory being sized. Similarly it may not call any
 * subroutines which contravene these limitations. The subroutine itself
 * must use registerised parameter passing, otherwise the startup stub will
 * break.
 *
 * Started 20/8/89 Alex G. Kiernan
 *
 * 22/8/89 - Changed to register convention, fixed up exact types of parms
 * 28/8/89 - Added stderr, stdaux and stdprt support
 *
 * $Log: _setargv.c,v $
 * Revision 1.4  1992/01/16  12:57:02  AGK
 * Killed of C/V warning
 *
 * Revision 1.3  1991/10/13  12:39:18  AGK
 * Corrected warnings.
 *
 * Revision 1.2  1991/10/13  12:10:52  AGK
 * Added closure of redirection files. This seems to be necessary on later
 * versions of the OS. TOS 1.0 gives EINTRN however...
 *
 * Revision 1.1  1991/06/17  17:49:00  AGK
 * Initial revision
 *
 * Copyright (c) 1989 HiSoft
 */

#include <stddef.h>
#include <dos.h>

void __regargs __redirect(const char *infile,const char *outfile,short *old)
{
	/* perform input redirection */
	if (infile)
	{
		long fh;
		
		if ((fh=Fopen(infile,0))>=0)
		{
			if (old)
				old[0]=Fdup(0);
			Fforce(0,fh);
			Fclose(fh);
		}
	}

	/* perform output redirection */
	if (outfile)
	{
		long fh;
		int append;

		if (*outfile=='>')
		{
			append=1;
			if (outfile[1]=='&')	/* redirect stderr as well */
			{
				fh=Fopen(outfile+2,2);
				outfile=NULL;		/* flag to do stderr as well */
			}
			else
				fh=Fopen(outfile+1,1);
		}
		else
		{
			append=0;
			if (*outfile=='&')		/* redirect stderr as well */
			{
				fh=Fcreate(outfile+1,0);
				outfile=NULL;		/* flag to do stderr as well */
			}
			else
				fh=Fcreate(outfile,0);
		}
		
		if (fh>=0)
		{
			if (append)
				Fseek(0,fh,2);
			if (old)
				old[1]=Fdup(1);
			Fforce(1,fh);
			if (!outfile)		/* redirect stderr as well */
			{
				if (old)
					old[2]=Fdup(2);
				Fforce(2,fh);
			}
			Fclose(fh);
		}
	}
}

__regargs char **_setargv(char *line, char **argv)
{
	char *infile=NULL, *outfile=NULL;
	
	*argv++ = (char *)"";	/* ANSI requirement when program name unavailable */
	for (;;)
	{
		register char c;
		
		while (*line==' '||*line=='\t')
			line++;
		if (!*line)
			break;
		switch(c=*line)
		{
			case '<':
				infile=++line;
				c='\0';
				break;
				
			case '>':
				outfile=++line;
				c='\0';
				break;

			case '\'':
			case '"':
				*argv++=++line;
				break;

			default:
				*argv++=line;
				c='\0';
				break;
		}
		while (*line && (c?*line!=c:(*line!=' '&&*line!='\t')))
			line++;
		if (!*line)
			break;
		*line++='\0';
	}
	
	if (_disatty(2))
		Fforce(2,-1);	/* point stderr to console */

	__redirect(infile,outfile,NULL);

	*argv++=NULL;		/* UNIX requirement */
	return argv;		/* return first free byte after argv[] */
}
