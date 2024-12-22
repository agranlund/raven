
#include <portab.h>
#include <gembind.h>


	WORD
menu_tnormal(tree, titlenum, normalit)
	LONG		tree;
	WORD		titlenum, normalit;
{
	MM_ITREE = tree;
	TITLE_NUM = titlenum;
	NORMAL_IT = normalit;
	return( crys_if( MENU_TNORMAL ) );
}
