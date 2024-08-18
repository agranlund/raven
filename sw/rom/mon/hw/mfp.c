#include "mfp.h"


bool mfp_Init()
{

    //-----------------------------------------------------------------------------------------------
    // mfp1
    //-----------------------------------------------------------------------------------------------

    volatile uint8 *mfp1 = (volatile uint8*) PADDR_MFP1;
    mfp1[MFP_GPDR]  = 0x00;      // gpip data register
    mfp1[MFP_AER]   = 0x00;      // interrupts on high->low transition
    mfp1[MFP_DDR]   = 0x00;      // gpip are inputs
    mfp1[MFP_IERA]  = 0x00;      // irqA disable
    mfp1[MFP_IERB]  = 0x00;      // irqB disable
    mfp1[MFP_IPRA]  = 0x00;      // irqA pending
    mfp1[MFP_IPRB]  = 0x00;      // irqB pending
    mfp1[MFP_ISRA]  = 0x00;      // irqA service
    mfp1[MFP_ISRB]  = 0x00;      // irqB service
    mfp1[MFP_IMRA]  = 0x00;      // irqA mask
    mfp1[MFP_IMRB]  = 0x00;      // irqB mask
    mfp1[MFP_VR]    = 0x48;      // vectors 0x40 to 0x4F, software end of interrupt
    mfp1[MFP_TACR]  = 0x00;      // timerA control
    mfp1[MFP_TBCR]  = 0x00;      // timerB control
    mfp1[MFP_TCDCR] = 0x00;      // timerCD control
    mfp1[MFP_TADR]  = 0x00;      // timerA data
    mfp1[MFP_TBDR]  = 0x00;      // timerB data
    mfp1[MFP_TCDR]  = 0x00;      // timerC data
    mfp1[MFP_TDDR]  = 0x00;      // timerD data
    mfp1[MFP_SCR]   = 0x00;      // uart sync char
    mfp1[MFP_UCR]   = 0x00;      // uart control
    mfp1[MFP_RSR]   = 0x00;      // uart rx status
    mfp1[MFP_TSR]   = 0x00;      // uart tx status

    //-----------------------------------------------------------------------------------------------
    // mfp2
    //-----------------------------------------------------------------------------------------------
    volatile uint8 *mfp2 = (volatile uint8*) PADDR_MFP2;
    mfp2[MFP_GPDR]  = 0x00;      // gpip data register
    mfp2[MFP_AER]   = 0xFF;      // ISA interrupts on low->high transition
    mfp2[MFP_DDR]   = 0x00;      // gpip are inputs
    mfp2[MFP_IERA]  = 0x00;      // irqA disable
    mfp2[MFP_IERB]  = 0x00;      // irqB disable
    mfp2[MFP_IPRA]  = 0x00;      // irqA pending
    mfp2[MFP_IPRB]  = 0x00;      // irqB pending
    mfp2[MFP_ISRA]  = 0x00;      // irqA service
    mfp2[MFP_ISRB]  = 0x00;      // irqB service
    mfp2[MFP_IMRA]  = 0x00;      // irqA mask
    mfp2[MFP_IMRB]  = 0x00;      // irqB mask
    mfp2[MFP_VR]    = 0x58;      // vectors 0x50 to 0x5F, software end of interrupt
    mfp2[MFP_TACR]  = 0x00;      // timerA control
    mfp2[MFP_TBCR]  = 0x00;      // timerB control
    mfp2[MFP_TCDCR] = 0x00;      // timerCD control
    mfp2[MFP_TADR]  = 0x00;      // timerA data
    mfp2[MFP_TBDR]  = 0x00;      // timerB data
    mfp2[MFP_TCDR]  = 0x00;      // timerC data
    mfp2[MFP_TDDR]  = 0x00;      // timerD data
    mfp2[MFP_SCR]   = 0x00;      // uart sync char
    mfp2[MFP_UCR]   = 0x00;      // uart control
    mfp2[MFP_RSR]   = 0x00;      // uart rx status
    mfp2[MFP_TSR]   = 0x00;      // uart tx status

    return true;
}


