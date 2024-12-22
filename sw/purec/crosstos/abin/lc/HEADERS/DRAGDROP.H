/**
 * $Id: dragdrop.h,v 1.2 1993/11/14 14:43:52 AGK Rel $
 *
 * GEM AES Drag'n'Drop protocol
 *
 * (c) Copyright 1992, 1993 Atari Corp.
 * (c) Copyright 1993 HiSoft
**/

#ifndef _DRAGDROP_H
#define _DRAGDROP_H

#define	DD_OK		0
#define DD_NAK		1
#define DD_EXT		2
#define DD_LEN		3
#define DD_TRASH	4
#define DD_PRINTER	5
#define DD_CLIPBOARD	6

/* timeout in milliseconds */
#define DD_TIMEOUT	4000

/* number of bytes of "preferred extensions" sent by receipient during
 * open
 */
#define DD_NUMEXTS	8
#define DD_EXTSIZE	32L

/* max size of a drag'n'drop item name */
#define DD_NAMEMAX	128

/* max length of a drag'n'drop header */
#define DD_HDRMAX	(8 + DD_NAMEMAX)
#endif
