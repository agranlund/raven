#ifndef __ETH_DRIVER_H__
#define __ETH_DRIVER_H__

#ifdef USB_DRIVER
	#include "usb/usbeth_dev.h"
	#define driver_init USBETHdev_init
	#define driver_poll USBETHdev_poll
	#define driver_send USBETHdev_send
	#define driver_deinit USBETHdev_done

#elif defined(SLIP_DRIVER)
	#include "slip/slip_dev.h"
	#define driver_init slipdev_init
	#define driver_poll slipdev_poll
	#define driver_send slipdev_send
	#define driver_deinit slipdev_done

#elif defined(RTL8019ISA_DRIVER)
	#include "rtl8019_isa/rtl8019.h"
	#define driver_init RTL8019dev_init
	#define driver_poll RTL8019dev_poll
	#define driver_send RTL8019dev_send
	#define driver_deinit RTL8019dev_done

#else
	#include "netusbee/rtl8019.h"
	#define driver_init RTL8019dev_init
	#define driver_poll RTL8019dev_poll
	#define driver_send RTL8019dev_send
	#define driver_deinit RTL8019dev_done
#endif

#endif // __ETH_DRIVER_H__
