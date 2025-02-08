/*
 * GEM resource C output of serial
 *
 * created by ORCS 2.16
 */

#ifndef _LONG_PTR
#  define _LONG_PTR LONG
#endif

#ifndef OS_NORMAL
#  define OS_NORMAL 0x0000
#endif
#ifndef OS_SELECTED
#  define OS_SELECTED 0x0001
#endif
#ifndef OS_CROSSED
#  define OS_CROSSED 0x0002
#endif
#ifndef OS_CHECKED
#  define OS_CHECKED 0x0004
#endif
#ifndef OS_DISABLED
#  define OS_DISABLED 0x0008
#endif
#ifndef OS_OUTLINED
#  define OS_OUTLINED 0x0010
#endif
#ifndef OS_SHADOWED
#  define OS_SHADOWED 0x0020
#endif
#ifndef OS_WHITEBAK
#  define OS_WHITEBAK 0x0040
#endif
#ifndef OS_DRAW3D
#  define OS_DRAW3D 0x0080
#endif

#ifndef OF_NONE
#  define OF_NONE 0x0000
#endif
#ifndef OF_SELECTABLE
#  define OF_SELECTABLE 0x0001
#endif
#ifndef OF_DEFAULT
#  define OF_DEFAULT 0x0002
#endif
#ifndef OF_EXIT
#  define OF_EXIT 0x0004
#endif
#ifndef OF_EDITABLE
#  define OF_EDITABLE 0x0008
#endif
#ifndef OF_RBUTTON
#  define OF_RBUTTON 0x0010
#endif
#ifndef OF_LASTOB
#  define OF_LASTOB 0x0020
#endif
#ifndef OF_TOUCHEXIT
#  define OF_TOUCHEXIT 0x0040
#endif
#ifndef OF_HIDETREE
#  define OF_HIDETREE 0x0080
#endif
#ifndef OF_INDIRECT
#  define OF_INDIRECT 0x0100
#endif
#ifndef OF_FL3DIND
#  define OF_FL3DIND 0x0200
#endif
#ifndef OF_FL3DBAK
#  define OF_FL3DBAK 0x0400
#endif
#ifndef OF_FL3DACT
#  define OF_FL3DACT 0x0600
#endif
#ifndef OF_MOVEABLE
#  define OF_MOVEABLE 0x0800
#endif
#ifndef OF_POPUP
#  define OF_POPUP 0x1000
#endif

#ifndef G_SWBUTTON
#  define G_SWBUTTON 34
#endif
#ifndef G_POPUP
#  define G_POPUP 35
#endif
#ifndef G_EDIT
#  define G_EDIT 37
#endif
#ifndef G_SHORTCUT
#  define G_SHORTCUT 38
#endif
#ifndef G_SLIST
#  define G_SLIST 39
#endif
#ifndef G_EXTBOX
#  define G_EXTBOX 40
#endif
#ifndef G_OBLINK
#  define G_OBLINK 41
#endif

#ifndef WHITEBAK
#  define WHITEBAK OS_WHITEBAK
#endif
#ifndef DRAW3D
#  define DRAW3D OS_DRAW3D
#endif
#ifndef FL3DIND
#  define FL3DIND OF_FL3DIND
#endif
#ifndef FL3DBAK
#  define FL3DBAK OF_FL3DBAK
#endif
#ifndef FL3DACT
#  define FL3DACT OF_FL3DACT
#endif

#ifndef C_UNION
#ifdef __PORTAES_H__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __GEMLIB__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __PUREC__
#  define C_UNION(x) { (_LONG_PTR)(x) }
#endif
#ifdef __ALCYON__
#  define C_UNION(x) x
#endif
#endif
#ifndef C_UNION
#  define C_UNION(x) (_LONG_PTR)(x)
#endif

#define T0OBJ 0
#define FREEBB 0
#define FREEIMG 0
#define FREESTR 45

