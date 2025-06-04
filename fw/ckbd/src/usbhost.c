//-------------------------------------------------------------------------
// usbhost.c
// core usb functionality
//-------------------------------------------------------------------------
#define NODEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "usbhost.h"
#include "usbdescr.h"
#include "usbreport.h"
#include "usbdata.h"
#include "settings.h"
#include "keyboardled.h"

#if defined(DEBUG)
void DumpHex(PUINT8 buffa, UINT16 len)
{
	for (UINT16 cnt = 0; cnt < len; cnt++) {
		dbg_printf("$%x ", buffa[cnt]);
		if ((cnt & 0x000F) == 0x000F) {
			dbg_printf("\n");
        }
	}
    if (len & 0x000F) {
        dbg_printf("\n");
    }
}
uint8_t DumpHID(INTERFACE *pInterface) {
    uint8_t count = 0;
    UsbLinkedList *tmpsegNode;
    HID_SEG *tmpsegment;
    for (uint8_t x = 0; x < MAX_REPORTS; x++) {
        HID_REPORT *tr = (HID_REPORT *)UsbListGetData(pInterface->Reports, x);
        if (tr != NULL) {
            tmpsegNode = tr->HidSegments;
            TRACE("Report %x, usage %x, length %u: ", x, tr->appUsage, tr->length);
            while (tmpsegNode != NULL) {
                tmpsegment = (HID_SEG *)(tmpsegNode->data);
                TRACE("startbit %u, it %hx, ip %x, chan %hx, cont %hx, size %hx, count %hx", tmpsegment->startBit, tmpsegment->InputType, tmpsegment->InputParam, tmpsegment->OutputChannel, tmpsegment->OutputControl, tmpsegment->reportSize, tmpsegment->reportCount);
                tmpsegNode = tmpsegNode->next;
                count++;
            }
        }
    }
    return count;
}
#else
#define DumpHex(...) { }
#define DumpHID(...) { }
#endif

//-----------------------------------------------------------------------------------------
// keyb = 458, mouse = 196
//#define MEMPOOLMAXSIZE 3600
#define MEMPOOLMAXSIZE 1400
#define RECEIVE_BUFFER_LEN 512
UINT8X ReceiveDataBuffer[RECEIVE_BUFFER_LEN];
__xdata uint8_t MemPool[MEMPOOLMAXSIZE];
__xdata uint8_t *MemPoolPtr = MemPool;
__xdata uint8_t *MemPoolTmp;

__xdata volatile bool forceEnumerate = false;

__at(0x0000) unsigned char __xdata RxBuffer[MAX_PACKET_SIZE];
__at(0x0100) unsigned char __xdata TxBuffer[MAX_PACKET_SIZE];
USB_HUB_PORT __xdata RootHubPort[ROOT_HUB_PORT_NUM];
USB_HUB_PORT __xdata SubHubPort[ROOT_HUB_PORT_NUM][MAX_EXHUB_PORT_NUM];


//-----------------------------------------------------------------------------------------
void InitUsbMemory(void) { MemPoolPtr = MemPool; }
uint16_t UsbMemoryUsed(void) { return MemPoolPtr - MemPool; }
uint16_t UsbMemoryFree(void) { return MEMPOOLMAXSIZE - UsbMemoryUsed(); }

void __xdata *UsbMemoryAlloc(uint16_t size) {
    // trigger a watchdog reset if we run out of memory
    if (UsbMemoryFree() <= size) {
        TRACE("Ram Exhausted");
        ET0 = 0;
        while (1);
    }
    MemPoolTmp = MemPoolPtr;
    MemPoolPtr += size;
    return MemPoolTmp;
}

__xdata UsbLinkedList* UsbListAdd(__xdata UsbLinkedList* head, uint16_t data_size, uint8_t index) {
    __xdata struct UsbLinkedList* newNode = (__xdata UsbLinkedList*)UsbMemoryAlloc(sizeof(UsbLinkedList));
    newNode->data = UsbMemoryAlloc(data_size);
    newNode->next = head;
    memset(newNode->data, 0x00, data_size);
    newNode->index = index;
    return newNode;
}

void *UsbListGetData(UsbLinkedList* head, uint8_t index) {
    while (head != NULL) {
        if (head->index == index) {
            return head->data;
        }
        head = head->next;
    }
    return NULL;
}

//-----------------------------------------------------------------------------------------
UINT8 EnableRootHubPort(UINT8 rootHubIndex)
{
	if (rootHubIndex == 0) {
		if (USB_HUB_ST & bUHS_H0_ATTACH) {
			if ((UHUB0_CTRL & bUH_PORT_EN) == 0x00) {
				if (USB_HUB_ST & bUHS_DM_LEVEL) {
					RootHubPort[0].DeviceSpeed = DEVICE_SPEED_LOW;
					TRACE("low speed dev, hub 0");
				} else {
					RootHubPort[0].DeviceSpeed = DEVICE_SPEED_FULL;
					TRACE("full speed dev, hub 0");
				}
				if (RootHubPort[0].DeviceSpeed == DEVICE_SPEED_LOW) {
					UHUB0_CTRL |= bUH_LOW_SPEED;
				}
			}
			UHUB0_CTRL |= bUH_PORT_EN;
			return ERR_SUCCESS;
		}
	} else if (rootHubIndex == 1) {
		if (USB_HUB_ST & bUHS_H1_ATTACH) {
			if ((UHUB1_CTRL & bUH_PORT_EN) == 0x00) {
				if (USB_HUB_ST & bUHS_HM_LEVEL) {
					RootHubPort[1].DeviceSpeed = DEVICE_SPEED_LOW;
					TRACE("low speed dev, hub 1");
				} else {
					RootHubPort[1].DeviceSpeed = DEVICE_SPEED_FULL;
					TRACE("full speed dev, hub 1");
				}
				if (RootHubPort[1].DeviceSpeed == DEVICE_SPEED_LOW) {
					UHUB1_CTRL |= bUH_LOW_SPEED;
				}
			}
			UHUB1_CTRL |= bUH_PORT_EN;
			return ERR_SUCCESS;
		}
	}
	return ERR_USB_DISCON;
}

void DisableRootHubPort(UINT8 RootHubIndex)
{
	InitRootHubPortData(RootHubIndex);
	if (RootHubIndex == 0) {
		UHUB0_CTRL = 0x00;
	} else if (RootHubIndex == 1) {
		UHUB1_CTRL = 0x00;
	}
}

