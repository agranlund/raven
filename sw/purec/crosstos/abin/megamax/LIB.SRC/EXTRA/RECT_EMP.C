

#include <obdefs.h>

int
rect_empty(therect)
	GRECT therect;
{
	return (!(therect.g_w && therect.g_h ));
}
