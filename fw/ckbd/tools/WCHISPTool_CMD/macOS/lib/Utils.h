/*
 * Header file for varible type
 *
 * Copyright (C) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
 * Web: http://wch.cn
 * Author: tech@wch.cn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */



#ifndef Utils_h
#define Utils_h

#include "macOSTypes.h"
#include "WCH55XISPDLL.h"

// Convert 2 characters to 1 byte hexadecimal value
BOOL Char2ToHex(PCHAR  pInChar,    // 2 character buffers to be converted
                PUCHAR pOutChar);  // converted hexadecimal value

// Convert string to integer
UINT mTStrToHEX(PCHAR str);        // string to be converted

// Convert HEX format file to BIN format
BOOL HexToBin(ULONG StartAddr,     // file start address
                PVOID Hexbuf,      // HEX file buffer
                ULONG  iHexBufLen, // length of HEX file buffer
                PVOID  Binbuf,     // length of BIN fle buffer
                PULONG iBinLen );  // length of BIN file buffer

// Read all data to buffer from download file, it should be converted to BIN file if it is HEX file. The downloaded data
// is aligned with 4K length, and 0xff is filled for the part less then 4096.
BOOL ReadDataFromDnFileB(PUCHAR FileDataBuf,
                         ULONG  FileDataLen,
                         UCHAR  FileDataType, //0:HEX,1:BIN
                         ULONG  StartAddr,    // start address of file
                         PUCHAR DnDataBuf,    // download file content buffer
                         ULONG *DnDataLen);   // download file content length

// Load chip configuration
BOOL LoadChipCfg();

// Read the specified key value from the configuration file
int GetProfileString(char *profile, char *AppName, char *KeyName, char *KeyVal);

// Resovle the IspOption download option from the configuration file
int SetIspCfg(PCH55xIspOptionS pIspOption, char *cfgfilename, char *PortName, int PortBaud);

//Convert letters to uppercase
void CharUpper(char *str);

// Set log switch
void setDebug(int level);

// Get file length
unsigned int get_file_size(const char *path);

//init WCH USB
int wch55xusb_int();

//deinit WCH USB
void wch55xusb_exit();


#endif /* Utils_h */
