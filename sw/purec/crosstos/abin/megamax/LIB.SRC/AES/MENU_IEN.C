
#include <portab.h>
#include <gembind.h>


	WORD
menu_ienable(tree, itemnum, enableit)
	LONG		tree;
	WORD		itemnum, enableit;
{
	MM_ITREE = tree;
	ITEM_NUM = itemnum;
	ENABLE_IT = enableit;
	return( crys_if(MENU_IENABLE) );
}

