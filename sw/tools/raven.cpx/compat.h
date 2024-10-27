/*
*	Colection of def needed to compilation 
*/

#define CLOCKS_PER_SEC CLK_TCK   /* missing declaration in time.h */
#define Vsetmode(mode) (int)xbios((short)0x58,(short)(mode))  /*missing from videocnf.c */