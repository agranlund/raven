/*-------------------------------------------------------------------------------
 * Raven support software
 * (c)2024 Anders Granlund
 *-------------------------------------------------------------------------------
 * This file is free software  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation  either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program  if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *-----------------------------------------------------------------------------*/
#ifndef _RVBIOS_H_
#define _RVBIOS_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "raven.h"

#define RVBIOS_VERSION	0x241027L

/*
	main.c
*/

extern const  	raven_t* g_rv;
extern void		SetCookie(uint32_t id, uint32_t value);


/*
	xbios.s
*/
extern void		InstallXbios(void);
extern uint16_t ipl_set(uint16_t ipl);


/*
	xbios_c.c
*/
extern uint32_t xbc_gettime(void);
extern int16_t	xbc_nvmaccess(uint16_t op, int16_t start, int16_t count, uint8_t* buffer);


/*
	eiffel.s
*/
extern void		InstallEiffel(void);


/*
	sp060.s
*/
extern void 	Install060sp(void);


/*
	utils.s
*/
extern uint16_t	ipl_set(uint16_t ipl);
extern uint32_t	pcr_get(void);
extern uint32_t	vbr_get(void);
extern uint32_t cacr_get(void);
extern uint32_t ticks_get(void);
extern void		cache_clear(void);

#endif /* _RVBIOS_H_ */

