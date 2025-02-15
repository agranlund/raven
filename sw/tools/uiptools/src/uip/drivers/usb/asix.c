/*
 * Modified for the FreeMiNT USB subsystem by Alan Hourihane 2014.
 * Modified to be used in uiptool by Christian Zietz 2018 
 *          and using asix_recv() code by Roger Burrows.
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

#include "asix.h"
#include "delay.h"

#include "usb.h"
#include "usb_api.h"
#include "usb_ether.h"
#include "mii.h"

static struct usb_module_api   *api = NULL;
static struct proc *asix_p;
static long usbEthID = -1;

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

/* ASIX AX8817X based USB 2.0 Ethernet Devices */

#define AX_CMD_SET_SW_MII		0x06
#define AX_CMD_READ_MII_REG		0x07
#define AX_CMD_WRITE_MII_REG		0x08
#define AX_CMD_SET_HW_MII		0x0a
#define AX_CMD_READ_EEPROM		0x0b
#define AX_CMD_READ_RX_CTL		0x0f
#define AX_CMD_WRITE_RX_CTL		0x10
#define AX_CMD_WRITE_IPG0		0x12
#define AX_CMD_READ_NODE_ID		0x13
#define AX_CMD_WRITE_NODE_ID		0x14
#define AX88172_CMD_READ_NODE_ID	0x17
#define AX_CMD_READ_PHY_ID		0x19
#define AX_CMD_WRITE_MEDIUM_MODE	0x1b
#define AX_CMD_WRITE_GPIOS		0x1f
#define AX_CMD_SW_RESET			0x20
#define AX_CMD_SW_PHY_SELECT		0x22

#define AX_SWRESET_CLEAR		0x00
#define AX_SWRESET_PRTE			0x04
#define AX_SWRESET_PRL			0x08
#define AX_SWRESET_IPRL			0x20
#define AX_SWRESET_IPPD			0x40

#define AX88772_IPG0_DEFAULT		0x15
#define AX88772_IPG1_DEFAULT		0x0c
#define AX88772_IPG2_DEFAULT		0x12

/* AX88772 & AX88178 Medium Mode Register */
#define AX_MEDIUM_PF		0x0080
#define AX_MEDIUM_JFE		0x0040
#define AX_MEDIUM_TFC		0x0020
#define AX_MEDIUM_RFC		0x0010
#define AX_MEDIUM_ENCK		0x0008
#define AX_MEDIUM_AC		0x0004
#define AX_MEDIUM_FD		0x0002
#define AX_MEDIUM_GM		0x0001
#define AX_MEDIUM_SM		0x1000
#define AX_MEDIUM_SBP		0x0800
#define AX_MEDIUM_PS		0x0200
#define AX_MEDIUM_RE		0x0100

#define AX88178_MEDIUM_DEFAULT	\
	(AX_MEDIUM_PS | AX_MEDIUM_FD | AX_MEDIUM_AC | \
	 AX_MEDIUM_RFC | AX_MEDIUM_TFC | AX_MEDIUM_JFE | \
	 AX_MEDIUM_RE)

#define AX88772_MEDIUM_DEFAULT	\
	(AX_MEDIUM_FD | AX_MEDIUM_RFC | \
	 AX_MEDIUM_TFC | AX_MEDIUM_PS | \
	 AX_MEDIUM_AC | AX_MEDIUM_RE)
	
#define AX88172_MEDIUM_DEFAULT \
	(AX_MEDIUM_FD | AX_MEDIUM_RFC | \
	 AX_MEDIUM_AC)

/* AX88772 & AX88178 RX_CTL values */
#define AX_RX_CTL_SO			0x0080
#define AX_RX_CTL_AB			0x0008

#define AX_DEFAULT_RX_CTL	\
	(AX_RX_CTL_SO | AX_RX_CTL_AB)

/* GPIO 2 toggles */
#define AX_GPIO_GPO2EN		0x10	/* GPIO2 Output enable */
#define AX_GPIO_GPO_2		0x20	/* GPIO2 Output value */
#define AX_GPIO_RSE		0x80	/* Reload serial EEPROM */

/* local defines */
#define ASIX_BASE_NAME "asx"
#define USB_CTRL_SET_TIMEOUT 5000
#define USB_CTRL_GET_TIMEOUT 5000
#define USB_BULK_SEND_TIMEOUT 5000
#define USB_BULK_RECV_TIMEOUT 5000

