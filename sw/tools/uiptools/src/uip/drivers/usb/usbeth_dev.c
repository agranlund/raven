/*
 * Interfacing ASIX USB2LAN adapters to uiptool.
 * Written by Christian Zietz 2018.
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


#include "usbeth_dev.h"
#include <stdio.h>
#include <string.h>
#include <osbind.h>
#include "uip.h"
#include "usb.h"
#include "usb_api.h"
#include "usb_ether.h"

#include "asix.h"
#include "picowifi.h"

//#define DEBUGOUT(x) printf x
#define DEBUGOUT(x)

#define TOTAL_HEADER_LENGTH (UIP_TCPIP_HLEN+UIP_LLH_LEN)

/*
 * Private variables 
 */
 
static bool asix_found = false;
static bool picowifi_found = false;
static unsigned char mac[6];

/* 
 * USB API 
 */

static struct usb_module_api *api;
static struct ueth_data ueth_dev;
static struct usb_device *mydev = NULL;

struct cookie
{
	long tag;
	long value;
};

#define _USB 0x5f555342L
#define CJAR ((struct cookie **) 0x5a0)

static inline struct usb_module_api *
get_usb_cookie (void)
{
	struct usb_module_api *api;
	//long ret;
	struct cookie *cjar;

	//ret = Super(0L);

	api = NULL;
	cjar = *CJAR;

	while (cjar->tag)
	{
		if (cjar->tag == _USB)
		{
			api = (struct usb_module_api *)cjar->value;
	
			//SuperToUser(ret);

			return api;
		}

		cjar++;
	}

	//SuperToUser(ret);

	return NULL;
}


/*
 * USB device interface
 */

static long ethernet_probe		(struct usb_device *dev, unsigned short ifnum);
static long ethernet_disconnect		(struct usb_device *dev);
static long ethernet_ioctl		(struct uddif *, short, long);

static char lname[] = "USB ethernet class driver\0";

static struct uddif eth_uif = 
{
	0,			/* *next */
	USB_API_VERSION,	/* API */
	USB_DEVICE,		/* class */
	lname,			/* lname */
	"eth",			/* name */
	0,			/* unit */
	0,			/* flags */
	ethernet_probe,		/* probe */
	ethernet_disconnect,	/* disconnect */
	0,			/* resrvd1 */
	ethernet_ioctl,		/* ioctl */
	0,			/* resrvd2 */
};

static long ethernet_probe(struct usb_device *dev, unsigned short ifnum)
{
	/*
	 * Note the different calling conventions: USB stack is compiled
	 * with -mshort, whereas uIP needs to be built without -mshort.
	 * Hence, here the 'ifnum' parameter is not passed correctly,
	 * when the USB stack calls ethernet_probe().
	 * Luckily, we know that both Asix and PicoWifi only have interface 0,
	 * and can therefore hardcode it in the calls to ..._probe().
	 */

	long old_async;
	
	DEBUGOUT(("ethernet_probe: %p\r\n", dev));

	if (dev == NULL)
		return -1;

	old_async = usb_disable_asynch(1); /* asynch transfer not allowed */

	asix_eth_before_probe(api);
	picowifi_eth_before_probe(api);
	if (asix_eth_probe(dev, 0, &ueth_dev))
	{
		if (asix_eth_get_info(dev, &ueth_dev, mac)) {
			asix_found = true;
			/* Save for later: Note: assumes only *ONE* device. */
			mydev = dev;
		}
	} else if (picowifi_eth_probe(dev, 0, &ueth_dev))
	{
		if (picowifi_eth_get_info(dev, &ueth_dev, mac)) {
			picowifi_found = true;
			/* Save for later: Note: assumes only *ONE* device. */
			mydev = dev;
		}
	}

	usb_disable_asynch(old_async); /* restore asynch value */
	return (asix_found || picowifi_found) ?0:-1;

}

static long ethernet_disconnect(struct usb_device *dev)
{
	return 0;
}

static long ethernet_ioctl(struct uddif *u, short cmd, long arg)
{
	return 0;
}

bool USBETHdev_init(uint8_t* macaddr, uint32_t cpu_type)
{
	long ret;

	DEBUGOUT(("USBETHdev_init()\r\n"));
	api = get_usb_cookie();
	if (!api)
	{
		(void)Cconws("ETH failed to get _USB cookie\r\n");
		return false;
	}

	DEBUGOUT(("udd_register()"));
	ret = udd_register(&eth_uif);
	if (ret)
	{
		(void)Cconws("\7\r\nSorry, failed!\r\n\r\n");
		return false;
	}

	memcpy(macaddr, mac, sizeof(mac));
	return true;
}

void USBETHdev_done(void)
{
	long i;

	DEBUGOUT(("USBETHdev_done()\r\n"));
	/* Ugly hack: 
	 * Before we go, we have to disconnect the driver from the interfaces.
	 * Otherwise, when the user disconnects the device, computer will crash.
	 */
	if (mydev) {
		for (i = 0; i < mydev->config.no_of_if; i++) {
			if(mydev->config.if_desc[i].driver)
				mydev->config.if_desc[i].driver->disconnect(mydev);
			mydev->config.if_desc[i].driver = NULL;
		}
	}

	udd_unregister(&eth_uif);
}

void USBETHdev_send(void)
{
	DEBUGOUT(("USBETHdev_send\r\n"));

	if (!(asix_found || picowifi_found))
		return;

	if (uip_len > 2048) {
		DEBUGOUT(("buffer overflow = %d\r\n", uip_len));
		return;
	}

	// send packet, using data in uip_appdata if over the IP+TCP header size
	if( uip_len <= TOTAL_HEADER_LENGTH ) {
		if (asix_found) {
			asix_send(&ueth_dev, uip_buf, uip_len);
		} else if (picowifi_found) {
			picowifi_send(&ueth_dev, uip_buf, uip_len);
		}
	} else {
		unsigned char buffer[2048];
		memcpy(&buffer[0], uip_buf, TOTAL_HEADER_LENGTH);
		memcpy(&buffer[TOTAL_HEADER_LENGTH], uip_appdata, uip_len-TOTAL_HEADER_LENGTH);
		if (asix_found) {
			asix_send(&ueth_dev, buffer, uip_len);
		} else if (picowifi_found) {
			picowifi_send(&ueth_dev, buffer, uip_len);
		}
	}

	return;
}

unsigned int USBETHdev_poll(void)
{
	long r = 0;

	if (asix_found) {
		r = asix_recv(&ueth_dev, uip_buf, UIP_BUFSIZE);
	} else if (picowifi_found) {
		r = picowifi_recv(&ueth_dev, uip_buf, UIP_BUFSIZE);
	}

	return r>0?r:0;
}

unsigned int USBETHdev_printstatus(void)
{
#ifdef USB_PRINTSTATUS
	if (asix_found) {
		printf("USB device: ASIX\r\n");
		printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	} else if (picowifi_found) {
		printf("USB device: PicoWifi\r\n");
		printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		picowifi_printstatus(&ueth_dev);
	}
#endif
	return 0;
}