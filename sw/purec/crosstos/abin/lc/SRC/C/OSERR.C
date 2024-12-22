/*
 * oserr.c - define _OSERR error text
 *
 * $Log: oserr.c,v $
 * Revision 1.3  1991/12/10  16:23:18  AGK
 * Added MetaDOS(tm) and Network GEMDOS error codes
 *
 * Revision 1.2  1991/11/23  15:52:56  AGK
 * Added network GEMDOS error codes
 *
 * Revision 1.1  1991/06/17  12:48:08  AGK
 * Initial revision
 *
 * Copyright (c) 1991 HiSoft
 */

const char *os_errlist[] = {
				"Unknown error code",
/* 01 */		"General failure",
/* 02 */		"Drive not ready",
/* 03 */		"Unknown command",
/* 04 */		"Data error",
/* 05 */		"Bad request structure length",
/* 06 */		"Seek error",
/* 07 */		"Unknown media type",
/* 08 */		"Sector not found",
/* 09 */		"Printer paper alarm",
/* 10 */		"Write fault",
/* 11 */		"Read fault",
/* 12 */		"Error 12",
/* 13 */		"Can't write on protected device",
/* 14 */		"Invalid disk change",
/* 15 */		"Unknown unit",
/* 16 */		"Bad sectors on format",
/* 17 */		"Insert other disk",
/* 18 */		"Insert disc",
/* 19 */		"Device not responding",
/* 20 */		"Hardware error",
/* 21 */		"Error 21",
/* 22 */		"Error 22",
/* 23 */		"Error 23",
/* 24 */		"Error 24",
/* 25 */		"Error 25",
/* 26 */		"Error 26",
/* 27 */		"Error 27",
/* 28 */		"Error 28",
/* 29 */		"Error 29",
/* 30 */		"Error 30",
/* 31 */		"Error 31",
/* 32 */		"Invalid function number",
/* 33 */		"File not found",
/* 34 */		"Path not found",
/* 35 */		"Too many files opened",
/* 36 */		"Access denied",
/* 37 */		"Invalid handle",
/* 38 */		"Error 38",
/* 39 */		"Insufficient memory",
/* 40 */		"Invalid memory block address",
/* 41 */		"Error 41",
/* 42 */		"Error 42",
/* 43 */		"Error 43",
/* 44 */		"Error 44",
/* 45 */		"Error 45",
/* 46 */		"Invalid drive code",
/* 47 */		"Error 47",
/* 48 */		"Not same device",
/* 49 */		"No more files",
/* 50 */		"Error 50",
/* 51 */		"Error 51",
/* 52 */		"Error 52",
/* 53 */		"Error 53",
/* 54 */		"Error 54",
/* 55 */		"Error 55",
/* 56 */		"Error 56",
/* 57 */		"Error 57",
/* 58 */		"Record is locked",
/* 59 */		"Matching lock not found",
/* 60 */		"Error 60",
/* 61 */		"Error 61",
/* 62 */		"Error 62",
/* 63 */		"Error 63",
/* 64 */		"Range error",
/* 65 */		"GEMDOS internal error",
/* 66 */		"Invalid program load format",
/* 67 */		"Memory growth failure",
};

/* Highest valid error number */
int os_nerr = sizeof(os_errlist)/sizeof(os_errlist[0]);