#define AX_RX_URB_SIZE 2048
#define PHY_CONNECT_TIMEOUT 5000

/* asix_flags defines */
#define FLAG_NONE			0
#define FLAG_TYPE_AX88172	(1U << 0)
#define FLAG_TYPE_AX88772	(1U << 1)
#define FLAG_TYPE_AX88772B	(1U << 2)
#define FLAG_EEPROM_MAC		(1U << 3) /* initial mac address in eeprom */

/* local vars */

/* driver private */
struct asix_private {
	long flags;
};

/*
 * Asix infrastructure commands
 */
static int asix_write_cmd(struct ueth_data *dev, u8 cmd, u16 value, u16 index,
			     u16 size, void *data) __attribute__((optimize("O0")));
static int asix_write_cmd(struct ueth_data *dev, u8 cmd, u16 value, u16 index,
			     u16 size, void *data)
{
	long len;

	DEBUGOUT(("asix_write_cmd() cmd=0x%02x value=0x%04x index=0x%04x "
		"size=%d\r\n", cmd, value, index, size));

	len = usb_control_msg(
		dev->pusb_dev,
		usb_sndctrlpipe(dev->pusb_dev, 0),
		cmd,
		USB_DIR_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
		value,
		index,
		data,
		size,
		USB_CTRL_SET_TIMEOUT);

	DEBUGOUT(("asix_write_cmd(): ret=%ld, status=%lx, act_len=%ld\r\n", len, dev->pusb_dev->status, dev->pusb_dev->act_len));
	return len == size ? 0 : -1;
}

static int asix_read_cmd(struct ueth_data *dev, u8 cmd, u16 value, u16 index,
			    u16 size, void *data)
{
	long len;

	DEBUGOUT(("asix_read_cmd() cmd=0x%02x value=0x%04x index=0x%04x size=%d\r\n",
		cmd, value, index, size));

	len = usb_control_msg(
		dev->pusb_dev,
		usb_rcvctrlpipe(dev->pusb_dev, 0),
		cmd,
		USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
		value,
		index,
		data,
		size,
		USB_CTRL_GET_TIMEOUT);

	DEBUGOUT(("asix_read_cmd(): ret=%ld\r\n", len));
	return len == size ? 0 : -1;
}

static inline int asix_set_sw_mii(struct ueth_data *dev)
{
	int ret;

	ret = asix_write_cmd(dev, AX_CMD_SET_SW_MII, 0x0000, 0, 0, NULL);
	if (ret < 0)
		DEBUGOUT(("Failed to enable software MII access\r\n"));
	return ret;
}

static inline int asix_set_hw_mii(struct ueth_data *dev)
{
	int ret;

	ret = asix_write_cmd(dev, AX_CMD_SET_HW_MII, 0x0000, 0, 0, NULL);
	if (ret < 0)
		DEBUGOUT(("Failed to enable hardware MII access\r\n"));
	return ret;
}

static int asix_mdio_read(struct ueth_data *dev, int phy_id, int loc)
{
	ALLOC_CACHE_ALIGN_BUFFER(__u16, res, 1);

	asix_set_sw_mii(dev);
	asix_read_cmd(dev, AX_CMD_READ_MII_REG, phy_id, (__u16)loc, 2, res);
	asix_set_hw_mii(dev);

	DEBUGOUT(("asix_mdio_read() phy_id=0x%02x, loc=0x%02x, returns=0x%04x\r\n",
			phy_id, loc, le2cpu16(*res)));

	return le2cpu16(*res);
}

static void
asix_mdio_write(struct ueth_data *dev, int phy_id, int loc, int val)
{
	ALLOC_CACHE_ALIGN_BUFFER(__u16, res, 1);
	*res = cpu2le16(val);

	DEBUGOUT(("asix_mdio_write() phy_id=0x%02x, loc=0x%02x, val=0x%04x\r\n",
			phy_id, loc, val));
	asix_set_sw_mii(dev);
	asix_write_cmd(dev, AX_CMD_WRITE_MII_REG, phy_id, (__u16)loc, 2, res);
	asix_set_hw_mii(dev);
}

/*
 * Asix "high level" commands
 */
static int asix_sw_reset(struct ueth_data *dev, u8 flags)
{
	int ret;

	ret = asix_write_cmd(dev, AX_CMD_SW_RESET, flags, 0, 0, NULL);
	if (ret < 0)
		DEBUGOUT(("Failed to send software reset: %02x\r\n", ret));
	else
		mdelay(150);

	return ret;
}

