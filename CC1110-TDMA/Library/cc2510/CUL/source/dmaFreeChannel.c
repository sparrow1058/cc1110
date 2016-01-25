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

Filename:     dmaFreeChannel.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/

#include <string.h>
#include "cul.h"

extern DMA_DESC __xdata dmaChannel1to4[4];
extern DMA_TABLE_ENTRY dmaTable[DMA_ADM_NUMBER_OF_CHANNELS];

//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void culDmaFreeChannel(BYTE dmaChannelNumber){
   // Saving the old interrupt enable status and turning off global interrupt.
   BYTE savedIntEnable = IEN0;
   INT_GLOBAL_ENABLE(INT_OFF);

   // Turning off the DMA channel
   DMA_ABORT_CHANNEL(dmaChannelNumber);

   // Clearing the channel occupied flag in the table
   dmaTable[dmaChannelNumber].assigned = FALSE;
   dmaTable[dmaChannelNumber].callBackFunction = NULL;

   // Restoring the interrupt enable status
   IEN0 = savedIntEnable;

   return;
} // ends culDmaFreeChannel()
