/*	string.h	4.1	83/05/26	*/
#ifndef DL_STRINGS
#define DL_STRINGS

/*
 * External function definitions
 * for routines described in string(3).
 */

extern char	*index();
extern char	*rindex();
extern char	*strcat();
extern char	*strcpy();
extern char	*strncat();
extern char	*strncpy();
extern char	*xtrcat();
extern char	*xtrcpy();
extern char	*xtrncpy();
extern int	strcmp();
extern int	strlen();
extern int	strncmp();

#endif /* DL_STRINGS */
