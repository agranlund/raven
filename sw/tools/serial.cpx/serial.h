/*
 * resource set indices for serial
 *
 * created by ORCS 2.16
 */

/*
 * Number of Strings:        45
 * Number of Bitblks:        0
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       7
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        42
 * Number of Trees:          1
 * Number of Userblks:       0
 * Number of Images:         0
 * Total file size:          1588
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "serial"
#endif
#undef RSC_ID
#ifdef serial
#define RSC_ID serial
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 45
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 7
#define NUM_OBS 42
#define NUM_TREE 1
#endif



#define SERIAL     0 /* free form */
#define FAILED     2 /* BOX in tree SERIAL */
#define PROT_BOX   7 /* BOX in tree SERIAL */
#define BAUDRATE   8 /* BUTTON in tree SERIAL */
#define BITS       9 /* BUTTON in tree SERIAL */
#define PARITY    10 /* BUTTON in tree SERIAL */
#define STOPS     11 /* BUTTON in tree SERIAL */
#define FLOW      17 /* BUTTON in tree SERIAL */
#define RSVF      18 /* BOX in tree SERIAL */
#define DRVR_BOX  21 /* BOX in tree SERIAL */
#define RCVE_BUF  23 /* FTEXT in tree SERIAL */ /* max len 5 */
#define LAN_BOX   24 /* IBOX in tree SERIAL */
#define USE_LAN   25 /* BUTTON in tree SERIAL */
#define LAN_TXT   26 /* STRING in tree SERIAL */
#define SEND_BUF  27 /* FTEXT in tree SERIAL */ /* max len 5 */
#define DTR       28 /* BUTTON in tree SERIAL */
#define FLUSH     29 /* BUTTON in tree SERIAL */
#define BREAK     30 /* BUTTON in tree SERIAL */
#define PORT      31 /* BUTTON in tree SERIAL */
#define MODE      32 /* BUTTON in tree SERIAL */
#define SAVE      38 /* BUTTON in tree SERIAL */
#define SET       40 /* BUTTON in tree SERIAL */
#define CANCEL    41 /* BUTTON in tree SERIAL */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD serial_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD serial_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD serial_rsc_free(void);
#endif
