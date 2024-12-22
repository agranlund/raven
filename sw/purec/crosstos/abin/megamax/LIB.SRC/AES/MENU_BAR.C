
#include <portab.h>
#include <gembind.h>


					/* Menu Manager			*/
	WORD
menu_bar(tree, showit)
	LONG		tree;
	WORD		showit;
{
	MM_ITREE = tree;
	SHOW_IT = showit;
	return( crys_if(MENU_BAR) );
}
