
#include <portab.h>
#include <gembind.h>


	WORD
objc_add(tree, parent, child)
	LONG		tree;
	WORD		parent, child;
{
	OB_TREE = tree;
	OB_PARENT = parent;
	OB_CHILD = child;
	return( crys_if( OBJC_ADD ) );
}
