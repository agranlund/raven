#ifndef _USBHOST_H_
#define _USBHOST_H_

#include <stdbool.h>
#include <stdint.h>

#define WAIT_USB_TOUT_200US         800

#define ERR_SUCCESS                 0x00
#define ERR_USB_CONNECT             0x15
#define ERR_USB_DISCON              0x16
#define ERR_USB_BUF_OVER            0x17
#define ERR_USB_DISK_ERR            0x1F
#define ERR_USB_TRANSFER            0x20
#define ERR_USB_UNSUPPORT           0xFB
#define ERR_USB_UNKNOWN             0xFE

#ifndef	USB_INT_RET_ACK
#define	USB_INT_RET_ACK		        USB_PID_ACK
#define	USB_INT_RET_NAK		        USB_PID_NAK
#define	USB_INT_RET_STALL	        USB_PID_STALL
#define	USB_INT_RET_DATA0	        USB_PID_DATA0
#define	USB_INT_RET_DATA1	        USB_PID_DATA1
#define	USB_INT_RET_TOUT	        0x00
#define	USB_INT_RET_TOUT1	        0x04
#define	USB_INT_RET_TOUT2	        0x08
#define	USB_INT_RET_TOUT3	        0x0C
#endif

#define ROOT_HUB_PORT_NUM           2           /* number of root hub ports */
#define MAX_EXHUB_PORT_NUM          4           /* maximum number of external hub ports */
#define EXHUB_PORT_NONE             0xff        /* none external hub port */
#define MAX_INTERFACE_NUM           4           /* maximum number of interfaces per devicve */
#define MAX_GLOBAL_INTERFACE_NUM    12          /* maximum number of interfaces across all devices */
#define MAX_ENDPOINT_NUM            4           /* maximum number of endpoints per interface */
#define MAX_EXHUB_LEVEL             1           /* maximum level of external hub */
#define ENDPOINT_OUT                0           /* in */
#define ENDPOINT_IN                 1           /* out */


/* USB PID */
#ifndef USB_PID_SETUP
#define USB_PID_NULL            0x00    /* reserved PID */
#define USB_PID_SOF             0x05
#define USB_PID_SETUP           0x0D
#define USB_PID_IN              0x09
#define USB_PID_OUT             0x01
#define USB_PID_ACK             0x02
#define USB_PID_NAK             0x0A
#define USB_PID_STALL           0x0E
#define USB_PID_DATA0           0x03
#define USB_PID_DATA1           0x0B
#define USB_PID_PRE             0x0C
#endif

/* USB standard device request code */
#ifndef USB_GET_DESCRIPTOR
#define USB_GET_STATUS          0x00
#define USB_CLEAR_FEATURE       0x01
#define USB_SET_FEATURE         0x03
#define USB_SET_ADDRESS         0x05
#define USB_GET_DESCRIPTOR      0x06
#define USB_SET_DESCRIPTOR      0x07
#define USB_GET_CONFIGURATION   0x08
#define USB_SET_CONFIGURATION   0x09
#define USB_GET_INTERFACE       0x0A
#define USB_SET_INTERFACE       0x0B
#define USB_SYNCH_FRAME         0x0C
#endif

/* USB hub class request code */
#ifndef HUB_GET_DESCRIPTOR
#define HUB_GET_STATUS          0x00
#define HUB_CLEAR_FEATURE       0x01
#define HUB_GET_STATE           0x02
#define HUB_SET_FEATURE         0x03
#define HUB_GET_DESCRIPTOR      0x06
#define HUB_SET_DESCRIPTOR      0x07
#endif

/* USB HID class request code */
#ifndef HID_GET_REPORT
#define HID_GET_REPORT          0x01
#define HID_GET_IDLE            0x02
#define HID_GET_PROTOCOL        0x03
#define HID_SET_REPORT          0x09
#define HID_SET_IDLE            0x0A
#define HID_SET_PROTOCOL        0x0B
#endif

/* HID Report Types */
#define HID_REPORT_INPUT        0x01
#define HID_REPORT_OUTPUT       0x02
#define HID_REPORT_FEATURE      0x03


