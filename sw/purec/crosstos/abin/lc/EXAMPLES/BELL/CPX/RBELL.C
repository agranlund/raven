#include <aes.h>

TEDINFO rs_tedinfo[] = {
/*0*/	{"Sample Directory Path", "", "", IBM,0,TE_CNTR,0x1181,0,-1,22,1},
/*1*/	{"C:\\SOUNDS\\*.SND__________", "_________________________", "", IBM,0,TE_CNTR,0x1181,0,-1,26,26},
/*2*/	{"SCANNING SAMPLES...", "", "", IBM,0,TE_CNTR,0x1181,0,0,20,1},
/*3*/	{"File name", "", "", IBM,0,TE_CNTR,0x1181,0,-1,10,1},
/*4*/	{"12345678123", " ________.___", "F", IBM,0,TE_LEFT,0x1181,0,-1,12,14},
/*5*/	{"Sample rate", "", "", IBM,0,TE_CNTR,0x1181,0,-1,12,1},
/*6*/	{"", "", "", IBM,6,TE_CNTR,0x1180,0,-1,1,1},
/*7*/	{"Sample name", "", "", IBM,0,TE_CNTR,0x1181,0,-1,12,1},
/*8*/	{"0123456789012345678901234567", " ____________________________", "X", IBM,0,TE_LEFT,0x1181,0,-1,29,30},
};


OBJECT bell_box[] = {
/*0*/	{-1,1,19,G_BOX,0x0,0x0,(void *)0x1100,64,64,256,176},
/*1*/	{2,-1,-1,G_BOXCHAR,0x40,0x0,(void *)0x1ff1181,238,0,18,18},
/*2*/	{4,3,3,G_BOX,0x40,0x0,(void *)0x11191,237,18,20,60},
/*3*/	{2,-1,-1,G_BOX,0x40,0x0,(void *)0x11190,0,0,20,16},
/*4*/	{5,-1,-1,G_BOXCHAR,0x40,0x0,(void *)0x2ff1181,238,78,18,18},
/*5*/	{12,6,11,G_BOX,0x0,0x0,(void *)0x11f0,0,0,237,96},
/*6*/	{7,-1,-1,G_STRING,0x40,0x0,"", 0,0,237,16},
/*7*/	{8,-1,-1,G_STRING,0x40,0x0,"", 0,16,237,16},
/*8*/	{9,-1,-1,G_STRING,0x40,0x0,"", 0,32,237,16},
/*9*/	{10,-1,-1,G_STRING,0x40,0x0,"", 0,48,237,16},
/*10*/	{11,-1,-1,G_STRING,0x40,0x0,"", 0,64,237,16},
/*11*/	{5,-1,-1,G_STRING,0x40,0x0,"", 0,80,237,16},
/*12*/	{13,-1,-1,G_BOXTEXT,0x0,0x0,&rs_tedinfo[0],0,97,256,16},
/*13*/	{17,14,16,G_BOX,0x0,0x0,(void *)0xff11c1,0,114,256,27},
/*14*/	{15,-1,-1,G_BOXCHAR,0x40,0x0,(void *)0x4011181,4,4,19,19},
/*15*/	{16,-1,-1,G_FBOXTEXT,0x40,0x0,&rs_tedinfo[1],27,5,202,17},
/*16*/	{13,-1,-1,G_BOXCHAR,0x40,0x0,(void *)0x3011181,233,4,19,19},
/*17*/	{19,18,18,G_BOX,0x0,0x0,(void *)0xff1181,0,142,86,34},
/*18*/	{17,-1,-1,G_BUTTON,0x5,0x0,"Save", 10,9,64,16},
/*19*/	{0,20,21,G_BOX,0x0,0x0,(void *)0xff1181,88,142,168,34},
/*20*/	{21,-1,-1,G_BUTTON,0x7,0x0,"OK", 12,9,64,16},
/*21*/	{19,-1,-1,G_BUTTON,0x25,0x0,"Cancel", 91,9,64,16},
};

OBJECT snd_scanning[] = {
/*0*/	{-1,1,1,G_BOX,0x0,0x10,(void *)0x21181,15,5,168,28},
/*1*/	{0,-1,-1,G_TEXT,0x20,0x0,&rs_tedinfo[2],0,0,168,28},
};

OBJECT snd_reconfigure[] = {
/*0*/	{-1,1,9,G_BOX,0x0,0x0,(void *)0x1141,58,42,256,176},
/*1*/	{2,-1,-1,G_BOXTEXT,0x0,0x0,&rs_tedinfo[3],0,0,256,16},
/*2*/	{3,-1,-1,G_FBOXTEXT,0x8,0x0,&rs_tedinfo[4],72,21,112,17},
/*3*/	{4,-1,-1,G_BOXTEXT,0x0,0x0,&rs_tedinfo[5],0,45,256,16},
/*4*/	{5,-1,-1,G_BOXTEXT,0x40,0x20,&rs_tedinfo[6],84,67,88,19},
/*5*/	{6,-1,-1,G_BOXTEXT,0x0,0x0,&rs_tedinfo[7],0,94,256,16},
/*6*/	{7,-1,-1,G_FBOXTEXT,0x8,0x0,&rs_tedinfo[8],8,117,240,17},
/*7*/	{9,8,8,G_BOX,0x0,0x0,(void *)0xff1181,0,142,127,34},
/*8*/	{7,-1,-1,G_BUTTON,0x5,0x0,"Save", 31,9,64,16},
/*9*/	{0,10,10,G_BOX,0x0,0x0,(void *)0xff1181,129,142,127,34},
/*10*/	{9,-1,-1,G_BUTTON,0x27,0x0,"Cancel", 31,9,64,16},
};


char snd_fsel[] = "Select Sample Directory Path";
char snd_standard[] = "Standard bell";
char snd_tsrmissing[] = "[1][You must run the Sample|Allocator program before|you can configure it|with this CPX.][  OK  ]";
char snd_locatetsr[] = "Please locate BELL.PRG";
char poptext_1[] = "   6258Hz ";
char poptext_2[] = "  12517Hz ";
char poptext_3[] = "  25033Hz ";
char poptext_4[] = "  50066Hz ";