void InitHubPortData(USB_HUB_PORT *pUsbHubPort)
{
	pUsbHubPort->HubPortStatus = PORT_DEVICE_NONE;
	pUsbHubPort->DeviceClass = USB_DEV_CLASS_RESERVED;
	pUsbHubPort->MaxPacketSize0 = DEFAULT_ENDP0_SIZE;

	pUsbHubPort->VendorID = 0x0000;
	pUsbHubPort->ProductID = 0x0000;
	pUsbHubPort->bcdDevice = 0x0000;

	pUsbHubPort->DeviceAddress = 0;
	pUsbHubPort->DeviceSpeed = DEVICE_SPEED_FULL;
	pUsbHubPort->InterfaceNum = 0;
	pUsbHubPort->Interfaces = NULL;

	pUsbHubPort->HubPortNum = 0;
}

void InitRootHubPortData(UINT8 rootHubIndex)
{
	InitHubPortData(&RootHubPort[rootHubIndex]);
	for (UINT8 i = 0; i < MAX_EXHUB_PORT_NUM; i++) {
		InitHubPortData(&SubHubPort[rootHubIndex][i]);
	}
}

void SetHostUsbAddr(UINT8 addr)
{
	USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | addr & 0x7F;
}

void SetUsbSpeed(UINT8 FullSpeed)
{
	if (FullSpeed) {
		USB_CTRL &= ~bUC_LOW_SPEED;
		UH_SETUP &= ~bUH_PRE_PID_EN;
	} else {
		USB_CTRL |= bUC_LOW_SPEED;
	}
}

void ResetRootHubPort(UINT8 RootHubIndex)
{
	SetHostUsbAddr(0x00);
	SetUsbSpeed(1);

	if (RootHubIndex == 0) {
		UHUB0_CTRL = UHUB0_CTRL & ~bUH_LOW_SPEED | bUH_BUS_RESET;
		delayms(15);
		UHUB0_CTRL = UHUB0_CTRL & ~bUH_BUS_RESET;
	} else if (RootHubIndex == 1) {
		UHUB1_CTRL = UHUB1_CTRL & ~bUH_LOW_SPEED | bUH_BUS_RESET;
		delayms(15);
		UHUB1_CTRL = UHUB1_CTRL & ~bUH_BUS_RESET;
	}

	delayus(250);
	UIF_DETECT = 0;
}

void SelectHubPort(UINT8 RootHubIndex, UINT8 HubPortIndex)
{
	if (HubPortIndex == EXHUB_PORT_NONE) {
		//normal device
		SetHostUsbAddr(RootHubPort[RootHubIndex].DeviceAddress);
		SetUsbSpeed(RootHubPort[RootHubIndex].DeviceSpeed);
	} else {
		USB_HUB_PORT *pUsbDevice = &SubHubPort[RootHubIndex][HubPortIndex];
		SetHostUsbAddr(pUsbDevice->DeviceAddress);
		if (pUsbDevice->DeviceSpeed == DEVICE_SPEED_LOW) {
			UH_SETUP |= bUH_PRE_PID_EN;
		}
		SetUsbSpeed(pUsbDevice->DeviceSpeed);
	}
}

void InitUsbHost(void)
{
	IE_USB = 0;
	USB_CTRL = bUC_HOST_MODE;
	USB_DEV_AD = 0x00;
	UH_EP_MOD = bUH_EP_TX_EN | bUH_EP_RX_EN;
	UH_RX_DMA = 0x0000;
	UH_TX_DMA = 0x0001;
	UH_RX_CTRL = 0x00;
	UH_TX_CTRL = 0x00;

	UHUB1_CTRL &= ~bUH1_DISABLE;
	USB_CTRL = bUC_HOST_MODE | bUC_INT_BUSY | bUC_DMA_EN;

	UH_SETUP = bUH_SOF_EN;
	USB_INT_FG = 0xFF;
    DisableRootHubPort(0);
    DisableRootHubPort(1);

	USB_INT_EN = bUIE_TRANSFER | bUIE_DETECT;
}

UINT8 USBHostTransact(UINT8 endp_pid, UINT8 tog, UINT16 timeout)
{
	UINT8 TransRetry = 0;
	UH_RX_CTRL = UH_TX_CTRL = tog;

	do
	{
		// wait for next start of frame
		while (!(USB_MIS_ST & bUMS_SOF_ACT));

		UH_EP_PID = endp_pid;
		UIF_TRANSFER = 0;
		for (UINT16 i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i--) {
			;
		}

		UH_EP_PID = 0x00;
		if (UIF_TRANSFER == 0) {
			TRACE("fail");
			return (ERR_USB_UNKNOWN);
		}

		if (UIF_TRANSFER) {
			if (U_TOG_OK) {
				//TRACE("retry:%d", (UINT16)TransRetry);
				return (ERR_SUCCESS);
			}

#if 0
			TRACE("endp_pid=$%x", (UINT16)endp_pid);
			TRACE("USB_INT_FG=$%x", (UINT16)USB_INT_FG);
			TRACE("USB_INT_ST=$%x", (UINT16)USB_INT_ST);
			TRACE("USB_MIS_ST=$%x", (UINT16)USB_MIS_ST);
			TRACE("USB_RX_LEN=$%x", (UINT16)USB_RX_LEN);
			TRACE("UH_TX_LEN=$%x", (UINT16)UH_TX_LEN);
			TRACE("UH_RX_CTRL=$%x", (UINT16)UH_RX_CTRL);
			TRACE("UH_TX_CTRL=$%x", (UINT16)UH_TX_CTRL);
			TRACE("UHUB0_CTRL=$%x", (UINT16)UHUB0_CTRL);
			TRACE("UHUB1_CTRL=$%x", (UINT16)UHUB1_CTRL);
#endif

			UINT8 r = USB_INT_ST & MASK_UIS_H_RES;
			//TRACE("r:$%x", (UINT16)r);

			if (r == USB_PID_STALL) {
				TRACE("fail");
				return (r | ERR_USB_TRANSFER);
			}

			if (r == USB_PID_NAK) {
				if (timeout == 0) {
				    //TRACE("fail");
					return (r | ERR_USB_TRANSFER);
				} else if (timeout < 0xFFFF) {
					timeout--;
				}
				if (TransRetry > 0) {
					TransRetry--;
				}
			} else {
				switch (endp_pid >> 4)
				{
				case USB_PID_SETUP:
				case USB_PID_OUT:
					if (r) {
    				    TRACE("fail");
						return (r | ERR_USB_TRANSFER);
					}
					break;

				case USB_PID_IN:
					if (r == USB_PID_DATA0 && r == USB_PID_DATA1) {
					}
                    else if (r) {
    				    TRACE("fail");
						return (r | ERR_USB_TRANSFER);
					}
					break;
				default:
					return (ERR_USB_UNKNOWN);
					break;
				}
			}
		} else {
			USB_INT_FG = 0xFF;
		}
		delayus(25);
	} while (++TransRetry < 250);

    TRACE("fail");
	return (ERR_USB_TRANSFER);
}