/* Bit define for USB request type */
#ifndef USB_REQ_TYP_MASK
#define USB_REQ_TYP_IN          0x80            /* control IN, device to host */
#define USB_REQ_TYP_OUT         0x00            /* control OUT, host to device */
#define USB_REQ_TYP_READ        0x80            /* control read, device to host */
#define USB_REQ_TYP_WRITE       0x00            /* control write, host to device */
#define USB_REQ_TYP_MASK        0x60            /* bit mask of request type */
#define USB_REQ_TYP_STANDARD    0x00
#define USB_REQ_TYP_CLASS       0x20
#define USB_REQ_TYP_VENDOR      0x40
#define USB_REQ_TYP_RESERVED    0x60
#define USB_REQ_RECIP_MASK      0x1F            /* bit mask of request recipient */
#define USB_REQ_RECIP_DEVICE    0x00
#define USB_REQ_RECIP_INTERF    0x01
#define USB_REQ_RECIP_ENDP      0x02
#define USB_REQ_RECIP_OTHER     0x03
#endif

/* USB request type for hub class request */
#ifndef HUB_GET_HUB_DESCRIPTOR
#define HUB_CLEAR_HUB_FEATURE   0x20
#define HUB_CLEAR_PORT_FEATURE  0x23
#define HUB_GET_BUS_STATE       0xA3
#define HUB_GET_HUB_DESCRIPTOR  0xA0
#define HUB_GET_HUB_STATUS      0xA0
#define HUB_GET_PORT_STATUS     0xA3
#define HUB_SET_HUB_DESCRIPTOR  0x20
#define HUB_SET_HUB_FEATURE     0x20
#define HUB_SET_PORT_FEATURE    0x23
#endif

/* Hub class feature selectors */
#ifndef HUB_PORT_RESET
#define HUB_C_HUB_LOCAL_POWER   0
#define HUB_C_HUB_OVER_CURRENT  1
#define HUB_PORT_CONNECTION     0
#define HUB_PORT_ENABLE         1
#define HUB_PORT_SUSPEND        2
#define HUB_PORT_OVER_CURRENT   3
#define HUB_PORT_RESET          4
#define HUB_PORT_POWER          8
#define HUB_PORT_LOW_SPEED      9
#define HUB_C_PORT_CONNECTION   16
#define HUB_C_PORT_ENABLE       17
#define HUB_C_PORT_SUSPEND      18
#define HUB_C_PORT_OVER_CURRENT 19
#define HUB_C_PORT_RESET        20
#endif

/* USB descriptor type */
#ifndef USB_DESCR_TYP_DEVICE
#define USB_DESCR_TYP_DEVICE    0x01
#define USB_DESCR_TYP_CONFIG    0x02
#define USB_DESCR_TYP_STRING    0x03
#define USB_DESCR_TYP_INTERF    0x04
#define USB_DESCR_TYP_ENDP      0x05
#define USB_DESCR_TYP_QUALIF    0x06
#define USB_DESCR_TYP_SPEED     0x07
#define USB_DESCR_TYP_OTG       0x09
#define USB_DESCR_TYP_HID       0x21
#define USB_DESCR_TYP_REPORT    0x22
#define USB_DESCR_TYP_PHYSIC    0x23
#define USB_DESCR_TYP_CS_INTF   0x24
#define USB_DESCR_TYP_CS_ENDP   0x25
#define USB_DESCR_TYP_HUB       0x29
#endif

/* USB device class */
#ifndef USB_DEV_CLASS_HUB
#define USB_DEV_CLASS_RESERVED  0x00
#define USB_DEV_CLASS_AUDIO     0x01
#define USB_DEV_CLASS_COMMUNIC  0x02
#define USB_DEV_CLASS_HID       0x03
#define USB_DEV_CLASS_MONITOR   0x04
#define USB_DEV_CLASS_PHYSIC_IF 0x05
#define USB_DEV_CLASS_POWER     0x06
#define USB_DEV_CLASS_PRINTER   0x07
#define USB_DEV_CLASS_STORAGE   0x08
#define USB_DEV_CLASS_HUB       0x09
#define USB_DEV_CLASS_VEN_SPEC  0xFF
#endif

/* USB endpoint type and attributes */
#ifndef USB_ENDP_TYPE_MASK
#define USB_ENDP_DIR_MASK       0x80
#define USB_ENDP_ADDR_MASK      0x0F
#define USB_ENDP_TYPE_MASK      0x03
#define USB_ENDP_TYPE_CTRL      0x00
#define USB_ENDP_TYPE_ISOCH     0x01
#define USB_ENDP_TYPE_BULK      0x02
#define USB_ENDP_TYPE_INTER     0x03
#endif

