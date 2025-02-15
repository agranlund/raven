/*
 * Modified for the FreeMiNT USB subsystem by Alan Hourihane 2014.
 * Modified to be used in uiptool by Christian Zietz 2018
 *          and using asix_recv() code by Roger Burrows.
 *
 * Modified for PicoWifi adapter by Christian Zietz 2022.
 *
 * Copyright (c) 2011 The Chromium OS Authors.
 * See file CREDITS for list of people who contributed to this
 * project.
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

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
#define __u32 u32
#define __u16 u16
#define __u8  u8

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <osbind.h>

#include "picowifi.h"
#include "delay.h"

#include "usb.h"
#include "usb_api.h"
#include "usb_ether.h"
#include "mii.h"

static struct usb_module_api   *api = NULL;

/*
 * Glue defines
 */
#define ALLOC_CACHE_ALIGN_BUFFER(x, y, z) x y[z]
#define le2cpu16(x) ((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))
#define le2cpu32(x) ((((x) & 0xFF000000UL) >> 24) | (((x) & 0x00FF0000UL) >> 8) | (((x) & 0x0000FF00UL) << 8) | (((x) & 0x000000FFUL) << 24))
#define cpu2le16(x) le2cpu16((x))
#define cpu2le32(x) le2cpu32((x))
#define mdelay(x)   Delay_microsec(1000UL * (long)(x))
#define ETH_ALEN    6   // length of a MAC address

/*
 * Debug section
 */

//#define DEBUGOUT(x) printf x
#define DEBUGOUT(x)
#define ALERT(x) printf x

/* local vars */

/* driver private */

#define MTU 1600
#define MAGIC 0xAA55AA55ul

typedef struct {
	u32 magic;
	u32 len;
	u8 payload[MTU];
} pkt_s;

typedef struct {
	u32 magic;
	u32 len;
} pkt_hdr_s;

/*
 * picowifi callbacks
 */

#ifndef offsetof
#define offsetof __builtin_offsetof
#endif
#define USB_BULK_SEND_TIMEOUT 5000
#define USB_BULK_RECV_TIMEOUT 5000
#define USB_CTRL_SET_TIMEOUT 5000
#define USB_CTRL_GET_TIMEOUT 5000

#define VENDOR_REQUEST_WIFI 2

#define WIFI_CONNECT_TIMEOUT 30000 /* ms */

enum {
	WIFI_SET_SSID = 0,
	WIFI_SET_PASSWD,
	WIFI_CONNECT,
	WIFI_STATUS,
	FIRMWARE_UPDATE = 0x100,
};

static long picowifi_load_wificred(char** wifi_ssid, char** wifi_pass)
{
	static char buffer[256]; /* will return pointer within this buffer, therefore static */
	long fhandle, res;
	int idx;

	*wifi_ssid = NULL;
	*wifi_pass = NULL;

	/* If drive C: exists, try to load from C:, else from A: */
	if (Drvmap() & (1<<2)) {
		fhandle = Fopen("C:\\WIFICRED.CFG", 0);
	} else {
		fhandle = Fopen("A:\\WIFICRED.CFG", 0);
	}

	if (fhandle < 0) {
		/* Try /etc/wificred.cfg for MiNT */
		fhandle = Fopen("U:\\etc\\wificred.cfg", 0);
	}

	if (fhandle < 0) {
		return -1;
	}

	res = Fread(fhandle, sizeof(buffer), buffer);
	Fclose(fhandle);
	if (res <= 0) {
		return -1;
	}

	/* zero terminate string */
	buffer[res] = '\0';
	*wifi_ssid = buffer;

	idx = 0;
	/* find password */
	while (buffer[idx]) {
		if (buffer[idx] == '\r') {
			/* strip CR */
			buffer[idx]= '\0';
		}
		if (buffer[idx] == '\n') {
			buffer[idx] = '\0';
			idx++;
			*wifi_pass = &buffer[idx];
			break;
		}
		idx++;
	}

	/* strip CR/NL */
	while (buffer[idx]) {
		if (buffer[idx] == '\r') {
			buffer[idx]= '\0';
		}
		if (buffer[idx] == '\n') {
			buffer[idx] = '\0';
		}
		idx++;
	}

	return 0;
}

