
#include <portab.h>
#include <gembind.h>



	WORD
objc_order(tree, mov_obj, newpos)
	LONG		tree;
	WORD		mov_obj, newpos;
{
	OB_TREE = tree;
	OB_OBJ = mov_obj;
	OB_NEWPOS = newpos;
	return( crys_if( OBJC_ORDER ) );
}
