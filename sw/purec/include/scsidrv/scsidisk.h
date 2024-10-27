/*********************************************************************
 *
 * Kommandos zum Zugriff auf CD-ROMs
 *
 * $Source: u:\k\usr\src\scsi\cbhd\rcs\scsidisk.h,v $
 *
 * $Revision: 1.2 $
 *
 * $Author: S_Engel $
 *
 * $Date: 1995/11/28 19:14:14 $
 *
 * $State: Exp $
 *
 **********************************************************************
 * History:
 *
 * $Log: scsidisk.h,v $
 * Revision 1.2  1995/11/28  19:14:14  S_Engel
 * *** empty log message ***
 *
 * Revision 1.1  1995/11/13  23:45:38  S_Engel
 * Initial revision
 *
 *
 *
 *********************************************************************/

#ifndef __SCSIDISK_H
#define __SCSIDISK_H

LONG Read6(ULONG BlockAdr,
              UWORD TransferLen, void *buffer);

LONG Read10(ULONG BlockAdr,
               UWORD TransferLen, void *buffer);

LONG Write6(ULONG BlockAdr,
               UWORD TransferLen, void *buffer);

LONG Write10(ULONG BlockAdr,
                UWORD TransferLen, void *buffer);


LONG Read(ULONG BlockAdr,
             UWORD TransferLen, void *buffer);
  /*
   * ReadCmd liest Datenbl�cke ein
   * Wenn n�tig, wird ein langes Kommando (10 Byte, Class 1) verwendet.
   */

LONG Write(ULONG BlockAdr,
              UWORD TransferLen, void *buffer);
  /*
   * WriteCmd speichert Datenbl�cke ab.
   * Wenn n�tig, wird ein langes Kommando (10 Byte, Class 1) verwendet.
   */

LONG StartStop(BOOLEAN LoadEject, BOOLEAN StartFlag);


/*-------------------------------------------------------------------------*/
/*-                                                                       -*/
/*- ReadCapacity fragt die Gr��e des Laufwerkes ab.                       -*/
/*- Bei PMI = TRUE wird der nach BlockAdr n�chste Block angegeben, der    -*/
/*- noch ohne Verz�gerung �bertragen werden kann.                         -*/
/*- Bei Platten kann dies der letzte PBlock auf dem gleichen Zylinder wie -*/
/*- BlockAdr sein, bei CD-ROMs in etwa der letzte Block, der ohne         -*/
/*- Geschwindigkeits�nderung �bertragen werden kann.                      -*/
/*- PMI=FALSE erfragt die absolute Gr��e des Ger�tes.                     -*/
/*- SCSI-Opcode $25                                                       -*/
/*-                                                                       -*/
/*-                                                                       -*/
/*-------------------------------------------------------------------------*/
LONG ReadCapacity(BOOLEAN PMI, ULONG *BlockAdr, ULONG *BlockLen);


BOOLEAN init_scsidisk (void);
  /* Initialisierung des Moduls */


#endif