UINT8 HostCtrlTransfer(USB_SETUP_REQ *pSetupReq, UINT8 MaxPacketSize0, PUINT8 DataBuf, PUINT16 RetLen)
{
	PUINT8 pBuf = DataBuf;
	PUINT16 pLen = RetLen;
	delayus(200);
	if (pLen) {
		*pLen = 0;
	}

	for (UINT8 TxCnt = 0; TxCnt < sizeof(USB_SETUP_REQ); TxCnt++) {
		TxBuffer[TxCnt] = ((UINT8 *)pSetupReq)[TxCnt];
	}

	UH_TX_LEN = sizeof(USB_SETUP_REQ);

	UINT8 s = USBHostTransact((UINT8)(USB_PID_SETUP << 4), 0x00, 10000);
	if (s != ERR_SUCCESS) {
        TRACE("fail %d", s);
		return (s);
	}
	UH_RX_CTRL = UH_TX_CTRL = bUH_R_TOG | bUH_R_AUTO_TOG | bUH_T_TOG | bUH_T_AUTO_TOG;
	UH_TX_LEN = 0x01;
	UINT16 RemLen = (pSetupReq->wLengthH << 8) | (pSetupReq->wLengthL);
	if (RemLen && pBuf) {
		if (pSetupReq->bRequestType & USB_REQ_TYP_IN) {
			while (RemLen) {
				delayus(200);
				s = USBHostTransact((UINT8)(USB_PID_IN << 4), UH_RX_CTRL, 10000);
				if (s != ERR_SUCCESS) {
                    TRACE("fail %d", s);
					return (s);
				}
				UINT8 RxLen = USB_RX_LEN < RemLen ? USB_RX_LEN : RemLen;
				RemLen -= RxLen;
				if (pLen) {
					*pLen += RxLen;
				}

				for (UINT8 RxCnt = 0; RxCnt != RxLen; RxCnt++) {
					*pBuf = RxBuffer[RxCnt];
					pBuf++;
				}
				if (USB_RX_LEN == 0 || (USB_RX_LEN < MaxPacketSize0)) {
					break;
				}
			}
			UH_TX_LEN = 0x00;
		} else {
			while (RemLen) {
				delayus(200);
				UH_TX_LEN = RemLen >= MaxPacketSize0 ? MaxPacketSize0 : RemLen;

				for (UINT8 TxCnt = 0; TxCnt != UH_TX_LEN; TxCnt++) {
					TxBuffer[TxCnt] = *pBuf;
					pBuf++;
				}
				s = USBHostTransact(USB_PID_OUT << 4 | 0x00, UH_TX_CTRL, 200000 / 20);
				if (s != ERR_SUCCESS) {
                    TRACE("fail %d", s);
					return (s);
				}
				RemLen -= UH_TX_LEN;
				if (pLen) {
					*pLen += UH_TX_LEN;
				}
			}
		}
	}

	delayus(200);
	s = USBHostTransact((UH_TX_LEN ? USB_PID_IN << 4 | 0x00 : USB_PID_OUT << 4 | 0x00), bUH_R_TOG | bUH_T_TOG, 200000 / 20);
	if (s != ERR_SUCCESS) {
        TRACE("fail %d", s);
		return (s);
	}
	if (UH_TX_LEN == 0) {
		return (ERR_SUCCESS);
	}
	if (USB_RX_LEN == 0) {
		return (ERR_SUCCESS);
	}
	return (ERR_USB_BUF_OVER);
}

UINT8 TransferReceive(ENDPOINT *pEndPoint, UINT8 *pData, UINT16 *pRetLen, UINT16 timeout)
{
	UINT8 s = USBHostTransact(USB_PID_IN << 4 | (pEndPoint->EndpointAddr & 0x7F), pEndPoint->TOG ? bUH_R_TOG | bUH_T_TOG : 0, timeout);
	if (s == ERR_SUCCESS) {
		UINT8 len = USB_RX_LEN;
		for (UINT8 i = 0; i < len; i++) {
			*pData++ = RxBuffer[i];
		}
		if (pRetLen != NULL) {
			*pRetLen = len;
		}
		pEndPoint->TOG = pEndPoint->TOG ? FALSE : TRUE;
	}
	return (s);
}




//-----------------------------------------------------------------------------------------
void InitInterface(INTERFACE* Interface)
{
	memset(Interface, 0, sizeof(INTERFACE));

	Interface->InterfaceClass = USB_DEV_CLASS_RESERVED;
	Interface->InterfaceProtocol = USB_PROTOCOL_NONE;
	Interface->ReportSize = 0;
	Interface->EndpointNum = 0;

	for (int j = 0; j < MAX_ENDPOINT_NUM; j++) {
		Interface->Endpoint[j].EndpointAddr = 0;
		Interface->Endpoint[j].MaxPacketSize = 0;
		Interface->Endpoint[j].EndpointDir = ENDPOINT_IN;
		Interface->Endpoint[j].TOG = FALSE;
	}

	Interface->usesReports = 0;
}

void FillSetupReq(USB_SETUP_REQ *pSetupReq, UINT8 type, UINT8 req, UINT16 value, UINT16 index, UINT16 length)
{
	pSetupReq->bRequestType = type;
	pSetupReq->bRequest = req;
	pSetupReq->wValueL = value & 0xff;
	pSetupReq->wValueH = (value >> 8) & 0xff;
	pSetupReq->wIndexL = index & 0xff;
	pSetupReq->wIndexH = (index >> 8) & 0xff;
	pSetupReq->wLengthL = length & 0xff;
	pSetupReq->wLengthH = (length >> 8) & 0xff;
}

//-----------------------------------------------------------------------------------------
UINT8 GetDeviceDescr(USB_HUB_PORT *pUsbDevice, UINT8 *pDevDescr, UINT16 reqLen, UINT16 *pRetLen) //get device describtion
{
	static __xdata UINT8 s;
	static __xdata UINT16 len;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, USB_REQ_TYP_IN | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE, USB_GET_DESCRIPTOR, USB_DESCR_TYP_DEVICE << 8, 0, reqLen);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, pDevDescr, &len);
    if (s == ERR_SUCCESS) {
		if (pRetLen != NULL) {
			*pRetLen = len;
		}
	}
	return s;
}

//----------------------------------------------------------------------------------------
UINT8 GetConfigDescr(USB_HUB_PORT *pUsbDevice, UINT8 *pCfgDescr, UINT16 reqLen, UINT16 *pRetLen)
{
	static __xdata UINT8 s;
	static __xdata UINT16 len;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, USB_REQ_TYP_IN | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE, USB_GET_DESCRIPTOR, USB_DESCR_TYP_CONFIG << 8, 0, reqLen);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, pCfgDescr, &len);
    if (s == ERR_SUCCESS) {
		if (pRetLen != NULL) {
			*pRetLen = len;
		}
	}
	return s;
}

