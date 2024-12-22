
#include <portab.h>
#include <gembind.h>


	VOID
graf_watchbox(tree, obj, instate, outstate)
	LONG		tree;
	WORD		obj;
	UWORD		instate, outstate;
{
	GR_TREE = tree;
	GR_OBJ = obj;
	GR_INSTATE = instate;
	GR_OUTSTATE = outstate;
	return( crys_if( GRAF_WATCHBOX ) );
}
