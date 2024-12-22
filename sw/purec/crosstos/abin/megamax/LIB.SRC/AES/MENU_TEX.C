
#include <portab.h>
#include <gembind.h>



	WORD
menu_text(tree, inum, ptext)
	LONG		tree;
	WORD		inum;
	LONG		ptext;
{
	MM_ITREE = tree;
	ITEM_NUM = inum;
	MM_PTEXT = ptext;
	return( crys_if( MENU_TEXT ) );
}
