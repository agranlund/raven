#include <osbind.h>

#if 0
int isatty(fd)
int fd;
{
	int r;
	long f = 0;

	r = Fdatime(&f, fd, 0);
	return f == 0;
}
#endif

int isatty(fd)
int fd;
{
  long savepos;
  long seekret;
 
  savepos = Fseek(0L,fd,1); /* save where we are */
  seekret = Fseek(1L,fd,0); /* seek to 1 byte from beginning */
  Fseek(savepos,fd,0);      /* then back to where we were */
  return (seekret != 1L);   /* if the seek didn't work, it's a tty */
}
