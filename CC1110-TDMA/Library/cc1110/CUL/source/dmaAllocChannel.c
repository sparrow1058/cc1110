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

Filename:     dmaAllocChannel.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/

#include "cul.h"

extern DMA_DESC dmaChannel1to4[4];
extern DMA_TABLE_ENTRY dmaTable[DMA_ADM_NUMBER_OF_CHANNELS];

//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
DMA_DESC* culDmaAllocChannel(UINT8* pDmaChannelNumber, FUNCTION* callBackFunction){
   DMA_DESC* dmaDesc;
   BYTE savedIntEnable = 0x00;
   INT8 i;

   // Checking for an unassigned DMA channel
   for(i = 1; i <= DMA_ADM_NUMBER_OF_CHANNELS; i++){
      if(dmaTable[i].assigned == FALSE){
         break;
      }
   }

   // If no channel is available, the function returns.
   if(i > DMA_ADM_NUMBER_OF_CHANNELS){
      *pDmaChannelNumber = 0x00;
      dmaDesc = NULL;
   }
   // An available table entry was found
   else {
      // Deactivating the channel and erasing old interrupt flag
      DMA_ABORT_CHANNEL(i);
      DMAIRQ &= ~(1 << i);

      // Storing interrupt enable register and turning off interrupts.
      savedIntEnable = IEN0;
      INT_GLOBAL_ENABLE(INT_OFF);

      // Reserving the DMA channel.
      dmaTable[i].assigned = TRUE;
      dmaTable[i].callBackFunction = callBackFunction;
      *pDmaChannelNumber = i;

      // Restoring old interrupt enable register.
      IEN0 = savedIntEnable;

      dmaDesc = &dmaChannel1to4[i-1];
   }

   // Returning pointer to the DMA descriptor
   return dmaDesc;
} // ends culDmaAlloc()


