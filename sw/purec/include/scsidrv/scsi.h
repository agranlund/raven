/*{{{}}}*/
/*********************************************************************
 *
 * SCSI-Aufrufe fÅr alle GerÑte
 *
 * $Source: U:\USR\src\scsi\CBHD\include\scsidrv\RCS\scsi.h,v $
 *
 * $Revision: 1.2 $
 *
 * $Author: Steffen_Engel $
 *
 * $Date: 1996/02/14 11:33:52 $
 *
 * $State: Exp $
 *
 **********************************************************************
 * History:
 *
 * $Log: scsi.h,v $
 * Revision 1.2  1996/02/14  11:33:52  Steffen_Engel
 * keine globalen Kommandostrukturen mehr
 *
 * Revision 1.1  1995/11/28  19:14:14  S_Engel
 * Initial revision
 *
 *
 *
 *********************************************************************/

#ifndef __SCSI_H
#define __SCSI_H

#include <portab.h>
#include "scsidrv/scsidefs.h"           /* Typen fÅr SCSI-Lib */


/*****************************************************************************
 * Konstanten
 *****************************************************************************/

#define DIRECTACCESSDEV  0       /* GerÑt mit Direktzugriff (Festplatte) */
#define SEQACCESSDEV     1       /*   "    "  seq. Zugriff  (Streamer)   */
#define PRINTERDEV       2       /* Drucker                              */
#define PROCESSORDEV     3       /* Hostadapter                          */
#define WORMDEV          4       /* WORM-Laufwerk                        */
#define ROMDEV           5       /* nur-lese Laufwerk (CD-ROM)           */
#define SCANNERDEF       6       /* Scanner                              */
#define OPTICALMEMDEV    7       /* optical memory device                */
#define MEDIUMCHNGDEV    8       /* medium changer device (zB JukeBox)   */
#define COMMDEV          9       /* Communicationdevice                  */
#define GRAPHDEV1       10
#define GRAPHDEV2       11
#define UNKNOWNDEV      31


/*
        SCSI opcodes
*/

#define TEST_UNIT_READY         0x00
#define REZERO_UNIT             0x01
#define REQUEST_SENSE           0x03
#define FORMAT_UNIT             0x04
#define READ_BLOCK_LIMITS       0x05
#define REASSIGN_BLOCKS         0x07
#define READ_6                  0x08
#define WRITE_6                 0x0a
#define SEEK_6                  0x0b
#define READ_REVERSE            0x0f
#define WRITE_FILEMARKS         0x10
#define SPACE                   0x11
#define INQUIRY                 0x12
#define RECOVER_BUFFERED_DATA   0x14
#define MODE_SELECT             0x15
#define RESERVE                 0x16
#define RELEASE                 0x17
#define COPY                    0x18
#define ERASE                   0x19
#define MODE_SENSE              0x1a
#define START_STOP              0x1b
#define RECEIVE_DIAGNOSTIC      0x1c
#define SEND_DIAGNOSTIC         0x1d
#define ALLOW_MEDIUM_REMOVAL    0x1e

