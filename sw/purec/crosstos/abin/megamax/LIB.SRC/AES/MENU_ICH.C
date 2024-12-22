
#include <portab.h>
#include <gembind.h>


	WORD
menu_icheck(tree, itemnum, checkit)
	LONG		tree;
	WORD		itemnum, checkit;
{
	MM_ITREE = tree;
	ITEM_NUM = itemnum;
	CHECK_IT = checkit;
	return( crys_if(MENU_ICHECK) );
}