//-------------------------------------------------------------------------------------
UINT8 SetUsbAddress(USB_HUB_PORT *pUsbDevice, UINT8 addr)
{
	static __xdata UINT8 s;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, USB_REQ_TYP_OUT | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE, USB_SET_ADDRESS, addr, 0, 0);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, NULL, NULL);
	if (s == ERR_SUCCESS) {
		SetHostUsbAddr(addr);
	}
	return s;
}

//-------------------------------------------------------------------------------------
UINT8 SetUsbConfig(USB_HUB_PORT *pUsbDevice, UINT8 cfg)
{
	static __xdata UINT8 s;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, USB_REQ_TYP_OUT | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_DEVICE, USB_SET_CONFIGURATION, cfg, 0, 0);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, NULL, NULL);
	return s;
}

//-----------------------------------------------------------------------------------------
UINT8 GetHubDescriptor(USB_HUB_PORT *pUsbDevice, UINT8 *pHubDescr, UINT16 reqLen, UINT16 *pRetLen)
{
	static __xdata UINT8 s;
	static __xdata UINT16 len;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, USB_REQ_TYP_IN | USB_REQ_TYP_CLASS | USB_REQ_RECIP_DEVICE, USB_GET_DESCRIPTOR, USB_DESCR_TYP_HUB << 8, 0, reqLen);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, (UINT8 *)pHubDescr, &len);
	if (s == ERR_SUCCESS) {
		if (pRetLen != NULL) {
			*pRetLen = len;
		}
	}
	return s;
}

//-----------------------------------------------------------------------------------------
UINT8 GetHubPortStatus(USB_HUB_PORT *pUsbDevice, UINT8 HubPort, UINT16 *pPortStatus, UINT16 *pPortChange)
{
	static __xdata UINT8 s;
	static __xdata UINT8 Ret[4];
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, USB_REQ_TYP_IN | USB_REQ_TYP_CLASS | USB_REQ_RECIP_OTHER, USB_GET_STATUS, 0, HubPort, 4);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, Ret, NULL);
	if (s == ERR_SUCCESS) {
		if (pPortStatus != NULL) {
			*pPortStatus = (Ret[1] << 8) | Ret[0];
		}
		if (pPortChange != NULL) {
			*pPortChange = (Ret[3] << 8) | Ret[2];
		}
	}
	return s;
}

//------------------------------------------------------------------------------------------
UINT8 SetHubPortFeature(USB_HUB_PORT *pUsbDevice, UINT8 HubPort, UINT8 selector) //this function set feature for port						//this funciton set
{
	static __xdata UINT8 s;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, USB_REQ_TYP_OUT | USB_REQ_TYP_CLASS | USB_REQ_RECIP_OTHER, USB_SET_FEATURE, selector, (0 << 8) | HubPort, 0);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, NULL, NULL);
	return s;
}

UINT8 ClearHubPortFeature(USB_HUB_PORT *pUsbDevice, UINT8 HubPort, UINT8 selector)
{
	static __xdata UINT8 s;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, USB_REQ_TYP_OUT | USB_REQ_TYP_CLASS | USB_REQ_RECIP_OTHER, USB_CLEAR_FEATURE, selector, (0 << 8) | HubPort, 0);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, NULL, NULL);
	return s;
}

//-----------------------------------------------------------------------------------------
UINT8 GetReportDescriptor(USB_HUB_PORT *pUsbDevice, UINT8 interface, UINT8 *pReportDescr, UINT16 reqLen, UINT16 *pRetLen)
{
	static __xdata UINT8 s;
	static __xdata UINT16 len;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, USB_REQ_TYP_IN | USB_REQ_TYP_STANDARD | USB_REQ_RECIP_INTERF, USB_GET_DESCRIPTOR, USB_DESCR_TYP_REPORT << 8, interface, reqLen);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, pReportDescr, &len);
	if (s == ERR_SUCCESS) {
		if (pRetLen != NULL) {
			*pRetLen = len;
		}
	}
	return s;
}

UINT8 SetBootProtocol(USB_HUB_PORT *pUsbDevice, UINT8 interface, uint16_t val)
{
	static __xdata UINT8 s;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, 0b00100001, HID_SET_PROTOCOL, val, interface, 0);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, NULL, NULL);
	return s;
}

UINT8 GetBootProtocol(USB_HUB_PORT *pUsbDevice, UINT8 interface)
{
	static __xdata UINT8 s;
	static __xdata UINT8 ret;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, 0b10100001, HID_GET_PROTOCOL, 0, interface, 1);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, &ret, NULL);
	return ret;
}

//-----------------------------------------------------------------------------------------
UINT8 SetIdle(USB_HUB_PORT *pUsbDevice, UINT16 durationMs, UINT8 reportID, UINT8 interface)
{
	static __xdata UINT8 s;
	static __xdata USB_SETUP_REQ SetupReq;
	UINT8 duration = (UINT8)(durationMs / 4);
	FillSetupReq(&SetupReq, USB_REQ_TYP_OUT | USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF, HID_SET_IDLE, (duration << 8) | reportID, interface, 0);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, NULL, NULL);
	return s;
}

//-----------------------------------------------------------------------------------------------
UINT8 SetReport(USB_HUB_PORT *pUsbDevice, UINT8 interface, UINT8 *pReport, UINT16 ReportLen)
{
	static __xdata UINT8 s;
	static __xdata USB_SETUP_REQ SetupReq;
	FillSetupReq(&SetupReq, USB_REQ_TYP_OUT | USB_REQ_TYP_CLASS | USB_REQ_RECIP_INTERF, HID_SET_REPORT, HID_REPORT_OUTPUT << 8, interface, ReportLen);
	s = HostCtrlTransfer(&SetupReq, pUsbDevice->MaxPacketSize0, pReport, NULL);
	return s;
}

//-----------------------------------------------------------------------------------------------
void InitUsbData(void)
{
    InitRootHubPortData(0);
    InitRootHubPortData(1);
}

