/*
	System info

    Copyright (C) 2024  Anders Granlund
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

#ifndef FORM_INFO_H
#define FORM_INFO_H

/*--- Variables ---*/

extern const form_menu_t form_menu_info;
extern form_setting_t form_setting_info[];

/*--- Functions prototypes ---*/

void initFormInfo(void);

void displayFormInfo(void);
void updateFormInfo(void);

#endif /* FORM_INFO_H */