#define READ_FORMAT_CAPACITIES  0x23
#define SET_WINDOW              0x24
#define READ_CAPACITY           0x25
#define READ_10                 0x28
#define WRITE_10                0x2a
#define SEEK_10                 0x2b
#define WRITE_VERIFY            0x2e
#define VERIFY                  0x2f
#define SEARCH_HIGH             0x30
#define SEARCH_EQUAL            0x31
#define SEARCH_LOW              0x32
#define SET_LIMITS              0x33
#define PRE_FETCH               0x34
#define READ_POSITION           0x34
#define SYNCHRONIZE_CACHE       0x35
#define LOCK_UNLOCK_CACHE       0x36
#define READ_DEFECT_DATA        0x37
#define MEDIUM_SCAN             0x38
#define COMPARE                 0x39
#define COPY_VERIFY             0x3a
#define WRITE_BUFFER            0x3b
#define READ_BUFFER             0x3c
#define UPDATE_BLOCK            0x3d
#define READ_LONG               0x3e
#define WRITE_LONG              0x3f
#define CHANGE_DEFINITION       0x40
#define WRITE_SAME              0x41
#define UNMAP		            0x42
#define READ_TOC                0x43
#define READ_HEADER             0x44
#define GET_EVENT_STATUS_NOTIFICATION 0x4a
#define LOG_SELECT              0x4c
#define LOG_SENSE               0x4d
#define XDWRITEREAD_10          0x53
#define MODE_SELECT_10          0x55
#define RESERVE_10				0x56
#define RELEASE_10				0x57
#define MODE_SENSE_10           0x5a
#define PERSISTENT_RESERVE_IN	0x5e
#define PERSISTENT_RESERVE_OUT	0x5f
#define VARIABLE_LENGTH_CMD		0x7f
#define EXTENDED_COPY       	0x83
#define RECEIVE_COPY_RESULTS	0x84
#define ACCESS_CONTROL_IN   	0x86
#define ACCESS_CONTROL_OUT  	0x87
#define READ_16             	0x88
#define COMPARE_AND_WRITE   	0x89
#define WRITE_16            	0x8a
#define READ_ATTRIBUTE      	0x8c
#define WRITE_ATTRIBUTE	    	0x8d
#define VERIFY_16				0x8f
#define SYNCHRONIZE_CACHE_16	0x91
#define WRITE_SAME_16			0x93
#define SERVICE_ACTION_BIDIRECTIONAL 0x9d
#define SERVICE_ACTION_IN_16	0x9e
#define SERVICE_ACTION_OUT_16	0x9f
#define REPORT_LUNS				0xa0
#define SECURITY_PROTOCOL_IN	0xa2
#define MAINTENANCE_IN      	0xa3
#define MAINTENANCE_OUT			0xa4
#define MOVE_MEDIUM             0xa5
#define EXCHANGE_MEDIUM     	0xa6
#define READ_12             	0xa8
#define SERVICE_ACTION_OUT_12	0xa9
#define WRITE_12                0xaa
#define READ_MEDIA_SERIAL_NUMBER 0xab /* Obsolete with SPC-2 */
#define SERVICE_ACTION_IN_12	0xab
#define WRITE_VERIFY_12         0xae
#define VERIFY_12	      		0xaf
#define SEARCH_HIGH_12          0xb0
#define SEARCH_EQUAL_12         0xb1
#define SEARCH_LOW_12           0xb2
#define SECURITY_PROTOCOL_OUT	0xb5
#define SEND_VOLUME_TAG         0xb6
#define READ_ELEMENT_STATUS     0xb8
#define WRITE_LONG_2            0xea

/*
 *  SCSI Architecture Model (SAM) Status codes. Taken from SAM-3 draft
 *  T10/1561-D Revision 4 Draft dated 7th November 2002.
 */
#define SAM_STAT_GOOD            0x00
#define SAM_STAT_CHECK_CONDITION 0x02
#define SAM_STAT_CONDITION_MET   0x04
#define SAM_STAT_BUSY            0x08
#define SAM_STAT_INTERMEDIATE    0x10
#define SAM_STAT_INTERMEDIATE_CONDITION_MET 0x14
#define SAM_STAT_RESERVATION_CONFLICT 0x18
#define SAM_STAT_COMMAND_TERMINATED 0x22	/* obsolete in SAM-3 */
#define SAM_STAT_TASK_SET_FULL   0x28
#define SAM_STAT_ACA_ACTIVE      0x30
#define SAM_STAT_TASK_ABORTED    0x40

/* values for service action in */
#define	SAI_READ_CAPACITY_16  0x10
#define SAI_GET_LBA_STATUS    0x12
#define SAI_REPORT_REFERRALS  0x13

/* values for VARIABLE_LENGTH_CMD service action codes
 * see spc4r17 Section D.3.5, table D.7 and D.8 */
#define VLC_SA_RECEIVE_CREDENTIAL 0x1800

/* values for maintenance in */
#define MI_REPORT_IDENTIFYING_INFORMATION 0x05
#define MI_REPORT_TARGET_PGS  0x0a
#define MI_REPORT_ALIASES     0x0b
#define MI_REPORT_SUPPORTED_OPERATION_CODES 0x0c
#define MI_REPORT_SUPPORTED_TASK_MANAGEMENT_FUNCTIONS 0x0d
#define MI_REPORT_PRIORITY    0x0e
#define MI_REPORT_TIMESTAMP   0x0f
#define MI_MANAGEMENT_PROTOCOL_IN 0x10

/* value for MI_REPORT_TARGET_PGS ext header */
#define MI_EXT_HDR_PARAM_FMT  0x20