#define USB_PROTOCOL_NONE               0x00

/* HID Protocol Codes */
#define HID_PROTOCOL_NONE               0x00
#define HID_PROTOCOL_KEYBOARD           0x01
#define HID_PROTOCOL_MOUSE              0x02

/* MSC Protocol Codes */
#define MSC_PROTOCOL_CBI_INT            0x00
#define MSC_PROTOCOL_CBI_NOINT          0x01
#define MSC_PROTOCOL_BULK_ONLY          0x50

#ifndef USB_DEVICE_ADDR
#define	USB_DEVICE_ADDR			0x02	/* Ĭ�ϵ�USB�豸��ַ */
#endif
#ifndef DEFAULT_ENDP0_SIZE
#define DEFAULT_ENDP0_SIZE      8       /* default maximum packet size for endpoint 0 */
#endif
#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE         64      /* maximum packet size */
#endif
#ifndef USB_BO_CBW_SIZE
#define USB_BO_CBW_SIZE			0x1F	/* �����CBW���ܳ��� */
#define USB_BO_CSW_SIZE			0x0D	/* ����״̬��CSW���ܳ��� */
#endif
#ifndef USB_BO_CBW_SIG0
#define USB_BO_CBW_SIG0         0x55    /* �����CBWʶ���־'USBC' */
#define USB_BO_CBW_SIG1         0x53
#define USB_BO_CBW_SIG2         0x42
#define USB_BO_CBW_SIG3         0x43
#define USB_BO_CSW_SIG0         0x55    /* ����״̬��CSWʶ���־'USBS' */
#define USB_BO_CSW_SIG1         0x53
#define USB_BO_CSW_SIG2         0x42
#define USB_BO_CSW_SIG3         0x53
#endif

//hid report define
#define HID_LOCAL_ITEM_TAG_USAGE                0x00
#define HID_LOCAL_ITEM_TAG_USAGE_MIN            0x01
#define HID_LOCAL_ITEM_TAG_USAGE_MAX            0x02

#define HID_GLOBAL_ITEM_TAG_USAGE_PAGE          0x00
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM     0x01
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM     0x02
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM    0x03
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM    0x04
#define HID_GLOBAL_ITEM_TAG_REPORT_SIZE         0x07
#define HID_GLOBAL_ITEM_TAG_REPORT_ID           0x08
#define HID_GLOBAL_ITEM_TAG_REPORT_COUNT        0x09

#define HID_MAIN_ITEM_TAG_INPUT                 0x08
#define HID_MAIN_ITEM_TAG_COLLECTION_START      0x0A
#define HID_MAIN_ITEM_TAG_COLLECTION_END        0x0C

#define HID_COLLECTION_APPLICATION              0x01
#define HID_INPUT_VARIABLE                      0x02

#define HID_ITEM_TYPE_MAIN          0
#define HID_ITEM_TYPE_GLOBAL        1
#define HID_ITEM_TYPE_LOCAL         2

#define HID_ITEM_FORMAT_SHORT       0
#define HID_ITEM_FORMAT_LONG        1

#define HID_ITEM_TAG_LONG           15

#define REPORT_USAGE_PAGE           0x04
#define REPORT_USAGE                0x08
#define REPORT_LOCAL_MINIMUM        0x14
#define REPORT_LOCAL_MAXIMUM        0x24
#define REPORT_PHYSICAL_MINIMUM     0x34
#define REPORT_PHYSICAL_MAXIMUM     0x44
#define REPORT_USAGE_MINIMUM        0x18
#define REPORT_USAGE_MAXIMUM        0x28

#define REPORT_UNIT                 0x64
#define REPORT_INPUT                0x80
#define REPORT_OUTPUT               0x90
#define REPORT_FEATURE              0xB0
#define REPORT_REPORT_SIZE          0x74
#define REPORT_REPORT_ID            0x84
#define REPORT_REPORT_COUNT         0x94
#define REPORT_COLLECTION           0xA0
#define REPORT_COLLECTION_END       0xC0

