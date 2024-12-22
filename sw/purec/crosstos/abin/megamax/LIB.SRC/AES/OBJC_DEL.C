
#include <portab.h>
#include <gembind.h>



	WORD
objc_delete(tree, delob)
	LONG		tree;
	WORD		delob;
{
	OB_TREE = tree;
	OB_DELOB = delob;
	return( crys_if( OBJC_DELETE ) );
}
