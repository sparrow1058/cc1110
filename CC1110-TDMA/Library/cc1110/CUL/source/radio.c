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

Filename:     radio.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include <string.h>
#include "cul.h"

extern volatile BYTE sppRxStatus;
extern volatile BYTE sppTxStatus;
SPP_STRUCT rxData;
SPP_STRUCT txData;

//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void radioInit(UINT32 frequency, BYTE localAddress)
{
   sppInit(frequency,localAddress);

   return;
}


//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
BOOL radioSend(BYTE* transmitData, WORD dataLength, BYTE remoteAddress, BYTE doAck)
{
   WORD sent = 0;
   BOOL status = TRUE;
   WORD remaining;
   BYTE retries;
   BYTE res;
   UINT8 timeOut;
   txData.destAddress = remoteAddress;
   txData.flags = doAck;


   while((sent < dataLength) && (status == TRUE))
   {
      retries = ACK_RETRIES;
      remaining = dataLength-sent;


      if(remaining > SPP_MAX_PAYLOAD_LENGTH)
      {
         memmove(txData.payload, transmitData + sent, SPP_MAX_PAYLOAD_LENGTH);
         txData.payloadLength = SPP_MAX_PAYLOAD_LENGTH;
         sent += SPP_MAX_PAYLOAD_LENGTH;
      }
      else
      {
         memmove(txData.payload, transmitData + sent, dataLength);
         txData.payloadLength = dataLength;
         sent += dataLength;
      }

      while(retries)
      {
         res = sppSend(&txData);
         if(res == CHANNEL_BUSY)
         {
            halWait(10);
            retries--;
            if(retries == 0)
            {
               status = FALSE;
            }
         }
         else
         {
            retries = 0;
         }
      }
      timeOut = (ACK_TIMEOUT << 3);
      while((sppTxStatus == TX_IN_PROGRESS) && timeOut)
      {
        halWait(1);
        if(!(--timeOut)){
          sppTxStatus = DEST_UNREACHABLE;
        }
      }


      if(sppTxStatus == DEST_UNREACHABLE)
      {
         status = FALSE;
      }
   }

   return status;
}





//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
BOOL radioReceive(BYTE** receiveData, BYTE* length, WORD timeout, BYTE* sender)
{
   BOOL status = TRUE;
   BOOL continueWaiting = TRUE;
   BOOL useTimeout = FALSE;

   if(timeout)
   {
      useTimeout = TRUE;
   }



   sppReceive(&rxData);

   while((sppRxStatus != RX_COMPLETE) && (continueWaiting))
   {
      if(useTimeout)
      {
         halWait(0x01);
         timeout--;
         if(timeout == 0)
         {
            continueWaiting = FALSE;
            status = FALSE;
         }
      }
   }

   if(status == TRUE)
   {
      *receiveData = rxData.payload;
      *length = rxData.payloadLength;
      *sender = rxData.srcAddress;
   }

   return status;
}
