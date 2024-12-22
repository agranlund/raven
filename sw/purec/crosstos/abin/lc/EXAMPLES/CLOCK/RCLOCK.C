#include <aes.h>

OBJECT rclock[] = {
/*0*/	{-1,1,1,G_BOX,0x0,0x0,(void *)0x1181,1095,1538,8,1},
/*1*/	{0,-1,-1,G_STRING,0x20,0x0,"00:00:00", 0,0,8,1},
};


char clock_menu[] = "  Clock ";
char clock_title[] = "Clock";

static void fix_tree(OBJECT *s,int max)
{
	int i;

	for(i=0; i<=max; i++)
		rsrc_obfix(s,i);
}

void rsrc_init(void)
{
	fix_tree(rclock,1);
}