static inline int asix_get_phy_addr(struct ueth_data *dev)
{
	ALLOC_CACHE_ALIGN_BUFFER(u8, buf, 2);

	int ret = asix_read_cmd(dev, AX_CMD_READ_PHY_ID, 0, 0, 2, buf);

	DEBUGOUT(("asix_get_phy_addr()\r\n"));

	if (ret < 0) {
		DEBUGOUT(("Error reading PHYID register: %02x\r\n", ret));
		goto out;
	}
	DEBUGOUT(("asix_get_phy_addr() returning 0x%02x%02x\r\n", buf[0], buf[1]));
	ret = buf[1];

out:
	return ret;
}

static int asix_write_medium_mode(struct ueth_data *dev, u16 mode)
{
	int ret;

	DEBUGOUT(("asix_write_medium_mode() - mode = 0x%04x\r\n", mode));
	ret = asix_write_cmd(dev, AX_CMD_WRITE_MEDIUM_MODE, mode,
			0, 0, NULL);
	if (ret < 0) {
		DEBUGOUT(("Failed to write Medium Mode mode to 0x%04x: %02x\r\n",
			mode, ret));
	}
	return ret;
}

static u16 asix_read_rx_ctl(struct ueth_data *dev)
{
	ALLOC_CACHE_ALIGN_BUFFER(__u16, v, 1);

	int ret = asix_read_cmd(dev, AX_CMD_READ_RX_CTL, 0, 0, 2, v);

	if (ret < 0)
		DEBUGOUT(("Error reading RX_CTL register: %02x\r\n", ret));
	else
		ret = le2cpu16(*v);
	return ret;
}

static int asix_write_rx_ctl(struct ueth_data *dev, u16 mode)
{
	int ret;

	DEBUGOUT(("asix_write_rx_ctl() - mode = 0x%04x\r\n", mode));
	ret = asix_write_cmd(dev, AX_CMD_WRITE_RX_CTL, mode, 0, 0, NULL);
	if (ret < 0) {
		DEBUGOUT(("Failed to write RX_CTL mode to 0x%04x: %02x\r\n",
				mode, ret));
	}
	return ret;
}

static int asix_write_gpio(struct ueth_data *dev, u16 value, int sleepy)
{
	int ret;

	DEBUGOUT(("asix_write_gpio() - value = 0x%04x\r\n", value));
	ret = asix_write_cmd(dev, AX_CMD_WRITE_GPIOS, value, 0, 0, NULL);
	if (ret < 0) {
		DEBUGOUT(("Failed to write GPIO value 0x%04x: %02x\r\n",
			value, ret));
	}
	if (sleepy)
		mdelay(sleepy);

	return ret;
}

/*
 * mii commands
 */

/*
 * mii_nway_restart - restart NWay (autonegotiation) for this interface
 *
 * Returns 0 on success, negative on error.
 */
static int mii_nway_restart(struct ueth_data *dev)
{
	int bmcr;
	int r = -1;

	/* if autoneg is off, it's an error */
	bmcr = asix_mdio_read(dev, dev->phy_id, MII_BMCR);

	if (bmcr & BMCR_ANENABLE) {
		bmcr |= BMCR_ANRESTART;
		asix_mdio_write(dev, dev->phy_id, MII_BMCR, bmcr);
		r = 0;
	}

	return r;
}

static int asix_read_mac(struct ueth_data *dev, unsigned char *mac_address)
{
	struct asix_private *priv = (struct asix_private *)dev->dev_priv;
	int i;
	ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buf, ETH_ALEN);

	if (priv->flags & FLAG_EEPROM_MAC) {
		for (i = 0; i < (ETH_ALEN >> 1); i++) {
			if (asix_read_cmd(dev, AX_CMD_READ_EEPROM,
					  0x04 + i, 0, 2, buf) < 0) {
				DEBUGOUT(("Failed to read SROM address 04h.\r\n"));
				return -1;
			}
			memcpy((mac_address + i * 2), buf, 2);
		}
	} else if (priv->flags & FLAG_TYPE_AX88172) {
		if (asix_read_cmd(dev, AX88172_CMD_READ_NODE_ID, 0, 0, ETH_ALEN, buf)
		     < 0) {
			DEBUGOUT(("Failed to read MAC address.\r\n"));
			return -1;
		}
		memcpy(mac_address, buf, ETH_ALEN);
	} else {
		if (asix_read_cmd(dev, AX_CMD_READ_NODE_ID, 0, 0, ETH_ALEN, buf)
		     < 0) {
			DEBUGOUT(("Failed to read MAC address.\r\n"));
			return -1;
		}
		memcpy(mac_address, buf, ETH_ALEN);
	}

	return 0;
}

