#ifndef __scsi3_h
#define __scsi3_h


typedef struct {
	unsigned int peripheralQualifier : 3;
	unsigned int deviceType : 5;
	unsigned int RMB : 1;
	unsigned int deviceTypeModifier : 7;
	unsigned int ISOVersion : 2;
	unsigned int ECMAVersion : 3;
	unsigned int ANSIVersion : 3;
	unsigned int AENC : 1;
	unsigned int TrmIOP : 1;
	unsigned int NormACA : 1;
	unsigned int HiSupport : 1;
	unsigned int responseDataFormat : 4;
	unsigned int additionalLength:8;
	unsigned int SCCS:1;
	unsigned int reserved:7;
	unsigned int BQue:1;
	unsigned int EncServ:1;
	unsigned int VendorSpecific:1;
	unsigned int MultiP:1;
	unsigned int MChangr:1;
	unsigned int AckReqQ:1;
	unsigned int ADR32:1;
	unsigned int ADR16:1;
	unsigned int RelAdr : 1;
	unsigned int WBus32 : 1;
	unsigned int WBus16 : 1;
	unsigned int Sync : 1;
	unsigned int Linked : 1;
	unsigned int TrnDis : 1;
	unsigned int CmdQue : 1;
	unsigned int SoftReset : 1;
	unsigned char vendor[8];
	unsigned char product[16];
	unsigned char revision[4];
	/* below optional */
	unsigned char serial[8];
	unsigned char unusedVendorSpecific[12];
	unsigned char reserved2[40];
	unsigned char copyright[48];
	unsigned char distributionSerial[4];
} INQUIRY_DATA;

typedef struct {
	unsigned int PS : 1;
	unsigned int : 1;
	unsigned int pageCode : 6;
	unsigned int pageLength : 8;
	unsigned int : 3;
	unsigned int DUA : 1;
	unsigned int : 4;
	unsigned int : 8;
} PAGE_0;

typedef struct {
	unsigned int PS : 1;
	unsigned int : 1;
	unsigned int pageCode : 6;
	unsigned int pageLength : 8;
	unsigned int tracksPerZone;
	unsigned int altSectorsPerZone;
	unsigned int altTracksPerZone;
	unsigned int altTracksPerLUN;
	unsigned int sectorsPerTrack;
	unsigned int bytesPerSector;
	unsigned int interleave;
	unsigned int trackSkewFactor;
	unsigned int cylinderSkewFactor;
	unsigned int SSEC : 1;
	unsigned int HSEC : 1;
	unsigned int RMB : 1;
	unsigned int SURF : 1;
	unsigned int : 4;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
} PAGE_3;

typedef struct {
	unsigned int PS : 1;
	unsigned int : 1;
	unsigned int pageCode : 6;
	unsigned int pageLength : 8;
	unsigned char cylinders[3];
	unsigned char heads;
	unsigned char preComp[3];
	unsigned char redWrite[3];
	unsigned int stepRate;
	unsigned char landingZone[3];
	unsigned int : 6;
	unsigned int RPL : 2;
	unsigned int rotationalOffset : 8;
	unsigned int : 8;
	unsigned int rotationRate;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
} PAGE_4;

typedef struct {
	unsigned int PS : 1;
	unsigned int : 1;
	unsigned int pageCode : 6;
	unsigned int pageLength : 8;
	unsigned int : 5;
	unsigned int WCE : 1;
	unsigned int MF : 1;
	unsigned int RCD : 1;
	unsigned int readRetention : 4;
	unsigned int writeRetention : 4;
	unsigned int disablePrefetch;
	unsigned int minimumPrefetch;
	unsigned int maximumPrefetch;
	unsigned int maximumPrefetchCeiling;
} PAGE_8;

typedef struct {
	unsigned int PS : 1;
	unsigned int : 1;
	unsigned int pageCode : 6;
	unsigned int pageLength : 8;
	unsigned int dhs : 1;
	unsigned int scrub : 1;
	unsigned int vsc : 1;
	unsigned int ftme : 1;
	unsigned int ridi : 1;
	unsigned int offtr : 1;
	unsigned int : 1;
	unsigned int phsk : 1;
	unsigned int spinDownTimer : 8;
	unsigned int headParkTimer : 8;
	unsigned int : 4;
	unsigned int seekRetryCount : 4;
} PAGE_47;

