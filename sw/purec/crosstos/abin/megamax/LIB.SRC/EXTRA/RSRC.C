#include <gemdefs.h>
#include <obdefs.h>
#include <stdio.h>
#include <fcntl.h>

/*
	Replacement for ROM rsrc_ routines
*/
extern int global[15];

int mrsrc_load(tree)
char *tree;
{
	register int i;
	RSHDR header;
	int fd;
	char **obpp;
	OBJECT *obp;
	TEDINFO *tep;
	ICONBLK *ibp;
	BITBLK *bbp;

	bcopy(tree, &header, sizeof(header));

	obpp = (char **)(tree + header.rsh_trindex);
	global[5] = ((long)obpp >> 16) & 0xffff;
	global[6] = (long)obpp & 0xffff;
	for (i=0; i<header.rsh_ntree; i++)
		obpp[i] += (long)tree;

	obp = (OBJECT *)(tree + header.rsh_object);
	for (i=0; i<header.rsh_nobs; i++) {
		switch (obp[i].ob_type) {
			case G_TITLE:
			case G_STRING:
			case G_BUTTON:
			case G_TEXT:
			case G_BOXTEXT:
			case G_FTEXT:
			case G_FBOXTEXT:
			case G_ICON:
			case G_IMAGE:
				obp[i].ob_spec += (long)tree;
				break;
		}

		rsrc_obfix(obp, i);		/* fix size and location */
	}

	tep = (TEDINFO *)(tree + header.rsh_tedinfo);
	for (i=0; i<header.rsh_nted; i++) {
		tep[i].te_ptext += (long)tree;
		tep[i].te_ptmplt += (long)tree;
		tep[i].te_pvalid += (long)tree;
	}

	ibp = (ICONBLK *)(tree + header.rsh_iconblk);
	for (i=0; i<header.rsh_nib; i++) {
		ibp[i].ib_pmask = (int *)((char *)ibp[i].ib_pmask + (long)tree);
		ibp[i].ib_pdata = (int *)((char *)ibp[i].ib_pdata + (long)tree);
		ibp[i].ib_ptext += (long)tree;
	}

	bbp = (BITBLK *)(tree + header.rsh_bitblk);
	for (i=0; i<header.rsh_nbb; i++)
		bbp[i].bi_pdata = (int *)((char *)bbp[i].bi_pdata + (long)tree);

	return 1;
}
