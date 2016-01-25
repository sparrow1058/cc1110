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

Filename:     dmaSetUpChannel.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/

#include <string.h>
#include "cul.h"


extern DMA_DESC /*__xdata*/ dmaChannel1to4[4];
extern DMA_TABLE_ENTRY /*__xdata*/ dmaTable[4];

//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
BYTE culDmaSetUpChannel(DMA_DESC* pDmaDescriptor, BOOL immediately, FUNCTION* callBackFunction){
   DMA_DESC* assignedDmaDescriptor = NULL;
   UINT8 dmaChannelNumber = 0x00;

   // Requesting an available DMA descriptor
   assignedDmaDescriptor = culDmaAllocChannel(&dmaChannelNumber, callBackFunction);

   // If all DMA channels are in use or an illegal channel number is assigned,the function returns.
   if((assignedDmaDescriptor == NULL) || (dmaChannelNumber == 0x00)){
      dmaChannelNumber = 0x00;
   } else if(dmaChannelNumber > 0x04){
      dmaChannelNumber = 0x00;
   } else {

      // Copying the DMA descriptor to the list of DMA descriptors.
      memmove(assignedDmaDescriptor, pDmaDescriptor, 8);

      // Arming the DMA channel in question
      DMA_ARM_CHANNEL(dmaChannelNumber);

      // If the transfer is to be performed ASAP, the channel is initiated.
      if(immediately){
         DMA_START_CHANNEL(dmaChannelNumber);
      }
   }

   // Returning the number of the channel in use
   return dmaChannelNumber;
}