static int asix_basic_reset_172(struct ueth_data *dev)
{
	if (asix_write_gpio(dev,
			AX_GPIO_RSE | AX_GPIO_GPO_2 | AX_GPIO_GPO2EN, 5) < 0)
		return -1;

	if (asix_write_rx_ctl(dev, 0x0000) < 0) {
        	DEBUGOUT(("asix_read_rx_ctl failed\r\n"));
		return -1;
	}

	dev->phy_id = asix_get_phy_addr(dev);
	if (dev->phy_id < 0)
		DEBUGOUT(("Failed to read phy id\r\n"));

	asix_mdio_write(dev, dev->phy_id, MII_BMCR, BMCR_RESET);
	asix_mdio_write(dev, dev->phy_id, MII_ADVERTISE,
			ADVERTISE_ALL | ADVERTISE_CSMA | ADVERTISE_PAUSE_CAP);
	mii_nway_restart(dev);

	if (asix_write_medium_mode(dev, AX88172_MEDIUM_DEFAULT) < 0)
		return -1;

	return 0;
}

static int asix_basic_reset_772(struct ueth_data *dev)
{
	int embd_phy;
	u16 rx_ctl;

	(void) rx_ctl; /* because of DEBUGOUT */

	if (asix_write_gpio(dev,
			AX_GPIO_RSE | AX_GPIO_GPO_2 | AX_GPIO_GPO2EN, 5) < 0)
		return -1;

	/* 0x10 is the phy id of the embedded 10/100 ethernet phy */
	embd_phy = ((asix_get_phy_addr(dev) & 0x1f) == 0x10 ? 1 : 0);
	if (asix_write_cmd(dev, AX_CMD_SW_PHY_SELECT,
				embd_phy, 0, 0, NULL) < 0) {
        	DEBUGOUT(("Select PHY #1 failed"));
		return -1;
	}

	if (asix_sw_reset(dev, AX_SWRESET_IPPD | AX_SWRESET_PRL) < 0) {
        	DEBUGOUT(("asix_sw_reset failed"));
		return -1;
	}

	if (asix_sw_reset(dev, AX_SWRESET_CLEAR) < 0) {
        	DEBUGOUT(("asix_sw_reset clear failed"));
		return -1;
	}

	if (embd_phy) {
		if (asix_sw_reset(dev, AX_SWRESET_IPRL) < 0) {
			return -1;
		}
	} else {
		if (asix_sw_reset(dev, AX_SWRESET_PRTE) < 0) {
			return -1;
		}
	}

	rx_ctl = asix_read_rx_ctl(dev);
	DEBUGOUT(("RX_CTL is 0x%04x after software reset\r\n", rx_ctl));
	if (asix_write_rx_ctl(dev, 0x0000) < 0) {
		return -1;
	}

	rx_ctl = asix_read_rx_ctl(dev);
	DEBUGOUT(("RX_CTL is 0x%04x setting to 0x0000\r\n", rx_ctl));

	dev->phy_id = asix_get_phy_addr(dev);
	if (dev->phy_id < 0)
		DEBUGOUT(("Failed to read phy id\r\n"));

	asix_mdio_write(dev, dev->phy_id, MII_BMCR, BMCR_RESET);
	asix_mdio_write(dev, dev->phy_id, MII_ADVERTISE,
			ADVERTISE_ALL | ADVERTISE_CSMA);
	mii_nway_restart(dev);

	if (asix_write_medium_mode(dev, AX88772_MEDIUM_DEFAULT) < 0)
		return -1;

	if (asix_write_cmd(dev, AX_CMD_WRITE_IPG0,
				AX88772_IPG0_DEFAULT | AX88772_IPG1_DEFAULT,
				AX88772_IPG2_DEFAULT, 0, NULL) < 0) {
		DEBUGOUT(("Write IPG,IPG1,IPG2 failed\r\n"));
		return -1;
	}

	return 0;
}

/*
 * Asix callbacks
 */
