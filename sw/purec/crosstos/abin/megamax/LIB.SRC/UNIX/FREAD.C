/* LINTLIBRARY */
#include <stdio.h>

/*
#define	OLDVERSION
*/

#ifdef	OLDVERSION
int fread(ptr, size, nitems, stream)
register char *ptr;
int size, nitems;
FILE *stream;
{
    int c;
    register int cnt = nitems*size;
    register int left = cnt;

    while (left > 0) {
	if ((c = getc(stream)) == EOF)
	    return (cnt-left)/size;
	*ptr++ = c;
	left--;
    }
    return nitems > 0 ? nitems : 0;
}

#else

static getitem(p, sz, s)
register char *p;
int sz;
register FILE *s;
{
	register int rv, rc;


	if (sz >= (rv = s->_cnt)) {
		if (rv) {
			bcopy(s->_ptr, p, rv);
			p += rv;
			s->_ptr += rv;
			sz -= rv;
			s->_cnt -= rv;
		}

		if (_loadbuf(s)) {
			return EOF;
		}
	}

	if (sz > (rv = s->_cnt)) {
		if (rv) {
			bcopy(s->_ptr, p, rv);
			s->_ptr += rv;
			p += rv;
			sz -= rv;
			s->_cnt = 0;
		}

		for (rv = sz; rv && (rc = read(s->_fd, p, rv)) > 0; p += rc, rv -= rc);
		if (rc <= 0) {
			s->_flag |= (rc < 0) ? _ERR : _EOF;
			return EOF;
		}
	}
	else {
		bcopy(s->_ptr, p, sz);
		s->_ptr += sz;
		s->_cnt -= sz;
	}

	return 0;
}

int fread(ptr, size, nitems, fp)
register char *ptr;
int size, nitems;
register FILE *fp;
{
	register int ic;
 
 	if (fp->_flag & _RDWR && !(fp->_flag & _WRITE))
 		fp->_flag |= _READ;

	if (!(fp->_flag | _READ))
		return 0;
	
	if (nitems <= 0)
		return 0;

	if (!fp->_base) {
		_getbuf(fp);
		fp->_ptr = fp->_base;
		fp->_cnt = 0;
	}

	for (ic = 0; --nitems >= 0 && !getitem(ptr, size, fp); ic++, ptr += size);

	return ic;
}

#endif