//-------------------------------------------------------------------------------------------
UINT8 HIDDataTransferReceive(USB_HUB_PORT *pUsbDevice)
{
	static UINT8 s, p;
	static int i, j;
	static int interfaceNum;
	static int endpointNum;

	static UINT16 len;

	s = 0;
	interfaceNum = pUsbDevice->InterfaceNum;
	for (i = 0; i < interfaceNum; i++) {
		//INTERFACE *pInterface = &pUsbDevice->Interface[i];
		__xdata INTERFACE *pInterface = (__xdata INTERFACE *)UsbListGetData(pUsbDevice->Interfaces, i);
		if (pInterface != NULL && pInterface->InterfaceClass == USB_DEV_CLASS_HID) {
			endpointNum = pInterface->EndpointNum;
			for (j = 0; j < endpointNum; j++) {
				ENDPOINT *pEndPoint = &pInterface->Endpoint[j];
				if (pEndPoint->EndpointDir == ENDPOINT_IN) {
					s = TransferReceive(pEndPoint, ReceiveDataBuffer, &len, 0);
					if (s == ERR_SUCCESS) {
						//TRACE("interface %d data:", (UINT16)i);
						// HIS IS WHERE THE FUN STUFF GOES
						//ProcessHIDData(pInterface, ReceiveDataBuffer, len);
						ParseReport(pInterface, len * 8, ReceiveDataBuffer);
					}
                    else{
#if 0
                        // todo: disconnect after x amount of errors?
                        if (s == ERR_USB_TRANSFER) {
                            TRACE("interface %d err $%x", (UINT16)i, s);
                            pUsbDevice->HubPortStatus = PORT_DEVICE_ENUM_FAILED;
                        }
#endif                        
                    }
				}
			}
		}
	}

	return (s);
}

//enum device
BOOL EnumerateHubPort(__xdata USB_HUB_PORT *pUsbHubPort, UINT8 addr)
{
	static __xdata UINT8 s;
	static __xdata UINT16 len;
	static __xdata UINT16 cfgDescLen;

	static __xdata USB_HUB_PORT *pUsbDevice;
	static __xdata USB_CFG_DESCR *pCfgDescr;

	pUsbDevice = pUsbHubPort;

	//get first 8 bytes of device descriptor to get maxpacketsize0
	s = GetDeviceDescr(pUsbDevice, ReceiveDataBuffer, 8, &len);
	if (s != ERR_SUCCESS) {
		TRACE("gdd.fail");
		return (FALSE);
	}
	TRACE("gdd len:%d", len);
	ParseDeviceDescriptor((USB_DEV_DESCR *)ReceiveDataBuffer, len, pUsbDevice);
	TRACE("mps %d", pUsbDevice->MaxPacketSize0);

	//set device address
	s = SetUsbAddress(pUsbDevice, addr);
	if (s != ERR_SUCCESS) {
		TRACE("addr fail");
		return (FALSE);
	}

	TRACE("addr ok %d", pUsbDevice->DeviceAddress);
	pUsbDevice->DeviceAddress = addr;
	//get full bytes of device descriptor
	s = GetDeviceDescr(pUsbDevice, ReceiveDataBuffer, sizeof(USB_DEV_DESCR), &len);
	if (s != ERR_SUCCESS) {
		TRACE("gddfull fail");
		return (FALSE);
	}

	TRACE("gddfull ok %d", len);
	TRACE("Device Descriptor :")
	DumpHex(ReceiveDataBuffer, len);

	ParseDeviceDescriptor((USB_DEV_DESCR *)ReceiveDataBuffer, len, pUsbDevice);
	TRACE("$%x $%x $%x", pUsbDevice->VendorID, pUsbDevice->ProductID, pUsbDevice->bcdDevice);

	//get configure descriptor for the first time
	cfgDescLen = sizeof(USB_CFG_DESCR);
	s = GetConfigDescr(pUsbDevice, ReceiveDataBuffer, cfgDescLen, &len);
	if (s != ERR_SUCCESS) {
		TRACE("gcd1 fail");
		return (FALSE);
	}

	//get configure descriptor for the second time
	pCfgDescr = (USB_CFG_DESCR *)ReceiveDataBuffer;
	cfgDescLen = pCfgDescr->wTotalLengthL | (pCfgDescr->wTotalLengthH << 8);
	if (cfgDescLen > RECEIVE_BUFFER_LEN) {
		cfgDescLen = RECEIVE_BUFFER_LEN;
	}

	s = GetConfigDescr(pUsbDevice, ReceiveDataBuffer, cfgDescLen, &len);
	if (s != ERR_SUCCESS) {
		TRACE("gcd 2 fail");
		return (FALSE);
	}

	TRACE("Config Descriptor :")
	DumpHex(ReceiveDataBuffer, len);

	//parse config descriptor
	ParseConfigDescriptor((USB_CFG_DESCR *)ReceiveDataBuffer, len, pUsbDevice);

	//set config
	s = SetUsbConfig(pUsbDevice, ((USB_CFG_DESCR *)ReceiveDataBuffer)->bConfigurationValue);
	if (s != ERR_SUCCESS) {
		TRACE("SetUsbConfig fail");
		return (FALSE);
	}

	TRACE("configure=%bd", ((USB_CFG_DESCR *)ReceiveDataBuffer)->bConfigurationValue);
	TRACE("pUsbDevice->InterfaceNum=%d", (UINT16)pUsbDevice->InterfaceNum);
	return (TRUE);
}

UINT8 AssignUniqueAddress(UINT8 RootHubIndex, UINT8 HubPortIndex)
{
	static UINT8 address;
	if (HubPortIndex == EXHUB_PORT_NONE) {
		address = (MAX_EXHUB_PORT_NUM + 1) * RootHubIndex + 1;
	} else {
		address = (MAX_EXHUB_PORT_NUM + 1) * RootHubIndex + 1 + HubPortIndex + 1;
	}
	return address;
}

BOOL QuerySubHubPort(UINT8 port)
{
    USB_HUB_PORT *pUsbDevice = &RootHubPort[port];
    INTERFACE *pInterface = (INTERFACE *)UsbListGetData(pUsbDevice->Interfaces, 0);
    ENDPOINT *pEndPoint = &pInterface->Endpoint[0];
    UINT8 hubPortNum = pUsbDevice->HubPortNum;

    for (UINT8 i = 0; i < hubPortNum; i++) {
        SelectHubPort(port, EXHUB_PORT_NONE);
        UINT16 hubPortStatus, hubPortChange;
        uint8_t s = GetHubPortStatus(pUsbDevice, i + 1, &hubPortStatus, &hubPortChange);
        //TRACE("%d/%d : $%x $%x", i, hubPortNum, hubPortStatus, hubPortChange);
        if (hubPortChange & 0x0001) {
            TRACE("%d:%d changed: $%x $%x", port, hubPortNum, hubPortStatus, hubPortChange);
            forceEnumerate = true;
        }
    }
    return true;
}

