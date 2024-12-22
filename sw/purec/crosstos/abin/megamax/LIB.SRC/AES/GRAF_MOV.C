
#include <portab.h>
#include <gembind.h>

	VOID
graf_movebox(w, h, srcx, srcy, dstx, dsty)
	WORD		w, h;
	WORD		srcx, srcy, dstx, dsty;
{
	GR_I1 = w;
	GR_I2 = h;
	GR_I3 = srcx;
	GR_I4 = srcy;
	GR_I5 = dstx;
	GR_I6 = dsty;
	return( crys_if( GRAF_MBOX ) );
}
