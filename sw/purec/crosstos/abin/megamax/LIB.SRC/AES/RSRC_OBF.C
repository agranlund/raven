
#include <portab.h>
#include <gembind.h>



	WORD
rsrc_obfix(tree, obj)
	LONG		tree;
	WORD		obj;
{
	RS_TREE = tree;
	RS_OBJ = obj;
	return( crys_if(RSRC_OBFIX) );
}