BOOL EnumerateRootHubPort(UINT8 port)
{
	static __xdata UINT8 i, s;
	static __xdata UINT16 len;
	static __xdata UINT8 retry;
	static __xdata UINT8 addr;
	retry = 0;
	/*if (RootHubPort[port].HubPortStatus != PORT_DEVICE_INSERT)
	{
		return FALSE;
	}*/

	TRACE("enumerate port:%bd", port);

//	DisableRootHubPort(port);
//	delayms(500);

	ResetRootHubPort(port);
//	delayms(500);

#if 1
	for (i = 0, s = 0; i < 10; i++) {
		EnableRootHubPort(port);
		delayms(1);
	}

#else
	for (i = 0, s = 0; i < 100; i++) // �ȴ�USB�豸��λ����������,100mS��ʱ
	{
		mDelaymS(1);
		if (EnableRootHubPort(port) == ERR_SUCCESS) // ʹ��ROOT-HUB�˿�
		{
			i = 0;
			s++; // ��ʱ�ȴ�USB�豸���Ӻ��ȶ�
			if (s > 10 * retry)
			{
				break; // �Ѿ��ȶ�����15mS
			}
		}
	}

	TRACE("i:%d", (UINT16)i);

	if (i) // ��λ���豸û������
	{
		DisableRootHubPort(port);
		TRACE("Disable root hub %1d# port because of disconnect", (UINT16)port);

		return (ERR_USB_DISCON);
	}
#endif

	delayms(100);
	SelectHubPort(port, EXHUB_PORT_NONE);

	addr = AssignUniqueAddress(port, EXHUB_PORT_NONE);
	if (EnumerateHubPort(&RootHubPort[port], addr)) {
		RootHubPort[port].HubPortStatus = PORT_DEVICE_ENUM_SUCCESS;
		TRACE("EnumerateHubPort success");

		//SelectHubPort(port, EXHUB_PORT_NONE);
		if (RootHubPort[port].DeviceClass == USB_DEV_CLASS_HUB) {
			TRACE("Found hub");

			//hub
			USB_HUB_DESCR *pHubDescr;
			UINT8 hubPortNum;
			UINT16 hubPortStatus, hubPortChange;
			USB_HUB_PORT *pUsbDevice = &RootHubPort[port];

			//hub
			s = GetHubDescriptor(pUsbDevice, ReceiveDataBuffer, sizeof(USB_HUB_DESCR), &len);
			if (s != ERR_SUCCESS) {
				DisableRootHubPort(port);
				RootHubPort[port].HubPortStatus = PORT_DEVICE_ENUM_FAILED;
				TRACE("hub desc err");
				return (FALSE);
			}

			TRACE("GetHubDescriptor len=%d", len);

			pHubDescr = (USB_HUB_DESCR *)ReceiveDataBuffer;
			hubPortNum = pHubDescr->bNbrPorts;

			TRACE("hubPortNum=%bd", hubPortNum);

			if (hubPortNum > MAX_EXHUB_PORT_NUM) {
				hubPortNum = MAX_EXHUB_PORT_NUM;
			}

			pUsbDevice->HubPortNum = hubPortNum;

			//supply power for each port
			for (i = 0; i < hubPortNum; i++) {
				s = SetHubPortFeature(pUsbDevice, i + 1, HUB_PORT_POWER);
				if (s != ERR_SUCCESS) {
					TRACE("SetHubPortFeature %d failed", (UINT16)i);
					SubHubPort[port][i].HubPortStatus = PORT_DEVICE_ENUM_FAILED;
					continue;
				}
				TRACE("SetHubPortFeature OK");
			}

			/*
			for (i = 0; i < hubPortNum; i++){
                s = ClearHubPortFeature(pUsbDevice, i + 1, HUB_C_PORT_CONNECTION );
                if (s != ERR_SUCCESS) {
                    TRACE("ClearHubPortFeature %d failed", (UINT16)i);
                    continue;
                }
                TRACE("ClearHubPortFeature OK");
            }
*/
			INTERFACE *pInterface = (INTERFACE *)UsbListGetData(pUsbDevice->Interfaces, 0);
			ENDPOINT *pEndPoint = &pInterface->Endpoint[0];

			// "new thing" means wait for a change bitmap before clearing ports
			// this is how other USB stacks seem to do it
			TRACE("Doing new thing - %x", pInterface->Endpoint[0].EndpointAddr);

			uint8_t newthing = 1;
			s = TransferReceive(pEndPoint, ReceiveDataBuffer, &len, 20000);
			if (s != ERR_SUCCESS){
				TRACE("new enum. failed");
				// if "new thing" failed just clear all ports without waiting
				// seems to be required for some hubs
				newthing = 0;
				//return FALSE;
			}

			uint8_t changebitmap = ReceiveDataBuffer[0];
			TRACE("new enum. %x", changebitmap);

			for (i = 0; i < hubPortNum; i++) {
				if (!newthing || (changebitmap & (1 << (i+1)))) {
					TRACE("checkn port %d", i);
				    delayms(50);

					SelectHubPort(port, EXHUB_PORT_NONE);
					s = GetHubPortStatus(pUsbDevice, i + 1, &hubPortStatus, &hubPortChange);
					if (s != ERR_SUCCESS) {
						SubHubPort[port][i].HubPortStatus = PORT_DEVICE_ENUM_FAILED;
						TRACE("GetHubPortStatus port:%d failed", (UINT16)(i + 1));
						return FALSE;
					}
					TRACE("ps- $%x pc- $%x", hubPortStatus, hubPortChange);

					if ((hubPortStatus & 0x0001) && (hubPortChange & 0x0001)) {
						//device attached
						TRACE("port %d attached", i);

						s = ClearHubPortFeature(pUsbDevice, i + 1, HUB_C_PORT_CONNECTION);
						if (s != ERR_SUCCESS) {
							SubHubPort[port][i].HubPortStatus = PORT_DEVICE_ENUM_FAILED;
							TRACE("ClearHubPortFeature failed");
							return FALSE;
						}

						TRACE("ClearHubPortFeature OK");
						s = SetHubPortFeature(pUsbDevice, i + 1, HUB_PORT_RESET); //reset the port device
						if (s != ERR_SUCCESS) {
							SubHubPort[port][i].HubPortStatus = PORT_DEVICE_ENUM_FAILED;
							TRACE("SetHubPortFeature port:%d failed", (UINT16)(i + 1));
							return FALSE;
						}

						delayms(100);
						do
						{
							s = GetHubPortStatus(pUsbDevice, i + 1, &hubPortStatus, &hubPortChange);
							if (s != ERR_SUCCESS) {
								SubHubPort[port][i].HubPortStatus = PORT_DEVICE_ENUM_FAILED;
								TRACE("GetHubPortStatus port:%d failed", (UINT16)(i + 1));
								return FALSE;
							}
							delayms(20);
						} while (hubPortStatus & 0x0010);

						if ((hubPortChange & 0x10) == 0x10) {   //reset over success
							TRACE("reset complete");
							if (hubPortStatus & 0x0200) {
								SubHubPort[port][i].DeviceSpeed = DEVICE_SPEED_LOW;
								TRACE("lowspeed");
							} else {
								SubHubPort[port][i].DeviceSpeed = DEVICE_SPEED_FULL;
								TRACE("fullspeed");
							}

							s = ClearHubPortFeature(pUsbDevice, i + 1, HUB_C_PORT_RESET);
							if (s != ERR_SUCCESS) {
								TRACE("ClearHubPortFeature failed");
							}

							/*s = ClearHubPortFeature(pUsbDevice, i + 1, HUB_PORT_SUSPEND);
							if (s != ERR_SUCCESS) {
								SubHubPort[port][i].HubPortStatus = PORT_DEVICE_ENUM_FAILED;
								TRACE("ClearHubPortFeature failed");
								return FALSE;
							}
							TRACE("ClearHubPortFeature OK");
							delayms(500);*/

							SelectHubPort(port, i);
							addr = AssignUniqueAddress(port, i);
							if (EnumerateHubPort(&SubHubPort[port][i], addr)) {
								TRACE("enum.ok");
								SubHubPort[port][i].HubPortStatus = PORT_DEVICE_ENUM_SUCCESS;
							} else {
								TRACE("enum.fail");
								SubHubPort[port][i].HubPortStatus = PORT_DEVICE_ENUM_FAILED;
							}
						}
					}
				}
			}
		}
	} else {
		DisableRootHubPort(port);
		RootHubPort[port].HubPortStatus = PORT_DEVICE_ENUM_FAILED;
	}

	return TRUE;
}

