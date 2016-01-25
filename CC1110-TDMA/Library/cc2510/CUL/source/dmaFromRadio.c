/******************************************************************************
*                                                                             *
*        **********                                                           *
*       ************                                                          *
*      ***        ***                                                         *
*     ***    ++    ***                                                        *
*     ***   +  +   ***                      CHIPCON                           *
*     ***   +                                                                 *
*     ***   +  +   ***                                                        *
*     ***    ++    ***                                                        *
*      ***        ***                                                         *
*       ************                                                          *
*        **********                                                           *
*                                                                             *
*******************************************************************************

Filename:     dmaFromRadio.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/

#include "cul.h"

//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void culDmaFromRadio(DMA_DESC* pDmaChannel, BYTE *pDstAddr, BOOL generateInterrupt)
{
   SET_WORD(pDmaChannel->SRCADDRH, pDmaChannel->SRCADDRL,   &X_RFD);   // The start address of the segment
   SET_WORD(pDmaChannel->DESTADDRH, pDmaChannel->DESTADDRL, pDstAddr); // Tx FiFo address
   SET_WORD(pDmaChannel->LENH, pDmaChannel->LENL, 255);                // Maximum of the Tx FiFo.
   pDmaChannel->VLEN       = VLEN_1_P_VALOFFIRST_P_2;// The first byte indicates transfer length
   pDmaChannel->PRIORITY   = PRI_LOW;
   pDmaChannel->M8         = M8_USE_8_BITS;
   pDmaChannel->IRQMASK    = generateInterrupt; // The DMA complete interrupt flag is set at completion.
   pDmaChannel->DESTINC    = DESTINC_1;         // The destination is constant
   pDmaChannel->SRCINC     = SRCINC_0;          // The address for data fetch is incremented by 1 byte after each transfer.
   pDmaChannel->TRIG       = DMATRIG_RADIO;     // DMA is started by the radio receiver.
   pDmaChannel->TMODE      = TMODE_SINGLE;      // One byte is transferred at each DMA trigger.
   pDmaChannel->WORDSIZE   = WORDSIZE_BYTE;     // Byte transfer.

   return;
}
