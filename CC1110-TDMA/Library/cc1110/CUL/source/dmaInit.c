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

Filename:     dmaInit.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/

#include <string.h>
#include "cul.h"

// Reserving space for a global DMA channel descriptor list
DMA_DESC dmaChannel1to4[4];

// Table for assigning DMA channels.
DMA_TABLE_ENTRY dmaTable[DMA_ADM_NUMBER_OF_CHANNELS];


//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void culDmaInit(void){

   // Clearing all channels
   memset((void*)dmaChannel1to4, 0, sizeof(dmaChannel1to4));

   // Clearing the DMA table
   memset((void*)dmaTable, 0, sizeof(dmaTable));

   // Clearing any arming of channels 1 to 4, but leaving staus of channel 0.
   DMAARM &= DMA_CHANNEL_0;

   // Clearing all starting of DMA channels 1 to 4, but leaving status of channel 0.
   DMAREQ &= DMA_CHANNEL_0;

   // Clearing all DMA interrupt flags of channels 1 to 4, but leaving status of channel 0.
   DMAIRQ &= DMA_CHANNEL_0;

   // Clearing the interrupt flag of the DMA and enabling DMA interrupt.
   INT_SETFLAG(INUM_DMA, INT_CLR);
   INT_ENABLE(INUM_DMA, INT_ON);

   // Setting the address of the DMA descriptors
   DMA_SET_ADDR_DESC1234(dmaChannel1to4);

} // ends culDmaInit()


//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void culSetDmaChannel0CallBack(FUNCTION* pCallBackFunction)
{
   dmaTable[0].assigned = TRUE;
   dmaTable[0].callBackFunction = pCallBackFunction;

   return;
}


//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void culClearDmaChannel0CallBack(void)
{
   dmaTable[0].assigned = FALSE;
   dmaTable[0].callBackFunction = NULL;

   return;
}



//-----------------------------------------------------------------------------
// dma_IRQ
//
//  Description:
//      This interrupt routine is run each time the DMA generates an interrupt.
//      According to the interrupt flag of the DMA channel, the corresponding
//      callBackFunction is called. The interrupt flags are cleared.
//-----------------------------------------------------------------------------
#pragma vector=DMA_VECTOR
__interrupt void dma_IRQ (void){

   BYTE clearedFlags = 0x00;
   
   INT_GLOBAL_ENABLE(INT_OFF);   

   // Handle each channel
   if ((DMAIRQ & DMA_CHANNEL_0) && dmaTable[0].callBackFunction) { clearedFlags |= DMA_CHANNEL_0; dmaTable[0].callBackFunction(); }
   if ((DMAIRQ & DMA_CHANNEL_1) && dmaTable[1].callBackFunction) { clearedFlags |= DMA_CHANNEL_1; dmaTable[1].callBackFunction(); }
   if ((DMAIRQ & DMA_CHANNEL_2) && dmaTable[2].callBackFunction) { clearedFlags |= DMA_CHANNEL_2; dmaTable[2].callBackFunction(); }
   if ((DMAIRQ & DMA_CHANNEL_3) && dmaTable[3].callBackFunction) { clearedFlags |= DMA_CHANNEL_3; dmaTable[3].callBackFunction(); }
   if ((DMAIRQ & DMA_CHANNEL_4) && dmaTable[4].callBackFunction) { clearedFlags |= DMA_CHANNEL_4; dmaTable[4].callBackFunction(); }

   // Clear the flags
   INT_SETFLAG(INUM_DMA, INT_CLR);
   DMAIRQ = ~clearedFlags;

   INT_GLOBAL_ENABLE(INT_ON);
}
