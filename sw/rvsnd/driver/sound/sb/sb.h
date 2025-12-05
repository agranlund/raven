/*-------------------------------------------------------------------------------
 * rvsnd : isa soundblaster driver
 * (c)2025 Anders Granlund
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

#ifndef _SBDRIVER_H_
#define _SBDRIVER_H_

#define SBTYPE_NONE         0
#define SBTYPE_SB1          1
#define SBTYPE_SB2          2
#define SBTYPE_SBPRO        3
#define SBTYPE_SB16         4
#define SBTYPE_ESS          5

extern isa_t* bus;                  /* isa bus */
extern volatile uint8_t* sbase;     /* base address */
extern uint16_t sbport;             /* soundblaster port */
extern uint16_t sbirq;              /* soundblaster irq */
extern uint16_t sbtype;             /* soundblaster type  */
extern uint16_t wssport;            /* windows soundsystem port */

#define reg_read(r)     *((volatile uint8_t*)(sbase+(r)))
#define reg_write(r,d)  *((volatile uint8_t*)(sbase+(r))) = d


/* mixer */
extern bool mixer_init(void);


#endif /* _SBDRIVER_H_ */