static long asix_init(struct ueth_data	*dev)
{
	int timeout = 0;
#define TIMEOUT_RESOLUTION 50	/* ms */
	int link_detected;

	DEBUGOUT(("** %s()\r\n", __func__));

	if (asix_write_rx_ctl(dev, AX_DEFAULT_RX_CTL) < 0)
		goto out_err;

	do {
		link_detected = asix_mdio_read(dev, dev->phy_id, MII_BMSR) &
			BMSR_LSTATUS;
		if (!link_detected) {
			if (timeout == 0) {
				ALERT(("Waiting for Ethernet connection... "));
			}
			mdelay(TIMEOUT_RESOLUTION);
			timeout += TIMEOUT_RESOLUTION;
		}
	} while (!link_detected && timeout < PHY_CONNECT_TIMEOUT);
	if (link_detected) {
		if (timeout != 0) {
			ALERT(("done.\r\n"));
			mdelay(500);
		}
	} else {
		ALERT(("unable to connect.\r\n"));
		goto out_err;
	}

	return 0;
out_err:
	return -1;
}

long asix_send(struct ueth_data *dev, void *packet, long length)
{
	long err = 0;
	u32 packet_len;
	long actual_len = 0;
	long size;
	unsigned char *msg;

	DEBUGOUT(("** %s(), len %ld\r\n", __func__, length));
	if (dev->pusb_dev == 0) {
		return 0;
	}

	packet_len = ((length ^ 0x0000ffff) << 16) + length;
	packet_len = cpu2le32(packet_len);

	msg = (unsigned char *)malloc(length + sizeof(packet_len) + 3 /* pad */);
	if (!msg) {
		DEBUGOUT(("Out of memory"));
		return -1;
	}

	memcpy(msg, &packet_len, sizeof(packet_len));
	memcpy(msg + sizeof(packet_len), (void *)packet, length);
	if (length & 1)
		length++;

	size = length + sizeof(packet_len);

	err = usb_bulk_msg(dev->pusb_dev,
				usb_sndbulkpipe(dev->pusb_dev, (long)dev->ep_out),
				(void *)msg,
				size,
				&actual_len,
				USB_BULK_SEND_TIMEOUT, 0);
	free(msg);
	//Delay_microsec(500uL);
	if (err != 0)
		return -1;
	else 
		return 0;
}

/*
 * asix_recv(): receive one ethernet packet into the destination buffer
 *
 * Background info for understanding the code:
 * . The Asix chip collects ethernet packets into a stream of bytes, each
 *   packet preceded by a 4-byte packet header which contains the packet
 *   length in big-endian & little-endian formats.
 * . Each call to usb_bulk_msg() receives the next sequence of bytes in
 *   the stream, up to a max of AX_RX_URB_SIZE bytes.  The actual number
 *   of bytes received is returned.  If this number is zero, then the chip
 *   has no more data in its buffers at the moment.
 *
 * The code implicitly assumes that the maximum size of an Ethernet packet
 * is less than or equal to AX_RX_URB_SIZE.
 *
 * Return code:
 * . A return code of 0 or more is the length of the packet returned.
 *   To obtain all the buffered data, call until the return code is zero.
 * . A negative return code indicates an error (see the code for the
 *   meaning of specific negative values)
 */
#define TRUE            (1)
#define FALSE           (0)
#define RECV_BUFSIZE    (2*AX_RX_URB_SIZE)      /* code only handles 2 buffers */
static unsigned char recv_buf[RECV_BUFSIZE];