static long picowifi_init(struct ueth_data	*dev)
{

	long len;
	char link_detected;
	int timeout = 0;
	char *wifi_ssid, *wifi_pass;
#define TIMEOUT_RESOLUTION 50	/* ms */

	if (picowifi_load_wificred(&wifi_ssid, &wifi_pass)) {
		ALERT(("Trying if credentials are stored on PicoWifi.\r\n"));
	} else {
		DEBUGOUT(("Wifi credentials: '%s', '%s'\r\n", wifi_ssid, wifi_pass?wifi_pass:"(no password)"));

		/* Set SSID */
		len = usb_control_msg(
		dev->pusb_dev,
		usb_sndctrlpipe(dev->pusb_dev, 0),
		VENDOR_REQUEST_WIFI,
		USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
		0,
		WIFI_SET_SSID,
		wifi_ssid,
		strlen(wifi_ssid),
		USB_CTRL_SET_TIMEOUT);

		if (wifi_pass && (strlen(wifi_pass) > 0)) {
			/* Set password */
			len = usb_control_msg(
			dev->pusb_dev,
			usb_sndctrlpipe(dev->pusb_dev, 0),
			VENDOR_REQUEST_WIFI,
			USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			0,
			WIFI_SET_PASSWD,
			wifi_pass,
			strlen(wifi_pass),
			USB_CTRL_SET_TIMEOUT);

			/* Connect */
			len = usb_control_msg(
			dev->pusb_dev,
			usb_sndctrlpipe(dev->pusb_dev, 0),
			VENDOR_REQUEST_WIFI,
			USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			0x406, /* WPA2/WPA */
			WIFI_CONNECT,
			NULL,
			0,
			USB_CTRL_SET_TIMEOUT);
		} else {
			/* Connect */
			len = usb_control_msg(
			dev->pusb_dev,
			usb_sndctrlpipe(dev->pusb_dev, 0),
			VENDOR_REQUEST_WIFI,
			USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			0, /* open */
			WIFI_CONNECT,
			NULL,
			0,
			USB_CTRL_SET_TIMEOUT);
		}
	}

	/* Wait for link */
	do {
		link_detected = 0;

		len = usb_control_msg(
		dev->pusb_dev,
		usb_rcvctrlpipe(dev->pusb_dev, 0),
		VENDOR_REQUEST_WIFI,
		USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
		0,
		WIFI_STATUS,
		&link_detected,
		sizeof(link_detected),
		USB_CTRL_GET_TIMEOUT);

		if (!link_detected) {
			if (timeout == 0) {
				ALERT(("Waiting for Wifi connection... "));
			}
			mdelay(TIMEOUT_RESOLUTION);
			timeout += TIMEOUT_RESOLUTION;
		}
	} while (!link_detected && timeout < WIFI_CONNECT_TIMEOUT);
	if (link_detected) {
		if (timeout != 0) {
			ALERT(("done.\r\n"));
			mdelay(500);
		}
	} else {
		ALERT(("unable to connect.\r\n"));
		return -1;
	}

	(void)len;
	return 0;
}

long picowifi_send(struct ueth_data *dev, void *packet, long length)
{
	static pkt_s outpkt;
	long size;
	long actual_len;
	long err;

	DEBUGOUT(("** %s(), len %ld\r\n", __func__, length));
	if (dev->pusb_dev == 0) {
		return 0;
	}

	outpkt.magic = cpu2le32(MAGIC);
	outpkt.len   = cpu2le32(length);

	memcpy(outpkt.payload, packet, length);

	size = length + offsetof(pkt_s, payload);

	err = usb_bulk_msg(dev->pusb_dev,
				usb_sndbulkpipe(dev->pusb_dev, (long)dev->ep_out),
				(void *)&outpkt,
				size,
				&actual_len,
				USB_BULK_SEND_TIMEOUT, 0);

	if (err != 0)
		return -1;
	else
		return 0;
}

#define FIFO_SIZE (2*4096) // twice the device FIFO
static struct {
	u8  buffer[FIFO_SIZE];
	int level;
	int readidx;
	int writeidx;
} recv_fifo;

static void fifo_reset(void)
{
	recv_fifo.readidx = recv_fifo.writeidx = recv_fifo.level = 0;
}

static void fifo_enqueue(u8* data, int len)
{

	if (recv_fifo.writeidx + len <= FIFO_SIZE) {
		memcpy(&recv_fifo.buffer[recv_fifo.writeidx], data, len);
		recv_fifo.level += len;
		recv_fifo.writeidx += len;
	} else { // data goes past the break
		int part1 = FIFO_SIZE - recv_fifo.writeidx;
		int part2 = len - part1;
		memcpy(&recv_fifo.buffer[recv_fifo.writeidx], data, part1);
		memcpy(&recv_fifo.buffer[0], &data[part1], part2);
		recv_fifo.level += len;
		recv_fifo.writeidx += len - FIFO_SIZE;
	}

}

