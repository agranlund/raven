/*
 * Written to be used in uiptool by Christian Zietz 2018.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __ASIX_H__
#define __ASIX_X__

#include "usb_ether.h"

void asix_eth_before_probe(void *a);
long asix_eth_probe(struct usb_device *dev, unsigned int ifnum, struct ueth_data *ss);
long asix_eth_get_info(struct usb_device *dev, struct ueth_data *ss, unsigned char* mac);
long asix_send(struct ueth_data *dev, void *packet, long length);
long asix_recv(struct ueth_data *dev, unsigned char *dest_buf, unsigned long dest_len);

#endif