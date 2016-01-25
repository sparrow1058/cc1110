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

Filename:     rfReceivePacket.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include "hal.h"

BYTE volatile length;

BOOL volatile isLengthByte;
UINT8 index;
BYTE* pPayload;
BYTE* pRssiVal;
BYTE* pLqiVal;
BYTE lengthReceived;
//-----------------------------------------------------------------------------
// See hal.h for a description of this function.
//-----------------------------------------------------------------------------
BYTE halRfReceivePacket(BYTE* pData, BYTE* pRssi, BYTE* pLqi, BYTE timeOut){
    index = 0;
    length = 0;
//    RFTXRXIF = 0;
    INT_SETFLAG(INUM_RFTXRX, INT_CLR);
    INT_ENABLE(INUM_RFTXRX, INT_ON);
    INT_GLOBAL_ENABLE(INT_ON);

    RFIF = RFIF & ~IRQ_DONE;

    pPayload = pData;
    pRssiVal = pRssi;
    pLqiVal = pLqi;
    *pRssiVal = 0x00;
    *pLqiVal = 0x00;
    lengthReceived = FALSE;

    isLengthByte = TRUE;

    SRX();

    while(!(RFIF & IRQ_DONE))
    {
      if(timeOut)
      {
         halWait(1);
         timeOut--;
      }
      else
      {
         if(lengthReceived == TRUE)
         {
            timeOut = 255;
            lengthReceived = FALSE;
         }
         else
         {
            SIDLE();
            INT_ENABLE(INUM_RFTXRX, INT_OFF);
            return 0;
         }
      }
   }


   IEN0 &= ~0x80;
   SIDLE();
   RFIF &= ~IRQ_DONE;
   INT_ENABLE(INUM_RFTXRX, INT_OFF);

   if(*pLqi & 0x80)
   {
      return index-1;
   }
   else
   {
      return 0;
   }
}


#pragma vector=RFTXRX_VECTOR
__interrupt static void rf_txrx_IRQ(void)
{

    //The interrupt flag is cleared automatically
    if (isLengthByte)
    {
        length = RFD;
        isLengthByte = FALSE;
        lengthReceived = TRUE;
    }
    else if (index < length)
    {
        pPayload[index++] = RFD;
    }
    else if (index == length)
    {
       index++;
       *pRssiVal = RFD;
    }
    else
    {
       *pLqiVal = RFD;
       isLengthByte = TRUE;
    }

   return;
}

