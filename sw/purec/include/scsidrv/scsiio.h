/****************************************************************************
 *
 * Definitionen und Kommandos f�r SCSI-Calls in C
 *
 * $Source: U:\USR\src\scsi\CBHD\include\scsidrv\RCS\scsiio.h,v $
 *
 * $Revision: 1.7 $
 *
 * $Author: S_Engel $
 *
 * $Date: 1995/11/28 19:14:14 $
 *
 * $State: Exp $
 *
 *****************************************************************************
 * History:
 *
 * $Log: scsiio.h,v $
 * Revision 1.7  1995/11/28  19:14:14  S_Engel
 * *** empty log message ***
 *
 * Revision 1.6  1995/10/22  15:43:34  S_Engel
 * Kommentare leicht �berarbeitet
 *
 * Revision 1.5  1995/10/03  12:49:08  S_Engel
 * Typendefinitionen nach scsidefs �bertragen
 *
 * Revision 1.4  1995/09/29  09:12:16  S_Engel
 * alles n�tige f�r virtuelles RAM
 *
 * Revision 1.3  1995/06/16  12:06:46  S_Engel
 * *** empty log message ***
 *
 * Revision 1.2  1995/03/09  09:53:16  S_Engel
 * Flags: Disconnect eingef�hrt
 *
 * Revision 1.1  1995/03/05  18:54:16  S_Engel
 * Initial revision
 *
 *
 *
 ****************************************************************************/


#ifndef __SCSIIO_H
#define __SCSIIO_H

#include <portab.h>
#include "scsidrv/scsidefs.h"           /* Typen f�r SCSI-Lib */

/*****************************************************************************
 * Typen
 *****************************************************************************/


/*****************************************************************************
 * Konstanten                                                                *
 *****************************************************************************/
#define DefTimeout 4000




/*****************************************************************************
 * Variablen
 *****************************************************************************/

extern tpScsiCall scsicall;     /* READ ONLY!! */

extern BOOLEAN    HasVirtual;   /* READ ONLY!! */

extern tReqData   ReqBuff;      /* Request Sense Buffer f�r alle Kommandos */

extern WORD       DriverRev;    /* Revision of identified scsidriver in system */

/*****************************************************************************
 * Funktionen und zugeh�rige Typen
 *****************************************************************************/




/* f�r In und Out k�nnen diese Routinen gerufen werden, sie beachten selbstt�tig,
 * wenn bei virtuellem RAM die Daten umkopiert werden m�ssen
 */
LONG cdecl In          (tpSCSICmd Parms);

LONG cdecl Out         (tpSCSICmd Parms);

LONG cdecl InquireSCSI (WORD          what,
                        tBusInfo     *Info);

LONG cdecl InquireBus  (WORD          what,
                        WORD          BusNo,
                        tDevInfo     *Dev);

LONG cdecl CheckDev    (WORD          BusNo,
                        const DLONG  *DevNo,
                        char         *Name,
                        UWORD        *Features);

LONG cdecl RescanBus   (WORD          BusNo);

LONG cdecl Open        (WORD          bus,
                        const DLONG  *Id,
                        ULONG        *MaxLen);

LONG cdecl Close       (tHandle       handle);

LONG cdecl Error       (tHandle       handle,
                        WORD          rwflag,
                        WORD          ErrNo);



BOOLEAN init_scsiio (void);
  /* Initialisierung des Moduls */

#endif
