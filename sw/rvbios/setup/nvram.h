/*
	Setup GUI
	NVRAM functions

	Copyright (C) 2009	Patrice Mandin

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef NVRAM_H
#define NVRAM_H 1

/*--- Defines ---*/

#define NVRAM_BOOT		1
#define  NVRAM_BOOT_NONE		0x00
#define  NVRAM_BOOT_MAGIC		0x08
#define  NVRAM_BOOT_LINUX		0x10
#define  NVRAM_BOOT_NETBSD		0x20
#define  NVRAM_BOOT_SVR4		0x40
#define  NVRAM_BOOT_TOS			0x80
#define NVRAM_LANGUAGE		6
#define NVRAM_KEYBOARD		7
#define NVRAM_DATE_FMT		8
#define  NVRAM_DATE_FMT_TIMEMASK	0x10
#define   NVRAM_DATE_FMT_12H		0x00
#define   NVRAM_DATE_FMT_24H		0x10
#define  NVRAM_DATE_FMT_DATEMASK	0x03
#define   NVRAM_DATE_FMT_MMDDYY		0x00
#define   NVRAM_DATE_FMT_DDMMYY		0x01
#define   NVRAM_DATE_FMT_YYMMDD		0x02
#define   NVRAM_DATE_FMT_YYDDMM		0x03
#define NVRAM_DATE_SEP		9
#define NVRAM_DELAY		10
#define NVRAM_VIDEO_HI		14
#define NVRAM_VIDEO_LO		15
#define NVRAM_SCSI_ARB		16
#define  NVRAM_SCSI_ARB_ENABLE		0x80
#define  NVRAM_SCSI_ARB_DEVMASK		7

/*--- Defines ---*/

#ifndef NVM_READ
#define NVM_READ 0
#endif

#ifndef NVM_WRITE
#define NVM_WRITE 1
#endif

#ifndef NVM_RESET
#define NVM_RESET 2
#endif

#endif /* NVRAM_H */