static void fifo_dequeue(u8* data, int len, int peek)
{
	if (recv_fifo.readidx + len <= FIFO_SIZE) {
		memcpy(data, &recv_fifo.buffer[recv_fifo.readidx], len);
		if (!peek) {
			recv_fifo.level -= len;
			recv_fifo.readidx += len;
		}
	} else { // data goes past the break
		int part1 = FIFO_SIZE - recv_fifo.readidx;
		int part2 = len - part1;
		memcpy(data, &recv_fifo.buffer[recv_fifo.readidx], part1);
		memcpy(&data[part1], &recv_fifo.buffer[0], part2);
		if (!peek) {
			recv_fifo.level -= len;
			recv_fifo.readidx += len - FIFO_SIZE;
		}
	}
}

long picowifi_recv(struct ueth_data *dev, unsigned char *dest_buf, unsigned long dest_len)
{
	static u8 recv_buffer[FIFO_SIZE/2]; // should match the *device* fifo
	static int resync_count = 0;
	pkt_hdr_s next_hdr;
	long actual_len = 0;
	long size = 0;
	long err;

	if (dev->pusb_dev == 0) {
		return -1L;
	}

	if (FIFO_SIZE - recv_fifo.level >= sizeof(recv_buffer)) { // try to get a new packet from USB
		err = usb_bulk_msg(dev->pusb_dev,
					usb_rcvbulkpipe(dev->pusb_dev, (long)dev->ep_in),
					(void *)recv_buffer,
					sizeof(recv_buffer),
					&actual_len,
					USB_BULK_RECV_TIMEOUT,
					USB_BULK_FLAG_EARLY_TIMEOUT);
		if ((err == 0) && (actual_len > 0)) { // got new data
			DEBUGOUT(("actual_len = %ld\n", actual_len));
			fifo_enqueue(recv_buffer, actual_len);
		}
	}

	if (recv_fifo.level > offsetof(pkt_s, payload)) {
		// check if we have a full packet in the FIFO

		fifo_dequeue((u8*)&next_hdr, sizeof(next_hdr), 1);

		if (next_hdr.magic != le2cpu32(MAGIC)) {
			DEBUGOUT(("invalid magic(%d) = %lx\n", resync_count, next_hdr.magic));
			if (resync_count == 3) {
				fifo_reset();
				resync_count = 0;
			} else {
				fifo_dequeue((u8*)&next_hdr, 4, 0); // pop 4 bytes and try to resync
				resync_count++;
			}
			return 0;
		}
		resync_count = 0;
		next_hdr.len = le2cpu32(next_hdr.len);

		if (recv_fifo.level >= offsetof(pkt_s, payload) + next_hdr.len) {
			DEBUGOUT(("pkt_recv: len = %ld\n", next_hdr.len));

			size = (dest_len < next_hdr.len) ? dest_len : next_hdr.len; // TODO correctly dequeue packets larger than dst buffer
			fifo_dequeue((u8*)&next_hdr, sizeof(next_hdr), 0);
			fifo_dequeue(dest_buf, size, 0);
		}

	}

	return size;
}

/*
 * picowifi probing functions
 */
void picowifi_eth_before_probe(void *a)
{
	api = a;
}

#define USB_VID 0x20A0u
#define USB_PID 0x42ECu