#define REPORT_USAGE_PAGE_NONE      0x00
#define REPORT_USAGE_PAGE_GENERIC   0x01
#define REPORT_USAGE_PAGE_KEYBOARD  0x07
#define REPORT_USAGE_PAGE_LEDS      0x08
#define REPORT_USAGE_PAGE_BUTTON    0x09
#define REPORT_USAGE_PAGE_VENDOR    0xff00

#define REPORT_USAGE_UNKNOWN        0x00
#define REPORT_USAGE_POINTER        0x01
#define REPORT_USAGE_MOUSE          0x02
#define REPORT_USAGE_RESERVED       0x03
#define REPORT_USAGE_JOYSTICK       0x04
#define REPORT_USAGE_GAMEPAD        0x05
#define REPORT_USAGE_KEYBOARD       0x06
#define REPORT_USAGE_KEYPAD         0x07
#define REPORT_USAGE_MULTI_AXIS     0x08
#define REPORT_USAGE_SYSTEM         0x09

#define REPORT_USAGE_X              0x30
#define REPORT_USAGE_Y              0x31
#define REPORT_USAGE_Z              0x32
#define REPORT_USAGE_Rx             0x33
#define REPORT_USAGE_Ry             0x34
#define REPORT_USAGE_Rz             0x35
#define REPORT_USAGE_WHEEL          0x38
#define REPORT_USAGE_HATSWITCH      0x39


//extern uint8_t sInterfacePoolPos;

typedef struct UsbLinkedList {
    uint8_t index;
    __xdata void *data;
    __xdata struct UsbLinkedList *next;
} UsbLinkedList;

extern __xdata UsbLinkedList* UsbListAdd(__xdata UsbLinkedList* head, uint16_t data_size, uint8_t index);
extern void *UsbListGetData(UsbLinkedList* head, uint8_t index);


typedef struct _USB_SETUP_REQ {
    UINT8 bRequestType;
    UINT8 bRequest;
    UINT8 wValueL;
    UINT8 wValueH;
    UINT8 wIndexL;
    UINT8 wIndexH;
    UINT8 wLengthL;
    UINT8 wLengthH;
} USB_SETUP_REQ, *PUSB_SETUP_REQ;

typedef USB_SETUP_REQ __xdata *PXUSB_SETUP_REQ;

typedef struct _DESCR_HEADER
{
	UINT8 bDescLength;
	UINT8 bDescriptorType;	
} DESCR_HEADER;


typedef struct _USB_DEVICE_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bcdUSBL;
    UINT8 bcdUSBH;
    UINT8 bDeviceClass;
    UINT8 bDeviceSubClass;
    UINT8 bDeviceProtocol;
    UINT8 bMaxPacketSize0;
    UINT8 idVendorL;
    UINT8 idVendorH;
    UINT8 idProductL;
    UINT8 idProductH;
    UINT8 bcdDeviceL;
    UINT8 bcdDeviceH;
    UINT8 iManufacturer;
    UINT8 iProduct;
    UINT8 iSerialNumber;
    UINT8 bNumConfigurations;
} USB_DEV_DESCR, *PUSB_DEV_DESCR;

typedef USB_DEV_DESCR __xdata *PXUSB_DEV_DESCR;

typedef struct _USB_CONFIG_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 wTotalLengthL;
    UINT8 wTotalLengthH;
    UINT8 bNumInterfaces;
    UINT8 bConfigurationValue;
    UINT8 iConfiguration;
    UINT8 bmAttributes;
    UINT8 MaxPower;
} USB_CFG_DESCR, *PUSB_CFG_DESCR;

typedef USB_CFG_DESCR __xdata *PXUSB_CFG_DESCR;

typedef struct _USB_INTERF_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bInterfaceNumber;
    UINT8 bAlternateSetting;
    UINT8 bNumEndpoints;
    UINT8 bInterfaceClass;
    UINT8 bInterfaceSubClass;
    UINT8 bInterfaceProtocol;
    UINT8 iInterface;
} USB_ITF_DESCR, *PUSB_ITF_DESCR;

typedef USB_ITF_DESCR __xdata *PXUSB_ITF_DESCR;

typedef struct _USB_ENDPOINT_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bEndpointAddress;
    UINT8 bmAttributes;
    UINT8 wMaxPacketSizeL;
    UINT8 wMaxPacketSizeH;
    UINT8 bInterval;
} USB_ENDP_DESCR, *PUSB_ENDP_DESCR;