BYTE *rs_strings[] = {
	(BYTE *)"This port is in use,",
	(BYTE *)"and therefore cannot be",
	(BYTE *)"configured !",
	(BYTE *)"Unknown",
	(BYTE *)"   8   ",
	(BYTE *)"  Odd  ",
	(BYTE *)"   2   ",
	(BYTE *)"bps",
	(BYTE *)"Bits",
	(BYTE *)"Parity",
	(BYTE *)"Stop Bits",
	(BYTE *)"FlowCTRL :",
	(BYTE *)"Xon/Xoff",
	(BYTE *)"RSVF",
	(BYTE *)"",
	(BYTE *)"",
	(BYTE *)"supported",
	(BYTE *)"",
	(BYTE *)"",
	(BYTE *)"Buffer sizes :",
	(BYTE *)"",
	(BYTE *)"",
	(BYTE *)"_____",
	(BYTE *)"Receive : _____",
	(BYTE *)"99999",
	(BYTE *)"",
	(BYTE *)"Use LAN Port",
	(BYTE *)"_____",
	(BYTE *)"Send : _____",
	(BYTE *)"99999",
	(BYTE *)"DTR",
	(BYTE *)"Flush",
	(BYTE *)"Break",
	(BYTE *)"  Serial 2  ",
	(BYTE *)"Parameter",
	(BYTE *)"Serial Setter",
	(BYTE *)"(c)  1997  by",
	(BYTE *)"",
	(BYTE *)"",
	(BYTE *)"Peter Rottengatter",
	(BYTE *)"",
	(BYTE *)"",
	(BYTE *)"Save",
	(BYTE *)"Ok",
	(BYTE *)"Cancel"
};

LONG rs_frstr[] = {
	0
};

BITBLK rs_bitblk[] = {
	{ 0, 0, 0, 0, 0, 0 }
};

LONG rs_frimg[] = {
	0
};

ICONBLK rs_iconblk[] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

TEDINFO rs_tedinfo[] = {
	{ (BYTE *)13L, (BYTE *)14L, (BYTE *)15L, 5, 6, 0, 0x1180, 0x0, -1, 5,1 },
	{ (BYTE *)16L, (BYTE *)17L, (BYTE *)18L, 5, 6, 0, 0x1180, 0x0, -1, 10,1 },
	{ (BYTE *)19L, (BYTE *)20L, (BYTE *)21L, 5, 6, 0, 0x1180, 0x0, -1, 15,1 },
	{ (BYTE *)22L, (BYTE *)23L, (BYTE *)24L, 3, 6, 0, 0x1180, 0x0, -1, 6,16 },
	{ (BYTE *)27L, (BYTE *)28L, (BYTE *)29L, 3, 6, 0, 0x1180, 0x0, -1, 6,13 },
	{ (BYTE *)36L, (BYTE *)37L, (BYTE *)38L, 5, 6, 0, 0x1180, 0x0, -1, 14,1 },
	{ (BYTE *)39L, (BYTE *)40L, (BYTE *)41L, 5, 6, 0, 0x1180, 0x0, -1, 19,1 }
};

