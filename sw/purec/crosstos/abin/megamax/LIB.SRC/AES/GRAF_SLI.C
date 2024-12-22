
#include <portab.h>
#include <gembind.h>



	VOID
graf_slidebox(tree, parent, obj, isvert)
	LONG		tree;
	WORD		parent;
	WORD		obj;
	WORD		isvert;
{
	GR_TREE = tree;
	GR_PARENT = parent;
	GR_OBJ = obj;
	GR_ISVERT = isvert;
	return( crys_if( GRAF_SLIDEBOX ) );
}
