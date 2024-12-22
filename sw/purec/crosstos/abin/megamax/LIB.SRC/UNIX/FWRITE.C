/* LINTLIBRARY */
#include	<stdio.h>


#ifdef NOVOID
#define void int
#endif

static putitem(p, sz, s)
register char *p;
int sz;
register FILE *s;
{
	register int rc, rv, cnt;
	long lseek();
	
	while (sz) {
		/* Fill up remainder of buffer */
		cnt = s->_bufsize - (s->_ptr - s->_base);
		rc = sz > cnt ? cnt : sz;
		if (rc) {
			sz -= rc;
			bcopy(p, s->_ptr, rc);
			p += rc;
			s->_ptr += rc;
			if (s->_cnt)
				s->_cnt -= rc;
			cnt -= rc;
			s->_flag |= _DIRTY;
		}
		if (cnt == 0)      /* EP 9-9-87: Moved out of above "if" */
			if (fflush(s))
				return EOF;
		
		cnt = s->_bufsize - (s->_ptr - s->_base);
		if (sz >= cnt) {	/* write out full intermediate blocks */
			int save_sz = sz;

			if (rc)
				rv = sz - (sz % cnt);
			else
				rv = sz;
			sz -= rv;
			for (; rv && (rc = write(s->_fd, p, rv)) > 0; rv -= rc, p += rc) ;
			if (rc == -1) {
				s->_flag |= _ERR;
				(void) lseek(s->_fd, (long) (save_sz - rv), 1);
				return EOF;
			}
		}
	}
	
	return 0;
}

int fwrite(ptr, size, nitems, s)
register char *ptr;
register int size, nitems;
register FILE *s;
{
	register int ic;
	register char *p;
 
 	if (s->_flag & _RDWR && !(s->_flag & _READ))
 		s->_flag |= _WRITE;
	
	if (!(s->_flag & _WRITE))
		return 0;
	
	if (!s->_base) {
		_getbuf(s);
		s->_ptr = s->_base;
		s->_cnt = s->_bufsize;
	}
	
	for (ic = 0; --nitems >= 0 && !putitem(ptr, size, s); ic++, ptr += size);
	
	if (!(s->_flag & _BIGBUF)) {
		if (s->_ptr != s->_base) {
			if (s->_flag & _UNBUF) {
				if (fflush(s)) {
					s->_flag |= _ERR;
				}
			}
			else {
				for (p = s->_base; p != s->_ptr && *p != '\n'; p++);
				if (p != s->_ptr && *p == '\n')
					if (fflush(s)) {
						s->_flag |= _ERR;
					}
			}
		}
		s->_cnt = 0;
	}
	
	return ic;
}
