#ifndef _MFP_H_
#define _MFP_H_

#include "../sys.h"

#define MFP1_CLK        2457600
#define MFP2_CLK        2000000

#define MFP_GPDR        0x01
#define MFP_AER         0x03
#define MFP_DDR         0x05
#define MFP_IERA        0x07
#define MFP_IERB        0x09
#define MFP_IPRA        0x0B
#define MFP_IPRB        0x0D
#define MFP_ISRA        0x0F
#define MFP_ISRB        0x11
#define MFP_IMRA        0x13
#define MFP_IMRB        0x15
#define MFP_VR          0x17
#define MFP_TACR        0x19
#define MFP_TBCR        0x1B
#define MFP_TCDCR       0x1D
#define MFP_TADR        0x1F
#define MFP_TBDR        0x21
#define MFP_TCDR        0x23
#define MFP_TDDR        0x25
#define MFP_SCR         0x27
#define MFP_UCR         0x29
#define MFP_RSR         0x2B
#define MFP_TSR         0x2D
#define MFP_UDR         0x2F


#ifndef __ASM__

bool mfp_Init();

#endif //!__ASM__


#endif // _MFP_H_

