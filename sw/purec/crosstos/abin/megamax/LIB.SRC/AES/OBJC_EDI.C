
#include <portab.h>
#include <gembind.h>

WORD objc_edit(tree, obj, inchar, idx, kind, newidx)
	LONG		tree;
	WORD		obj;
	WORD		inchar, idx, kind;
	WORD		*newidx;
{
	OB_TREE = tree;
	OB_OBJ  = obj;
	OB_CHAR = inchar;
	OB_IDX  = idx;
	OB_KIND = kind;

	crys_if( OBJC_EDIT );

	*newidx = OB_ODX;

	return( RET_CODE );
}