/* values for maintenance out */
#define MO_SET_IDENTIFYING_INFORMATION 0x06
#define MO_SET_TARGET_PGS     0x0a
#define MO_CHANGE_ALIASES     0x0b
#define MO_SET_PRIORITY       0x0e
#define MO_SET_TIMESTAMP      0x0f
#define MO_MANAGEMENT_PROTOCOL_OUT 0x10

/* values for variable length command */
#define XDREAD_32	      0x03
#define XDWRITE_32	      0x04
#define XPWRITE_32	      0x06
#define XDWRITEREAD_32	      0x07
#define READ_32		      0x09
#define VERIFY_32	      0x0a
#define WRITE_32	      0x0b
#define WRITE_SAME_32	      0x0d


/* Values for T10/04-262r7 */
#define	ATA_16		      0x85	/* 16-byte pass-thru */
#define	ATA_12		      0xa1	/* 12-byte pass-thru */

/* Vendor specific CDBs start here */
#define VENDOR_SPECIFIC_CDB 0xc0

/*
 *  SENSE KEYS
 */
#define NO_SENSE            0x00
#define RECOVERED_ERROR     0x01
#define NOT_READY           0x02
#define MEDIUM_ERROR        0x03
#define HARDWARE_ERROR      0x04
#define ILLEGAL_REQUEST     0x05
#define UNIT_ATTENTION      0x06
#define DATA_PROTECT        0x07
#define BLANK_CHECK         0x08
#define VENDOR_SPECIFIC		0x09
#define COPY_ABORTED        0x0a
#define ABORTED_COMMAND     0x0b
#define VOLUME_OVERFLOW     0x0d
#define MISCOMPARE          0x0e
#define COMPLETED			0x0f


/*
 *  DEVICE TYPES
 */

#define TYPE_DISK           0x00
#define TYPE_TAPE           0x01
#define TYPE_PRINTER        0x02
#define TYPE_PROCESSOR      0x03    /* HP scanners use this */
#define TYPE_WORM           0x04
#define TYPE_ROM            0x05
#define TYPE_SCANNER        0x06
#define TYPE_MOD            0x07    /* Magneto-optical disk - treated as TYPE_DISK */
#define TYPE_MEDIUM_CHANGER 0x08
#define TYPE_COMM           0x09    /* Communications device */
#define TYPE_RAID           0x0c
#define TYPE_ENCLOSURE      0x0d    /* Enclosure Services Device */
#define TYPE_RBC			0x0e	/* Simplified Direct Access */
#define TYPE_OCR			0x0f	/* Optical Card Reader */
#define TYPE_BRIDGE			0x10	/* Bridge Controller */
#define TYPE_OSD            0x11	/* Object-based Storage */
#define TYPE_ADC			0x12	/* Automation/Drive Interface */
#define TYPE_SECURITY		0x13
#define TYPE_ZBC            0x14	/* Zoned Block Commands */
#define TYPE_WLUN           0x1e    /* well-known logical unit */
                         /* 0x1f       unknown or no device type */
#define TYPE_NO_LUN         0x7f


/*****************************************************************************
 * Typen
 *****************************************************************************/

/* Inquiry-Struktur */
typedef struct
{
  UCHAR Device;
  UCHAR Qualifier;
  UCHAR Version;
  UCHAR Format;
  UCHAR AddLen;
  UCHAR Res1;
  UWORD Res2;
  char  Vendor[8];
  char  Product[16];
  char  Revision[4];
}tInqData;

/* Modesense/select-Typen */

/* Pages fÅr CD-ROMS */
/* {{{ */
typedef struct{
  BYTE CDP0DRes2;
  BYTE InactTMul;      /* unteres Nibble */
  UWORD SperMSF;
  UWORD FperMSF;
}tCDPage0D;

typedef struct {
  UBYTE ImmedFlags;
  BYTE CD0ERes3;
  BYTE CD0ERes4;
  UBYTE LBAFlags;
  UWORD BlocksPerSecond;
    /* Genau:
     *   LBAFlags MOD 10H = 0 -> BlocksPerSecond
     *   LBAFlags MOD 10H = 8 -> 256 * BlocksPerSecond
     */
  UBYTE Port0Channel;
  UBYTE Port0Volume;
  UBYTE Port1Channel;
  UBYTE Port1Volume;
  UBYTE Port2Channel;
  UBYTE Port2Volume;
  UBYTE Port3Channel;
  UBYTE Port3Volume;
}tCDPage0E;
/* }}} */

