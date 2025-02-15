#ifndef __USBETH_DEV_H__
#define __USBETH_DEV_H__

#include "uip.h"
#include <stdbool.h>

/*****************************************************************************
*  USBETHdev_init()
*  Description: Enumeration and initialization of the USB Ethernet adapter
*****************************************************************************/
bool USBETHdev_init(uint8_t* macaddr, uint32_t cpu_type);

/*****************************************************************************
*  USBETHdev_done()
*  Description: Unregister Ethernet adapter from USB stack
*****************************************************************************/
void USBETHdev_done(void);

/*****************************************************************************
*  USBETHdev_send()
*  Description: Sends the packet contained in uip_buf and uip_appdata over
*                 the network
*****************************************************************************/
void USBETHdev_send(void);


/*****************************************************************************
*  unsigned int USBETHdev_poll()
*  Returns:     Length of the packet retreived, or zero if no packet retreived
*  Description: Polls the USB Ethernet looking for a new
*                 packet in the receive buffer.  If a new packet exists and
*                 will fit in uip_buf, it is retreived, and the length is
*                 returned.  A packet bigger than the buffer is discarded
*****************************************************************************/
unsigned int USBETHdev_poll(void);

/*****************************************************************************
*  USBETHdev_printstatus(void)
*  Description: Prints status information about the connected USB device.
*****************************************************************************/
unsigned int USBETHdev_printstatus(void);

#endif /* __USBETH_DEV_H__ */