typedef USB_ENDP_DESCR __xdata *PXUSB_ENDP_DESCR;

typedef struct _USB_CONFIG_DESCR_LONG {
    USB_CFG_DESCR   cfg_descr;
    USB_ITF_DESCR   itf_descr;
    USB_ENDP_DESCR  endp_descr[1];
} USB_CFG_DESCR_LONG, *PUSB_CFG_DESCR_LONG;

typedef USB_CFG_DESCR_LONG __xdata *PXUSB_CFG_DESCR_LONG;

typedef struct _USB_HUB_DESCR {
    UINT8 bDescLength;
    UINT8 bDescriptorType;
    UINT8 bNbrPorts;
    UINT8 wHubCharacteristicsL;
    UINT8 wHubCharacteristicsH;
    UINT8 bPwrOn2PwrGood;
    UINT8 bHubContrCurrent;
    UINT8 DeviceRemovable;
    UINT8 PortPwrCtrlMask;
} USB_HUB_DESCR, *PUSB_HUB_DESCR;

typedef USB_HUB_DESCR __xdata *PXUSB_HUB_DESCR;

typedef struct _USB_HID_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bcdHIDL;
    UINT8 bcdHIDH;
    UINT8 bCountryCode;
    UINT8 bNumDescriptors;
    UINT8 bDescriptorTypeX;
    UINT8 wDescriptorLengthL;
    UINT8 wDescriptorLengthH;
} USB_HID_DESCR, *PUSB_HID_DESCR;

typedef USB_HID_DESCR __xdata *PXUSB_HID_DESCR;

typedef struct _UDISK_BOC_CBW {         /* command of BulkOnly USB-FlashDisk */
    UINT8 mCBW_Sig0;
    UINT8 mCBW_Sig1;
    UINT8 mCBW_Sig2;
    UINT8 mCBW_Sig3;
    UINT8 mCBW_Tag0;
    UINT8 mCBW_Tag1;
    UINT8 mCBW_Tag2;
    UINT8 mCBW_Tag3;
    UINT8 mCBW_DataLen0;
    UINT8 mCBW_DataLen1;
    UINT8 mCBW_DataLen2;
    UINT8 mCBW_DataLen3;                /* uppest byte of data length, always is 0 */
    UINT8 mCBW_Flag;                    /* transfer direction and etc. */
    UINT8 mCBW_LUN;
    UINT8 mCBW_CB_Len;                  /* length of command block */
    UINT8 mCBW_CB_Buf[16];              /* command block buffer */
} UDISK_BOC_CBW, *PUDISK_BOC_CBW;

typedef UDISK_BOC_CBW __xdata *PXUDISK_BOC_CBW;

typedef struct _UDISK_BOC_CSW {         /* status of BulkOnly USB-FlashDisk */
    UINT8 mCSW_Sig0;
    UINT8 mCSW_Sig1;
    UINT8 mCSW_Sig2;
    UINT8 mCSW_Sig3;
    UINT8 mCSW_Tag0;
    UINT8 mCSW_Tag1;
    UINT8 mCSW_Tag2;
    UINT8 mCSW_Tag3;
    UINT8 mCSW_Residue0;                /* return: remainder bytes */
    UINT8 mCSW_Residue1;
    UINT8 mCSW_Residue2;
    UINT8 mCSW_Residue3;                /* uppest byte of remainder length, always is 0 */
    UINT8 mCSW_Status;                  /* return: result status */
} UDISK_BOC_CSW, *PUDISK_BOC_CSW;

typedef UDISK_BOC_CSW __xdata *PXUDISK_BOC_CSW;

typedef struct _ENDPOINT
{
	uint8_t  EndpointAddr;
	uint16_t MaxPacketSize;
	uint8_t  EndpointDir : 1;
	uint8_t  TOG : 1;
} ENDPOINT, *PENDPOINT;

typedef struct _HID_ITEM_INFO
{
	unsigned int format;
	uint8_t size;
	uint8_t type;
	uint8_t tag;

	union
	{
		uint8_t  u8;
		int8_t   s8;
		uint16_t u16;
		int16_t  s16;
		uint32_t u32;
		int32_t  s32;

		const uint8_t *longdata;
	} value;
	
} HID_ITEM;

