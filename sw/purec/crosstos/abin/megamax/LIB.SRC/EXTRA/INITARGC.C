/*
	initargcv() sets up the global variables _argc and _argcv which are
	passed to main() by _main() in init.c.  If main() has no parameters
	then the compiler puts out a dummy initargcv() procedure which over-
	rides this definition thereby saving code space.
*/

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <osbind.h>

#define MAXARGS		300		/* maximum nuber of command line arguments */

extern int _argc;
extern char **_argv;
extern char **environ;

_initargcv(comlin, envp)
char *comlin;	/* points to byte length encoded image of command line */
char *envp;
{
	register int i, j;
	int len;
	int inword;		/* 1=currently in a word */
	int quote;		/* 1=currently in a double quote, 2=single quote */
	register char *p, *q;
	char *new_stdin = 0, *new_stdout = 0;  /* redirection file names */
	int append;					/* 1=append to stdout */
	char *v[MAXARGS];			/* temporary argv[] array */
	char *ARGV = NULL;			/* ARGV string (extra comlin) */

	/* Set up environment variables */
	for (p=envp, i=0; p && *p; p += strlen(p)+1) 	/* i=num strings */
		if (!strncmp(p, "MMARGV=", 7))	/* must be last var. */
			ARGV = p+7;			/* not counted (i++ not done) */
		else
			i++;
	environ = (char **)Malloc( (long)(i+1) * sizeof(char *) );
	for (p=envp, j=0; i; p += strlen(p)+1, i--)	/* i=num strings */
		environ[j++] = p;
	environ[j] = NULL;

	_argc = 1;	/* always have at least program name */
	len = (*comlin & 255);	/* first byte is length of command line */
	bcopy(comlin+1, comlin, len); /* Ensure room for trailing \0 */
	comlin[len] = 0;			/* Null terminate now */

	for (q = p = comlin; p; ) {
		inword = quote = 0;
		for (i=0; i<len; p++, i++) {
			if (isspace(*p)) {
				if (!quote) {
					if (inword)
						inword = 0;
					*q++ = 0;	/* convert spaces to nulls */
				}
				else
					*q++ = ' ';
			}
			else if (*p == '"') {
				if (quote == 1)
					quote = 0;
				else if (!inword && !quote) {
					quote = 1;
					v[_argc++] = q;
				}
				else
					*q++ = '"';
			}
			else if (*p == '\'') {
				if (quote == 2)
					quote = 0;
				else if (!inword && !quote) {
					quote = 2;
					v[_argc++] = q;
				}
				else
					*q++ = '\'';
			}
			else {
				if (!inword && !quote) {
					char **std_ptr = NULL;

					inword = 1;
					if (*p == '<') 	/* redirect stdin */
						std_ptr = &new_stdin;
					else
						if (*p == '>')
							if (*(p+1) == '>') {
								std_ptr = &new_stdout; /* redirect and append */
								append = 1;
							}
							else {
								std_ptr = &new_stdout; /* redirect stdout */
								append = 0;
							}
						else
							v[_argc++] = q;	/* not redirection, add to argv[] */

					if (std_ptr) { /* Skip up to name of file */
						while (*(p+1)&&(*p == '<' || *p == '>' || isspace(*p)))
							p++, i++;
						*std_ptr = q;
					}
				}
				*q++ = *p;
			}
		}

		*q = 0;	/* make sure last string is null terminated */
		
		if (ARGV) {
			p = q = ARGV;
			len = strlen(ARGV);
			ARGV = NULL;
		}
		else
			p = NULL;
	}

	/* Open redirection files now */
	if (new_stdin) {
		if ( (i = open(new_stdin, O_RDONLY)) >= 0)
			Fforce(STDIN, i); 
	}
	if (!isatty(STDIN)) {
		stdin->_flag = _READ;	/* big buffering */
		stdin->_bufsize = _BUFSIZE;
		stdin->_fd = i;
	}

	if (new_stdout) {
		if ( (i = open(new_stdout, O_CREAT | O_WRONLY |
					   (append ? O_APPEND : O_TRUNC))) >= 0)
			Fforce(STDOUT, i);
	}
	if (!isatty(STDOUT))
		stdout->_flag = _WRITE | (append ? _APPEND : 0);

	_argv = (char **)Malloc((long)((_argc+1) * sizeof(char *)));
	for (i=1; i < _argc; i++)
		_argv[i] = v[i];		/* copy temporary argv[] to real argv[] */
	_argv[0] = q;				/* no program name */
	_argv[_argc] = NULL;		/* execv assumes null ptr after last argv[] */
}
