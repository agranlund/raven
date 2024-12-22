
#include <portab.h>
#include <gembind.h>

static int last3_x, last3_y, last3_w, last3_h;		/* EP 4-9-87 */

	WORD
form_dial(dtype, ix, iy, iw, ih, x, y, w, h)
	WORD		dtype;
	WORD		ix, iy, iw, ih;
	WORD		x, y, w, h;
{
	FM_TYPE = dtype;
	FM_IX = ix;
	FM_IY = iy;
	FM_IW = iw;
	FM_IH = ih;
	FM_X = x;
	FM_Y = y;
	FM_W = w;
	FM_H = h;

	if (dtype == 3) {							/* EP 4-9-87 */
		last3_x = x;							/*     "     */
		last3_y = y;							/*     "     */
		last3_w = w;							/*     "     */
		last3_h = h;							/*     "     */
	}

	return( crys_if( FORM_DIAL ) );
}

/* EP 4-9-87 */
last_dial3(x,y,w,h)
int *x, *y, *w, *h;
{
	*x = last3_x;
	*y = last3_y;
	*w = last3_w;
	*h = last3_h;
}