typedef struct {
	unsigned int PS : 1;
	unsigned int : 1;
	unsigned int pageCode : 6;
	unsigned int pageLength : 8;
	unsigned int DIO : 1;
	unsigned int DII : 1;
	unsigned int FDB : 1;
	unsigned int RUEE : 1;
	unsigned int FDPE : 1;
	unsigned int : 1;
	unsigned int DUA : 1;
	unsigned int DRT : 1;
	unsigned int DDIS : 1;
	unsigned int DELDIS : 1;
	unsigned int : 1;
	unsigned int DP : 1;
	unsigned int SSID : 1;
	unsigned int SCSIADR : 3;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
} PAGE_57;

typedef struct {
	unsigned char density;
	unsigned char res1[4];
	unsigned char bytesPerSector[3];
	unsigned char formatCode;
	unsigned char cylinders[2];
	unsigned char heads;
	unsigned char redWrite[2];
	unsigned char preComp[2];
	unsigned char landingZone;
	unsigned char stepRate;
	unsigned char flags;
	unsigned char sectorsPerTrack;
} PAGE_RLL;

typedef struct {
	unsigned int opcode : 8;
	unsigned int lun : 3;
	unsigned int vendor : 5;
	unsigned int reserved1 : 8;
	unsigned int reserved2 : 8;
	unsigned int length : 8;
	unsigned int : 8;
} CMD_BLK;

typedef struct {
	unsigned int opcode : 8;
	unsigned int lun : 3;
	unsigned int fmtData : 1;
	unsigned int cmpLst : 1;
	unsigned int defectListFormat : 3;
	unsigned int vendor : 8;
	unsigned int interleaveMSB : 8;
	unsigned int interleaveLSB : 8;
	unsigned int : 8;
} FORMAT_BLK;

typedef struct {
	unsigned int opcode : 8;
	unsigned int lun : 3;
	unsigned int flags : 5;
	unsigned int PC : 2;
	unsigned int pagecode : 6;
	unsigned int reserved3 : 8;
	unsigned int length : 8;
	unsigned int vu : 1;
	unsigned int : 5;
	unsigned int flag : 1;
	unsigned int link : 1;
} SENSE_BLK;

typedef struct {
	unsigned int opcode : 8;
	unsigned int lun : 3;
	unsigned int flags : 5;
	unsigned int PC : 2;
	unsigned int pagecode : 6;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int length_lo : 8;
	unsigned int vu : 1;
	unsigned int : 5;
	unsigned int flag : 1;
	unsigned int link : 1;
} SENSE_10_BLK;

typedef struct {
	unsigned int opcode : 8;
	unsigned int lun : 3;
	unsigned int pf : 1;
	unsigned int reserved : 3;
	unsigned int sp : 1;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int length : 8;
	unsigned int vu : 1;
	unsigned int : 5;
	unsigned int flag : 1;
	unsigned int link : 1;
} SELECT_BLK;

typedef struct {
	unsigned int opcode : 8;
	unsigned int lun : 3;
	unsigned int pf : 1;
	unsigned int reserved : 3;
	unsigned int sp : 1;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int : 8;
	unsigned int length_lo : 8;
	unsigned int vu : 1;
	unsigned int : 5;
	unsigned int flag : 1;
	unsigned int link : 1;
} SELECT_10_BLK;

typedef struct {
	unsigned int valid : 1;
	unsigned int errorCode : 7;
	unsigned int segmentNumber : 8;
	unsigned int fileMark : 1;
	unsigned int EOM : 1;
	unsigned int ILI : 1;
	unsigned int : 1;
	unsigned int senseKey : 4;
	unsigned int InformationByte0 : 8;
	unsigned int InformationByte1 : 8;
	unsigned int InformationByte2 : 8;
	unsigned int InformationByte3 : 8;
	unsigned int addSenseLength : 8;
	unsigned int commandSpecific0 : 8;
	unsigned int commandSpecific1 : 8;
	unsigned int commandSpecific2 : 8;
	unsigned int commandSpecific3 : 8;
	unsigned int addSenseCode : 8;
	unsigned int addSenseCodeQualifier : 8;
	unsigned int FieldReplaceableUnitCode : 8;
	unsigned int SKSV : 1;
	unsigned int senseKeySpecific0 : 7;
	unsigned int senseKeySpecific1 : 8;
	unsigned int senseKeySpecific2 : 8;
	/* unsigned char addSenseData[244]; */
} SENSE_DATA;

#endif /* __scsi3_h */
