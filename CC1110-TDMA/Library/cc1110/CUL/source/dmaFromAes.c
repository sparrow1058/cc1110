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

Filename:     dmaFromAes.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/

#include "cul.h"


//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
BOOL culDmaFromAes(DMA_DESC* pDmaChannel, BYTE __xdata* pDstAddr, WORD length, BOOL generateInterrupt){

   // Making sure a multiple of 16 bytes is transferred to the AES module.
   while(length & 0x000F){
      length++;
   }

   SET_WORD(pDmaChannel->SRCADDRH, pDmaChannel->SRCADDRL,   &X_ENCDO); // The start address of the segment
   SET_WORD(pDmaChannel->DESTADDRH, pDmaChannel->DESTADDRL, pDstAddr); // Input of the AES module
   SET_WORD(pDmaChannel->LENH, pDmaChannel->LENL, length); // Setting the length of the transfer (bytes)

   pDmaChannel->VLEN       = VLEN_USE_LEN;      // Using the length field
   pDmaChannel->PRIORITY   = PRI_LOW;           // Transfer Priority
   pDmaChannel->M8         = M8_USE_8_BITS;     // Transferring all 8 bits in each byte.
   pDmaChannel->IRQMASK    = generateInterrupt; // The DMA complete interrupt flag is set at completion.
   pDmaChannel->DESTINC    = DESTINC_1;         // The destination address is inremented by 1 byte
   pDmaChannel->SRCINC     = SRCINC_0;          // The address for data fetch is constant after each transfer.
   pDmaChannel->TRIG       = DMATRIG_ENC_UP;    // Setting the AES module to generate the DMA trigger
   pDmaChannel->TMODE      = TMODE_SINGLE;      // A single byte is transferred each time.
   pDmaChannel->WORDSIZE   = WORDSIZE_BYTE;     // One byte is transferred each time.

   return TRUE;
}