typedef struct _HID_GLOBAL
{
	uint8_t  usagePage;
	int32_t  logicalMinimum;
	uint32_t logicalMaximum;
	int8_t   physicalMinimum;
	int8_t   physicalMaximum;
	uint8_t  unitExponent;
	uint8_t  unit;
	uint8_t  reportID;
	uint8_t  reportSize;
	uint8_t  reportCount;
} HID_GLOBAL;

typedef struct _HID_LOCAL
{
	uint32_t usage;
	uint32_t usageMin;
	uint32_t usageMax;
} HID_LOCAL;


//hid segement define
#define HID_SEG_KEYBOARD_MODIFIER_INDEX  0
#define HID_SEG_KEYBOARD_VAL_INDEX       1
#define HID_SEG_BUTTON_INDEX             2
#define HID_SEG_X_INDEX                  3
#define HID_SEG_Y_INDEX                  4
#define HID_SEG_WHEEL_INDEX              5
#define HID_SEG_NUM                      6

#define MAX_USAGE_NUM                   10
#define MAX_REPORTS                     18

//interface struct
typedef struct _INTERFACE
{
	uint8_t  InterfaceClass;
	uint8_t	 InterfaceSubClass;
	uint8_t  InterfaceProtocol;
	uint16_t ReportSize;
	uint8_t  EndpointNum;   //number of endpoints in this interface
	ENDPOINT Endpoint[MAX_ENDPOINT_NUM]; //endpoints
	bool     usesReports;
	__xdata UsbLinkedList *Reports;

} INTERFACE, *PINTERFACE;
extern void InitInterface(INTERFACE* Interface);

//hub struct
#define PORT_DEVICE_NONE            0
#define PORT_DEVICE_INSERT          1
#define PORT_DEVICE_ENUM_FAILED     2
#define PORT_DEVICE_ENUM_SUCCESS    3

#define DEVICE_SPEED_LOW            0
#define DEVICE_SPEED_FULL           1


typedef struct _USB_HUB_PORT
{
	uint8_t       HubPortStatus;
	uint8_t       HubPortNum;
	uint8_t       DeviceClass;
	uint8_t       MaxPacketSize0;
	uint16_t      VendorID;
	uint16_t      ProductID;
	uint16_t      bcdDevice;
	uint8_t       DeviceAddress;
	uint8_t       DeviceSpeed;
	uint8_t       InterfaceNum;
	__xdata UsbLinkedList*  Interfaces;
} USB_HUB_PORT, *USB_PHUB_PORT;

extern void InitUsbHost(void);
extern void InitUsbData(void);
extern void DealUsbPort(void);
extern void InterruptProcessRootHubPort(uint8_t port_index);
extern void UpdateUsbKeyboardLed(uint8_t led);
extern INTERFACE* AllocInterface(uint8_t count);
extern void ReenumerateAllPorts(void);
extern void ProcessUsbHostPort(void);

extern __xdata bool volatile s_CheckUsbPort0;
extern __xdata bool volatile s_CheckUsbPort1;


UINT8 EnableRootHubPort(UINT8 rootHubIndex);
void DisableRootHubPort(UINT8 RootHubIndex);
void SetHostUsbAddr(UINT8 addr);
void SetUsbSpeed(UINT8 FullSpeed);
void ResetRootHubPort(UINT8 RootHubIndex);
void SelectHubPort(UINT8 RootHubIndex, UINT8 HubPortIndex);
void InitUsbHost(void);
UINT8 USBHostTransact(UINT8 endp_pid, UINT8 tog, UINT16 timeout);
UINT8 HostCtrlTransfer(USB_SETUP_REQ *pSetupReq, UINT8 MaxPacketSize0, PUINT8 DataBuf, PUINT16 RetLen);
UINT8 TransferReceive(ENDPOINT *pEndPoint, UINT8 *pData, UINT16 *pRetLen, UINT16 timeout);
void InitHubPortData(USB_HUB_PORT *pUsbHubPort);
void InitRootHubPortData(UINT8 rootHubIndex);

extern USB_HUB_PORT __xdata RootHubPort[ROOT_HUB_PORT_NUM];
extern USB_HUB_PORT __xdata SubHubPort[ROOT_HUB_PORT_NUM][MAX_EXHUB_PORT_NUM];


#endif