UINT8 QueryHubPortAttach(void)
{
	static __xdata BOOL res;
	static __xdata UINT8 s;

	res = FALSE; s = ERR_SUCCESS;

	if (UIF_DETECT) {
		UIF_DETECT = 0;
		if (USB_HUB_ST & bUHS_H0_ATTACH) {
			if (RootHubPort[0].HubPortStatus == PORT_DEVICE_NONE /*|| (UHUB0_CTRL & bUH_PORT_EN) == 0x00*/) {
				DisableRootHubPort(0);
				RootHubPort[0].HubPortStatus = PORT_DEVICE_INSERT;
				s = ERR_USB_CONNECT;
				TRACE("hub 0 dev in");
			}
		} else if (RootHubPort[0].HubPortStatus >= PORT_DEVICE_INSERT) {
			DisableRootHubPort(0);
			if (s == ERR_SUCCESS) {
				s = ERR_USB_DISCON;
			}
			TRACE("hub 0 dev out");
		}

		if (USB_HUB_ST & bUHS_H1_ATTACH) {
            TRACE("hub1 attach");
			if (RootHubPort[1].HubPortStatus == PORT_DEVICE_NONE /*|| ( UHUB1_CTRL & bUH_PORT_EN ) == 0x00*/) {
				DisableRootHubPort(1);
				RootHubPort[1].HubPortStatus = PORT_DEVICE_INSERT;
				s = ERR_USB_CONNECT;
				TRACE("hub 1 dev in");
			}
		} else if (RootHubPort[1].HubPortStatus >= PORT_DEVICE_INSERT) {
			DisableRootHubPort(1);
			if (s == ERR_SUCCESS) {
				s = ERR_USB_DISCON;
			}
			TRACE("hub 1 dev out");
		}
	}
	return s;
}

void regrabinterfaces(__xdata USB_HUB_PORT *pUsbHubPort)
{
	static __xdata UINT8 i, s, c;
	static __xdata UINT16 len, cnt;
	static __xdata USB_HUB_PORT *pUsbDevice;

	pUsbDevice = pUsbHubPort;
	if (pUsbDevice->DeviceClass != USB_DEV_CLASS_HUB) {
		for (i = 0; i < pUsbDevice->InterfaceNum; i++) {

			TRACE("");
			//INTERFACE *pInterface = &pUsbDevice->Interface[i];		
			INTERFACE *pInterface = (INTERFACE *)UsbListGetData(pUsbDevice->Interfaces, i);
			TRACE("InterfaceClass=$%x", (UINT16)pInterface->InterfaceClass);
			TRACE("InterfaceProtocol=$%x", (UINT16)pInterface->InterfaceProtocol);

			if (pInterface->InterfaceClass == USB_DEV_CLASS_HID) {
				s = SetIdle(pUsbDevice, 0, 0, i);
				if (s != ERR_SUCCESS) {
					TRACE("SetIdle failed");
				}

				TRACE("Interface %x:", i);
				TRACE("InterfaceProtocol: %x", pInterface->InterfaceProtocol);
				TRACE("Report Size:%d", pInterface->ReportSize);
				s = GetReportDescriptor(pUsbDevice, i, ReceiveDataBuffer, pInterface->ReportSize <= sizeof(ReceiveDataBuffer) ? pInterface->ReportSize : sizeof(ReceiveDataBuffer), &len);

				if (s != ERR_SUCCESS) {
					TRACE("rep descr fail %u", s);
					return; // FALSE;
				}
				TRACE("report descr len:%d", len);
				TRACE("Interface %hx Report Descriptor - ", i);
				DumpHex(ReceiveDataBuffer, len);

				// use default boot mode descriptors if a keyboard or mouse is detected and "advanced USB" is disabled in menu
				if (
					(!Settings.UsbMouseReportMode && pInterface->InterfaceProtocol == HID_PROTOCOL_MOUSE) ||
					(!Settings.UsbKeyboardReportMode && pInterface->InterfaceProtocol == HID_PROTOCOL_KEYBOARD)
					)
				{
					// if it supports boot mode, enable that and use the default descriptor
					if (pInterface->InterfaceSubClass == 0x01) {
						TRACE("set boot mode - %x - ", GetBootProtocol(pUsbDevice, i));
						SetBootProtocol(pUsbDevice, i, 0);
						ParseReportDescriptor(
							pInterface->InterfaceProtocol == HID_PROTOCOL_MOUSE ? StandardMouseDescriptor : StandardKeyboardDescriptor, 
							pInterface->InterfaceProtocol == HID_PROTOCOL_MOUSE ? 50 : 63, 
							pInterface
						);
						TRACE("%x", GetBootProtocol(pUsbDevice, i));
					}
					// Otherwise don't attempt to use this device at all unless advanced USB is enabled
					else{ 
						TRACE("No boot mode");
						continue;
					}
				}
				else
                {
					// only switch into report mode if boot mode is supported (yeah that's the way it works)
					if (pInterface->InterfaceSubClass == 0x01) {
						TRACE("set report mode - %x - ", GetBootProtocol(pUsbDevice, i));
						SetBootProtocol(pUsbDevice, i, 1);
						TRACE("%x", GetBootProtocol(pUsbDevice, i));
					}
					ParseReportDescriptor(ReceiveDataBuffer, len, pInterface);
					DumpHID(pInterface);
				}

				if (pInterface->InterfaceProtocol == HID_PROTOCOL_KEYBOARD)
				{
					UINT8 led;
					led = GetKeyboardLedStatus();
					SetReport(pUsbDevice, i, &led, sizeof(led));
				}
			}
		}
	}
}