long asix_recv(struct ueth_data *dev, unsigned char *dest_buf, unsigned long dest_len)
{
    static unsigned char *fill_ptr = recv_buf;
    static unsigned char *empty_ptr = recv_buf;
    static unsigned char *end_buf = recv_buf + RECV_BUFSIZE;
    static long bytes_remaining = 0L;
    static int end_of_stream = FALSE;
    char *p;
    long actual_len;
    u32 packet_len;
    long err = 0L;
    int i, do_copy, wrap;

    if (dev->pusb_dev == 0) {
        return -1L;
    }

    if (!end_of_stream && (bytes_remaining <= AX_RX_URB_SIZE)) {
        /* more bytes available, and room for more bytes in our buffer */
        err = usb_bulk_msg(dev->pusb_dev,
                    usb_rcvbulkpipe(dev->pusb_dev, (long)dev->ep_in),
                    (void *)fill_ptr,
                    AX_RX_URB_SIZE,
                    &actual_len,
                    USB_BULK_RECV_TIMEOUT,
                    USB_BULK_FLAG_EARLY_TIMEOUT);

        /* switch to other buffer for next receive */
        if (fill_ptr == recv_buf)
            fill_ptr = recv_buf + AX_RX_URB_SIZE;
        else
            fill_ptr = recv_buf;

        /*
         * if no data is available from USB, usb_bulk_msg() returns an
         * error of -1, so for that case, we just set 'actual_len' to zero
         */
        if (err == -1L) {
            err = 0L;
            actual_len = 0;
        }

        if (err < 0L) {
            DEBUGOUT(("Rx: usb_bulk_msg() returned %ld\r\n", err));
            err = -2L;
            goto out;
        }


        if (actual_len > AX_RX_URB_SIZE) {
            DEBUGOUT(("Rx: received too many bytes %ld\r\n", actual_len));
            err = -3L;
            goto out;
        }

        if (actual_len < AX_RX_URB_SIZE)
            end_of_stream = TRUE;

        bytes_remaining += actual_len;
    }

    /*
     * check for 'no packet available'
     */
    if (bytes_remaining <= 0L) {
        err = 0L;
        goto out;
    }

    /*
     * the next 4 bytes of the data stream contain the length of the
     * actual packet as two complementary 16-bit words.  First ensure
     * that we have enough data left for a packet header.
     */
    if (bytes_remaining < sizeof(packet_len)) {
        DEBUGOUT(("Rx: incomplete packet length header\r\n"));
        err = -4L;
        goto out;
    }

    /*
     * Extract the data, allowing the packet length header to wrap at
     * the end of the buffers
     */
    for (p = (char *)&packet_len, i = 0; i < sizeof(packet_len); i++) {
        *p++ = *empty_ptr++;
        if (empty_ptr >= end_buf)
            empty_ptr = recv_buf;
    }
    packet_len = le2cpu32(packet_len);

    if (((~packet_len >> 16) & 0x7ff) != (packet_len & 0x7ff)) {
        DEBUGOUT(("Rx: malformed packet length header: %#lx (%#lx:%#lx)\r\n",
                packet_len, (~packet_len >> 16) & 0x7ff, packet_len & 0x7ff));
        err = -5L;
        goto out;
    }

    packet_len = packet_len & 0x7ff;
    if (packet_len > bytes_remaining - sizeof(packet_len)) {
        DEBUGOUT(("Rx: length error: packet_len=%ld, bytes_remaining=%ld\r\n",
                packet_len, bytes_remaining));
        err = -6L;
        goto out;
    }

    /*
     * at last, the normal case, but we still need to check if the
     * destination buffer is large enough
     */
    do_copy = TRUE;
    if (packet_len > dest_len) {
        DEBUGOUT(("Rx: packet_len=%ld > dest_len=%ld\r\n", packet_len, dest_len));
        do_copy = FALSE;
    }

    wrap = empty_ptr + packet_len - end_buf;
    if (wrap > 0) {         /* packet wraps */
        if (do_copy) {
            memcpy(dest_buf, empty_ptr, packet_len-wrap);
            memcpy(dest_buf+packet_len-wrap, recv_buf, wrap);
        }
        empty_ptr = recv_buf + wrap;
    } else {
        if (do_copy)
            memcpy(dest_buf, empty_ptr, packet_len);
        empty_ptr += packet_len;
    }
    err = packet_len;       /* value to return */

    /* Adjust for next iteration. Packets are padded to 16-bits */
    if (packet_len & 1) {
        packet_len++;
        empty_ptr++;
    }
    bytes_remaining -= sizeof(packet_len) + packet_len;

    /*
     * if dest_len was too short, we return an error, dropping the packet
     * but continuing with the remaining data
     */
    if (!do_copy)
        return -7L;

    return err;     /* i.e. packet length */

    /*
     * error exit
     */
out:
    end_of_stream = FALSE;
    bytes_remaining = 0L;
    fill_ptr = empty_ptr = recv_buf;
    return err;
}

/*
 * Asix probing functions
 */
void asix_eth_before_probe(void *a)
{
	api = a;
}

struct asix_dongle {
	unsigned short vendor;
	unsigned short product;
	int flags;
};