/* Probe to see if a new device is actually an picowifi device */
long
picowifi_eth_probe(struct usb_device *dev, unsigned int ifnum, struct ueth_data *ss)
{
	struct usb_interface *iface;
	struct usb_interface_descriptor *iface_desc;
	int ep_in_found = 0, ep_out_found = 0;
	int i;

	/* let's examine the device now */
	iface = &dev->config.if_desc[ifnum];
	iface_desc = &dev->config.if_desc[ifnum].desc;

	if (dev->descriptor.idVendor != USB_VID ||
	    dev->descriptor.idProduct != USB_PID)
		/* Found a supported dongle */
		return 0;

	memset(ss, 0, sizeof(struct ueth_data));

	/* At this point, we know we've got a live one */
	DEBUGOUT(("\r\n\r\nUSB Ethernet device detected: %#04x:%#04x\r\n",
	      dev->descriptor.idVendor, dev->descriptor.idProduct));

	/* Initialize the ueth_data structure with some useful info */
	ss->ifnum = ifnum;
	ss->pusb_dev = dev;
	ss->subclass = iface_desc->bInterfaceSubClass;
	ss->protocol = iface_desc->bInterfaceProtocol;
	ss->dev_priv = NULL;

	/*
	 * We are expecting a minimum of 2 endpoints - in, out (bulk)
	 * We will ignore any others.
	 */
	for (i = 0; i < iface_desc->bNumEndpoints; i++) {
		/* is it an BULK endpoint? */
		if ((iface->ep_desc[i].bmAttributes &
		     USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK) {
			u8 ep_addr = iface->ep_desc[i].bEndpointAddress;
			if (ep_addr & USB_DIR_IN) {
				if (!ep_in_found) {
					ss->ep_in = ep_addr &
						USB_ENDPOINT_NUMBER_MASK;
					ep_in_found = 1;
				}
			} else {
				if (!ep_out_found) {
					ss->ep_out = ep_addr &
						USB_ENDPOINT_NUMBER_MASK;
					ep_out_found = 1;
				}
			}
		}
	}

	/* Do some basic sanity checks, and bail if we find a problem */
	if (usb_set_interface(dev, iface_desc->bInterfaceNumber, 0) ||
	    !ss->ep_in || !ss->ep_out) {
		DEBUGOUT(("Problems with device\r\n"));
		return 0;
	}
	dev->privptr = (void *)ss;
	return 1;
}

static int hexchar(unsigned char c)
{
	int v = 0;

	if ((c >= '0') && (c <= '9')) {
		v = c - '0';
	} else if ((c >= 'A') && (c <= 'F')) {
		v = c - 'A' + 0xa;
	} else if ((c >= 'a') && (c <= 'f')) {
		v = c - 'a' + 0xa;
	}

	return v;
}

int picowifi_read_mac(struct ueth_data *dev, unsigned char *mac_address)
{
	int k;
	// parse MAC encoded in device serial number
	for (k=0; k < ETH_ALEN; k++) {
		mac_address[k] = hexchar(dev->pusb_dev->serial[2*k]) << 4 | hexchar(dev->pusb_dev->serial[2*k+1]);
	}
	DEBUGOUT(("MAC: %02x%02x%02x%02x%02x%02x\n", mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5]));
	return 0;
}

long
picowifi_eth_get_info(struct usb_device *dev, struct ueth_data *ss, unsigned char* mac)
{
	(void) dev;

	DEBUGOUT(("picowifi_get_info: begin\r\n"));

	DEBUGOUT(("picowifi_get_info: before read_mac\r\n"));
	/* Get the MAC address */
	if (picowifi_read_mac(ss, mac))
		return 0;

	DEBUGOUT(("picowifi_get_info: before picowifi_init\r\n"));
	picowifi_init(ss);

	DEBUGOUT(("picowifi_get_info: done\r\n"));

	return 1;
}

#ifdef USB_PRINTSTATUS

#define CYW43_LINK_DOWN         (0)     ///< link is down
#define CYW43_LINK_JOIN         (1)     ///< Connected to wifi
#define CYW43_LINK_FAIL         (-1)    ///< Connection failed
#define CYW43_LINK_NONET        (-2)    ///< No matching SSID found (could be out of range, or down)
#define CYW43_LINK_BADAUTH      (-3)    ///< Authenticatation failure

#define FW_VERSION_STRING (4)

int
picowifi_printstatus(struct ueth_data *dev)
{
	long len;
	char fw_version[64] = {0};

	struct {
		long link_up;
		long cyw43_status;
		long last_rssi;
		long last_rate;
	} status = {0};

	if (dev->pusb_dev == 0) {
		return -1L;
	}

	/* Read firmware version */
	len = usb_string(dev->pusb_dev, FW_VERSION_STRING, fw_version, sizeof(fw_version));
	if (len>=0)
	{
		printf("Version: %s\r\n", fw_version);
	}

	/* Read status information */
	len = usb_control_msg(
	dev->pusb_dev,
	usb_rcvctrlpipe(dev->pusb_dev, 0),
	VENDOR_REQUEST_WIFI,
	USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
	0,
	WIFI_STATUS,
	&status,
	sizeof(status),
	USB_CTRL_GET_TIMEOUT);

	printf("Link: %s\r\nWifi status: ", le2cpu32(status.link_up)?"Up":"Down");
	switch (le2cpu32(status.cyw43_status))
	{
	case CYW43_LINK_DOWN:
		printf("Not connected\r\n");
		break;
	case CYW43_LINK_JOIN:
		printf("Connected\r\n");
		break;
	case CYW43_LINK_FAIL:
		printf("Connection failed\r\n");
		break;
	case CYW43_LINK_NONET:
		printf("SSID not found\r\n");
		break;
	case CYW43_LINK_BADAUTH:
		printf("Authenticatation failed\r\n");
		break;
	default:
		printf("Unknown (%ld)\r\n", le2cpu32(status.cyw43_status));
		break;
	}

	/* Not all FW versions return RSSI */
	if (len > offsetof(typeof(status), last_rssi))
	{
		printf("RSSI: %ld dB\r\n", le2cpu32(status.last_rssi));
	}

	return 0;
}
#endif