/* allgmeine Struktur fÅr ModeSense/Select */
typedef struct{
  BYTE ModeLength;
  BYTE MediumType;
  UBYTE DeviceSpecs;  /* GerÑteabhÑngig */
  BYTE BlockDescLen;
} tParmHead;

typedef struct{
  ULONG Blocks;                  /* Byte HH = DensityCode */
  ULONG BlockLen;                /* Byte HH = Reserved    */
} tBlockDesc;

/* die Varianten fÅr die Pages */
typedef union{
  tCDPage0D CDP0D;
  tCDPage0E CDP0E;
} tPage;

typedef struct{
  tParmHead ParmHead;
  tBlockDesc BlockDesc;
  tPage Page;
} tModePage;


/*****************************************************************************
 * Variablen
 *****************************************************************************/
extern long ScsiFlags;   /* Wert fÅr tScsiCmd.Flags */


/*****************************************************************************
 * Funktionen
 *****************************************************************************/

LONG TestUnitReady(void);


LONG Inquiry(void  *data, BOOLEAN Vital, UWORD VitalPage, WORD length);
  /* Inquiry von einem GerÑt abholen */

#define MODESEL_SMP 0x01            /* Save Mode Parameters */
#define MODESEL_PF  0x10            /* Page Format          */

LONG ModeSelect(UWORD        SelectFlags,
                void        *Buffer,
                UWORD        ParmLen);

#define MODESENSE_CURVAL 0          /* current values     */
#define MODESENSE_CHANGVAL 1        /* changeable values  */
#define MODESENSE_DEFVAL 2          /* default values     */
#define MODESENSE_SAVEDVAL 3        /* save values        */

LONG ModeSense(UWORD     PageCode,
               UWORD     PageControl,
               void     *Buffer,
               UWORD     ParmLen);


LONG PreventMediaRemoval(BOOLEAN Prevent);


BOOLEAN init_scsi (void);
  /* Initialisierung des Moduls */



/*-------------------------------------------------------------------------*/
/*-                                                                       -*/
/*- Allgemeine Tools                                                      -*/
/*-                                                                       -*/
/*-------------------------------------------------------------------------*/
void SuperOn(void);

void SuperOff(void);

void Wait(ULONG Ticks);

void SetBlockSize(ULONG NewLen);
  /*
   * SetBlockLen legt die BlocklÑnge fÅr das SCSI-GerÑt fest
   * (normalerweise 512 Bytes).
   */

ULONG GetBlockSize();
  /*
   * GetBlockLen gibt die aktuell eingestellte BlocklÑnge zurÅck.
   */


void SetScsiUnit(tHandle handle, WORD Lun, ULONG MaxLen);
  /*
   * SetScsiUnit legt das GerÑt fest an das die nachfolgenden Kommandos
   * gesendet werden und wie lang die Transfers maximal sein dÅrfen.
   */


/*-------------------------------------------------------------------------*/
/*-                                                                       -*/
/*- Zugriff fÅr Submodule (ScsiStreamer, ScsiCD, ScsiDisk...)             -*/
/*-                                                                       -*/
/*-------------------------------------------------------------------------*/

typedef struct
{
  UBYTE     Command;
  BYTE      LunAdr;
  UWORD     Adr;
  UBYTE     Len;
  BYTE      Flags;
}tCmd6;

typedef struct
{
  UBYTE     Command;
  BYTE      Lun;
  ULONG     Adr;
  BYTE      Reserved;
  UBYTE     LenHigh;
  UBYTE     LenLow;
  BYTE      Flags;
}tCmd10;

typedef struct
{
  UBYTE     Command;
  BYTE      Lun;
  ULONG     Adr;
  ULONG     Len;
  BYTE      Reserved;
  BYTE      Flags;
}tCmd12;


extern ULONG    BlockLen;
extern ULONG    MaxDmaLen;
extern UWORD    LogicalUnit;

void SetCmd6(tCmd6 *Cmd,
             UWORD Opcode,
             ULONG BlockAdr,
             UWORD TransferLen);

void SetCmd10(tCmd10 *Cmd,
              UWORD Opcode,
              ULONG BlockAdr,
              UWORD TransferLen);
         
void SetCmd12(tCmd12 *Cmd,
              UWORD Opcode,
              ULONG BlockAdr,
              ULONG TransferLen);

tpSCSICmd SetCmd(BYTE    *Cmd,
                 WORD     CmdLen,
                 void    *Buffer,
                 ULONG    Len,
                 ULONG   TimeOut);


#endif