void RegrabDeviceReports(UINT8 port)
{
	static __xdata USB_HUB_PORT *pUsbHubPort;
	pUsbHubPort = &RootHubPort[port];

	if (pUsbHubPort->HubPortStatus == PORT_DEVICE_ENUM_SUCCESS) {
		if (pUsbHubPort->DeviceClass != USB_DEV_CLASS_HUB) {
			SelectHubPort(port, EXHUB_PORT_NONE);
			regrabinterfaces(pUsbHubPort);
		} else {
			UINT8 exHubPortNum = pUsbHubPort->HubPortNum;
			for (UINT8 i = 0; i < exHubPortNum; i++) {
				pUsbHubPort = &SubHubPort[port][i];
				if (pUsbHubPort->HubPortStatus == PORT_DEVICE_ENUM_SUCCESS && pUsbHubPort->DeviceClass != USB_DEV_CLASS_HUB) {
					SelectHubPort(port, i);
					regrabinterfaces(pUsbHubPort);
				}
			}
		}
	}
}

void ReenumerateAllPorts(void){
	TRACE("usb: enumerate ports");
	//delayms(150);

	InitUsbData();
	InitUsbMemory();
//	InitPresets();
	
	for (UINT8 i = 0; i < ROOT_HUB_PORT_NUM; i++) {
		TRACE("port %d", i);
		EnumerateRootHubPort(i);
		RegrabDeviceReports(i);
	}
    TRACE("usb: mem %d free %d", UsbMemoryUsed(), UsbMemoryFree());
	TRACE("usb: enumerate ports done");
}

//----------------------------------------------------------------------------------
void DealUsbPort(void) //main function should use it at least 500ms
{
	static __xdata UINT8 s;
	s = QueryHubPortAttach();
    if (forceEnumerate) {
        forceEnumerate = false;
        s = ERR_USB_CONNECT;
    }
	if (s == ERR_USB_CONNECT) {
		ReenumerateAllPorts();
	}
}

void InterruptProcessRootHubPort(UINT8 port)
{
	static __xdata USB_HUB_PORT *pUsbHubPort;
	pUsbHubPort = &RootHubPort[port];

	if (pUsbHubPort->HubPortStatus == PORT_DEVICE_ENUM_SUCCESS)
    {
		if (pUsbHubPort->DeviceClass != USB_DEV_CLASS_HUB)
        {
            // process device on root hub
			SelectHubPort(port, EXHUB_PORT_NONE);
			HIDDataTransferReceive(pUsbHubPort);
		}
        else
        {
            UINT8 exHubPortNum = pUsbHubPort->HubPortNum;
#if 1
			for (UINT8 i = 0; i < exHubPortNum; i++)
            {
                UINT16 hubPortStatus, hubPortChange;
                SelectHubPort(port, EXHUB_PORT_NONE);
                uint8_t s = GetHubPortStatus(pUsbHubPort, i + 1, &hubPortStatus, &hubPortChange);
                if (hubPortChange & 0x0001)
                {
                    TRACE("port %d:%d changed: $%x $%x", port, exHubPortNum, hubPortStatus, hubPortChange);
                    forceEnumerate = true;
                    return;
                }
            }
#endif            
			for (UINT8 i = 0; i < exHubPortNum; i++)
            {
                pUsbHubPort = &SubHubPort[port][i];
                if (pUsbHubPort->HubPortStatus == PORT_DEVICE_ENUM_SUCCESS && pUsbHubPort->DeviceClass != USB_DEV_CLASS_HUB) {
                    SelectHubPort(port, i);
                    HIDDataTransferReceive(pUsbHubPort);
                }
			}
		}
	}
}

void UpdateUsbKeyboardLedInternal(USB_HUB_PORT *pUsbDevice, UINT8 led)
{
	UINT8 i;
	for (i = 0; i < pUsbDevice->InterfaceNum; i++) {
		//INTERFACE *pInterface = &pUsbDevice->Interface[i];
		INTERFACE *pInterface = (INTERFACE *)UsbListGetData(pUsbDevice->Interfaces, i);
		if (pInterface->InterfaceClass == USB_DEV_CLASS_HID) {
			if (pInterface->InterfaceProtocol == HID_PROTOCOL_KEYBOARD) {
				SetReport(pUsbDevice, i, &led, 1);
				//TRACE("led=0x%x", led);
			}
		}
	}
}

//-----------------------------------------------------------------------------
void UpdateUsbKeyboardLed(UINT8 led)
{
	for (UINT8 i = 0; i < ROOT_HUB_PORT_NUM; i++) {
		USB_HUB_PORT *pUsbHubPort = &RootHubPort[i];
		if (pUsbHubPort->HubPortStatus == PORT_DEVICE_ENUM_SUCCESS) {
			if (pUsbHubPort->DeviceClass != USB_DEV_CLASS_HUB) {
				SelectHubPort(i, EXHUB_PORT_NONE);
				UpdateUsbKeyboardLedInternal(pUsbHubPort, led);
			} else {
				int exHubPortNum = pUsbHubPort->HubPortNum;
				for (UINT8 j = 0; j < exHubPortNum; j++) {
					pUsbHubPort = &SubHubPort[i][j];
					if (pUsbHubPort->HubPortStatus == PORT_DEVICE_ENUM_SUCCESS && pUsbHubPort->DeviceClass != USB_DEV_CLASS_HUB) {
						SelectHubPort(i, j);
						UpdateUsbKeyboardLedInternal(pUsbHubPort, led);
					}
				}
			}
		}
	}
}

__xdata BOOL volatile s_CheckUsbPort0 = FALSE;
__xdata BOOL volatile s_CheckUsbPort1 = FALSE;

void ProcessUsbHostPort(void)
{	
	DealUsbPort();
	if (s_CheckUsbPort0) {
		s_CheckUsbPort0 = FALSE;
		InterruptProcessRootHubPort(0);
	}
	if (s_CheckUsbPort1) {
		s_CheckUsbPort1 = FALSE;
		InterruptProcessRootHubPort(1);
	}
}