OBJECT rs_object[] = {
	{ -1, 1, 39, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1101L), 0x0000,0x0000, 0x0020,0x000b },
	{ 37, 2, 33, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1100L), 0x0000,0x0000, 0x0020,0x0309 },
	{ 7, 3, 3, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 0x0000,0x0b03, 0x0020,0x0805 },
	{ 2, 4, 6, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1100L), 0x0002,0x0900, 0x001c,0x0004 },
	{ 5, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(0x0L), 0x0004,0x0600, 0x0014,0x0001 },
	{ 6, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(0x1L), 0x0402,0x0801, 0x0017,0x0001 },
	{ 3, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(0x2L), 0x0008,0x0a02, 0x000c,0x0001 },
	{ 21, 8, 18, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1101L), 0x0000,0x0c03, 0x0020,0x0705 },
	{ 9, -1, -1, G_BUTTON, 0x41, OS_SHADOWED, C_UNION(0x3L), 0x0002,0x0400, 0x0008,0x0001 },
	{ 10, -1, -1, G_BUTTON, 0x41, OS_SHADOWED, C_UNION(0x4L), 0x0002,0x0801, 0x0008,0x0001 },
	{ 11, -1, -1, G_BUTTON, 0x41, OS_SHADOWED, C_UNION(0x5L), 0x0002,0x0c02, 0x0008,0x0001 },
	{ 12, -1, -1, G_BUTTON, 0x41, OS_SHADOWED, C_UNION(0x6L), 0x0002,0x0004, 0x0008,0x0001 },
	{ 13, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(0x7L), 0x000b,0x0400, 0x0003,0x0001 },
	{ 14, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(0x8L), 0x000b,0x0801, 0x0004,0x0001 },
	{ 15, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(0x9L), 0x000b,0x0c02, 0x0006,0x0001 },
	{ 16, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(0xAL), 0x000b,0x0004, 0x0009,0x0001 },
	{ 17, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(0xBL), 0x0014,0x0a00, 0x000a,0x0001 },
	{ 18, -1, -1, G_BUTTON, 0x41, OS_SHADOWED, C_UNION(0xCL), 0x0014,0x0e01, 0x000a,0x0001 },
	{ 7, 19, 20, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 0x0516,0x0c03, 0x0107,0x0001 },
	{ 20, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(0x0L), 0x0102,0x0100, 0x0003,0x0600 },
	{ 18, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(0x1L), 0x0100,0x0900, 0x0606,0x0600 },
	{ 31, 22, 30, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1100L), 0x0000,0x0c03, 0x0020,0x0705 },
	{ 23, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(0x2L), 0x0002,0x0800, 0x050a,0x0c00 },
	{ 24, -1, -1, G_FTEXT, OF_EDITABLE, OS_NORMAL, C_UNION(0x3L), 0x0003,0x0402, 0x000f,0x0001 },
	{ 27, 25, 26, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 0x0003,0x0004, 0x000f,0x0001 },
	{ 26, -1, -1, (0x1<<8)+G_BUTTON, OF_SELECTABLE, 0x6, C_UNION(0x19L), 0x0000,0x0000, 0x0002,0x0001 },
	{ 24, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(0x1AL), 0x0003,0x0000, 0x000c,0x0001 },
	{ 28, -1, -1, G_FTEXT, OF_EDITABLE, OS_NORMAL, C_UNION(0x4L), 0x0006,0x0401, 0x000c,0x0001 },
	{ 29, -1, -1, G_BUTTON, OF_SELECTABLE, OS_NORMAL, C_UNION(0x1EL), 0x0016,0x0001, 0x0007,0x0001 },
	{ 30, -1, -1, G_BUTTON, 0x41, OS_NORMAL, C_UNION(0x1FL), 0x0016,0x0802, 0x0007,0x0001 },
	{ 21, -1, -1, G_BUTTON, 0x41, OS_NORMAL, C_UNION(0x20L), 0x0016,0x0004, 0x0007,0x0001 },
	{ 32, -1, -1, G_BUTTON, 0x41, OS_SHADOWED, C_UNION(0x21L), 0x0002,0x0800, 0x000c,0x0001 },
	{ 33, -1, -1, G_BUTTON, 0x41, OS_SHADOWED, C_UNION(0x22L), 0x0002,0x0202, 0x000c,0x0001 },
	{ 1, 34, 36, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 0x0610,0x0400, 0x000e,0x0d02 },
	{ 35, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(0x23L), 0x0400,0x0600, 0x000d,0x0001 },
	{ 36, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(0x5L), 0x0002,0x0601, 0x000a,0x0d00 },
	{ 33, -1, -1, G_TEXT, OF_NONE, OS_NORMAL, C_UNION(0x6L), 0x0200,0x0f01, 0x050d,0x0d00 },
	{ 39, 38, 38, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1100L), 0x0000,0x0409, 0x070b,0x0c01 },
	{ 37, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(0x2AL), 0x0701,0x0600, 0x0008,0x0001 },
	{ 0, 40, 41, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1100L), 0x000c,0x0409, 0x0014,0x0c01 },
	{ 41, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(0x2BL), 0x0202,0x0600, 0x0005,0x0001 },
	{ 39, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(0x2CL), 0x0009,0x0600, 0x0009,0x0001 }
};

_LONG_PTR rs_trindex[] = {
	0L
};

#ifndef __foobar_defined
#define __foobar_defined 1
struct foobar {
	WORD 	dummy;
	WORD 	*image;
};
#endif
struct foobar rs_imdope[] = {
	{ 0, 0 }
};



#define NUM_STRINGS 45
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 7
#define NUM_OBS 42
#define NUM_TREE 1

BYTE pname[] = "SERIAL.RSC";