static const struct asix_dongle const asix_dongles[] = {
	{ 0x05ac, 0x1402, FLAG_TYPE_AX88772 },	/* Apple USB Ethernet Adapter */
	{ 0x0b95, 0x772a, FLAG_TYPE_AX88772 },  /* Cables-to-Go USB Ethernet Adapter */
	{ 0x0b95, 0x7720, FLAG_TYPE_AX88772 },	/* Trendnet TU2-ET100 V3.0R */
	{ 0x0b95, 0x1720, FLAG_TYPE_AX88172 },	/* SMC 2209 - others ?? */
	{ 0x0db0, 0xa877, FLAG_TYPE_AX88772 },	/* MSI - ASIX 88772a */
	{ 0x13b1, 0x0018, FLAG_TYPE_AX88172 },	/* Linksys 200M v2.1 */
	{ 0x1557, 0x7720, FLAG_TYPE_AX88772 },	/* 0Q0 cable ethernet */
	{ 0x2001, 0x1a00, FLAG_TYPE_AX88172 },	/* D-Link DUB-E100 */
	{ 0x07d1, 0x3c05, FLAG_TYPE_AX88772 },	/* D-Link DUB-E100 H/W Ver B1 */
	{ 0x2001, 0x3c05, FLAG_TYPE_AX88772 },  /* D-Link DUB-E100 H/W Ver B1 Alternate */
	{ 0x2001, 0x1a02, FLAG_TYPE_AX88772 },	/* D-Link DUB-E100 H/W Ver C1 / D1 */
	/* ASIX 88772B */
	{ 0x0b95, 0x772b, FLAG_TYPE_AX88772B | FLAG_EEPROM_MAC },
	{ 0x0000, 0x0000, FLAG_NONE }	/* END - Do not remove */
};

/* Probe to see if a new device is actually an asix device */
long
asix_eth_probe(struct usb_device *dev, unsigned int ifnum, struct ueth_data *ss)
{
	struct usb_interface *iface;
	struct usb_interface_descriptor *iface_desc;
	int ep_in_found = 0, ep_out_found = 0;
	int i;

	/* let's examine the device now */
	iface = &dev->config.if_desc[ifnum];
	iface_desc = &dev->config.if_desc[ifnum].desc;

	for (i = 0; asix_dongles[i].vendor != 0; i++) {
		if (dev->descriptor.idVendor == asix_dongles[i].vendor &&
		    dev->descriptor.idProduct == asix_dongles[i].product)
			/* Found a supported dongle */
			break;
	}

	if (asix_dongles[i].vendor == 0)
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

	/* alloc driver private */
	ss->dev_priv = malloc(sizeof(struct asix_private));
	if (!ss->dev_priv)
		return 0;
	memset(ss->dev_priv, 0, sizeof(struct asix_private));

	((struct asix_private *)ss->dev_priv)->flags = asix_dongles[i].flags;

	/*
	 * We are expecting a minimum of 3 endpoints - in, out (bulk), and
	 * int. We will ignore any others.
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

		/* is it an interrupt endpoint? */
		if ((iface->ep_desc[i].bmAttributes &
		    USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT) {
			ss->ep_int = iface->ep_desc[i].bEndpointAddress &
				USB_ENDPOINT_NUMBER_MASK;
			ss->irqinterval = iface->ep_desc[i].bInterval;
		}
	}

	/* Do some basic sanity checks, and bail if we find a problem */
	if (usb_set_interface(dev, iface_desc->bInterfaceNumber, 0) ||
	    !ss->ep_in || !ss->ep_out || !ss->ep_int) {
		DEBUGOUT(("Problems with device\r\n"));
		free(ss->dev_priv);
		return 0;
	}
	dev->privptr = (void *)ss;
	return 1;
}


long 
asix_eth_get_info(struct usb_device *dev, struct ueth_data *ss, unsigned char* mac)
{
	struct asix_private *priv = (struct asix_private *)ss->dev_priv;

	(void) dev;
	
	DEBUGOUT(("asix_get_info: begin\r\n"));

	
	DEBUGOUT(("asix_get_info: before reset\r\n"));

	if (priv->flags & FLAG_TYPE_AX88172) {
		if (asix_basic_reset_172(ss))
			return 0;
	} else {
		if (asix_basic_reset_772(ss))
			return 0;
	}

	DEBUGOUT(("asix_get_info: before read_mac\r\n"));
	/* Get the MAC address */
	if (asix_read_mac(ss, mac))
		return 0;

	DEBUGOUT(("asix_get_info: before asix_init\r\n"));
	asix_init(ss);
	
	DEBUGOUT(("asix_get_info: done\r\n"));

	return 1;
}

