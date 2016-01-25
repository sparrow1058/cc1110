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

Filename:     dmaToUart0.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/

#include "cul.h"

//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void culDmaToUart0(DMA_DESC* pDmaChannel, WORD length, BYTE *pSrcAddr, BOOL generateInterrupt)
{
   SET_WORD(pDmaChannel->SRCADDRH, pDmaChannel->SRCADDRL,   pSrcAddr);  // The start address of the segment
   SET_WORD(pDmaChannel->DESTADDRH, pDmaChannel->DESTADDRL, &X_U0DBUF); // Tx FIFO address
   SET_WORD(pDmaChannel->LENH, pDmaChannel->LENL, length);              // Setting the length of the transfer (bytes)
   pDmaChannel->VLEN       = VLEN_USE_LEN;      // Using the length field
   pDmaChannel->PRIORITY   = PRI_LOW;           // Transfer Priority
   pDmaChannel->M8         = M8_USE_8_BITS;     // Transferring all 8 bits in each byte.
   pDmaChannel->IRQMASK    = generateInterrupt; // The DMA complete interrupt flag is set at completion.
   pDmaChannel->DESTINC    = DESTINC_0;         // The destination address is constant
   pDmaChannel->SRCINC     = SRCINC_1;          // The address for data fetch is inremented by 1 byte
   // after each transfer.
   pDmaChannel->TRIG       = DMATRIG_UTX0;      // DMA is started by writing the DMAREQ register.
   pDmaChannel->TMODE      = TMODE_SINGLE;      // The whole block is transferred.
   pDmaChannel->WORDSIZE   = WORDSIZE_BYTE;     // One byte is transferred each time.

   return;
}
