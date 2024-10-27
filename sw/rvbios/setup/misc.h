/*
	Setup GUI
	Misc functions

	Copyright (C) 2024	Anders Granlund
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

#ifndef MISC_H
#define MISC_H 1

/* Format a number in a string */
void format_number(char *str, long value, int num_chars, char null_digit);

/* Format a number in a hexa string */
void format_number_hex(char *str, long value, int num_chars, int prefix);

/* String length */
int strLength(const char *str);

/* String copy */
void strCopy(const char *src, char *dest);

/* String to int */
int strToInt(const char *src);

#endif /* MISC_H */
