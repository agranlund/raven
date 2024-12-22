/**
 * $Header: e:/src/gem/aes/util\_aespb.c,v 1.2 1993/04/01 15:28:06 AGK Exp $
 *
 * AES parameter block
 *
 * (c) Copyright 1989, 1993 HiSoft
**/

#include <aes.h>

void *_AESpb[6] = {
	_AEScontrol, _AESglobal, _AESintin, _AESintout, _AESaddrin, _AESaddrout
};
