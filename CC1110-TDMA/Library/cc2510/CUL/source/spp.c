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

Filename:     spp.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include <string.h>
#include "cul.h"
#include <stdio.h>
#define FREQUENCY_4_CC1110       433500     // radioConfigure() in
// protos
void rxCallBack(void);
void ackTimeout(void);
BOOL ackReceived(BYTE sourceAddress);
void sendAck(SPP_STRUCT* receivedPacket);
void waitForAck(void);


static DMA_DESC* dmaTx;          // pointer to the DMA descriptor for transmit.
static DMA_DESC* dmaRx;          // pointer to the DMA descriptor for receive.
static BYTE dmaNumberTx = 0;     // number indicating which DMA channel is used for transmit.
static BYTE dmaNumberRx = 0;     // number indicating which DMA channel is used for receive.
static BYTE myAddress;
volatile BYTE sppRxStatus = 0;
volatile BYTE  sppTxStatus = 0;
static BYTE pAckBuffer[6];
static SPP_STRUCT* pAckData;
static volatile UINT8 retransmissionCounter;
static UINT8 ackTimerNumber;
static FUNCTION* rxCallBackFunction;


//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void sppSetRxCallBackFunction(FUNCTION* callBackFunction)
{
   rxCallBackFunction = callBackFunction;
} // Ends sppSetRxCallBackFunction()




void sendAck(SPP_STRUCT* receivedPacket)
{
   SIDLE();
   // Debug:
   halWait(1);
   DMA_ABORT_CHANNEL(dmaNumberTx);

   RFTXRXIF = 0;
   STX();
   RFIF &= ~IRQ_DONE;
   while(RFTXRXIF == 0);
   RFD = SPP_HEADER_AND_FOOTER_LENGTH + SPP_ACK_LENGTH;
   RFTXRXIF = 0;
   while(RFTXRXIF == 0);
   RFTXRXIF = 0;
   RFD = receivedPacket->srcAddress;
   while(RFTXRXIF == 0);
   RFTXRXIF = 0;
   RFD = myAddress;
   while(RFTXRXIF == 0);
   RFTXRXIF = 0;
   RFD = ACK;
   while(!(RFIF & IRQ_DONE));
   RFIF &= ~IRQ_DONE;

   return;
}

//------------------------------------------------------------------------------------------------------
// void rxCallBack(...)
//
//  Description:
//      This function is called by the interrupt routine when the Rx DMA channel
//      finishes the data transfer. The received packet's destination address
//      is checked. If not addressed to this node, or if the CRC value is not
//      correct, the packet is erased. An ACK is sent if the packet
//      tells to. A user defined callback function may is run if set (set
//      with setRxCallBackFunction())
//
//  Arguments:
//      void
//
//  Return value:
//      void
//-----------------------------------------------------------------------------
void rxCallBack(void)
{
   SPP_STRUCT __xdata* receivedPacket;
   BYTE res = FALSE;


   // Investigating the received packet.
   // Checking the destination address and that the CRC is OK.
   // The message is ACK'ed if it tells to.
   receivedPacket = (SPP_STRUCT __xdata*) GET_DMA_DEST(dmaRx);
   receivedPacket->payloadLength = receivedPacket->payloadLength-SPP_HEADER_AND_FOOTER_LENGTH;

   if((receivedPacket->destAddress == myAddress) || (receivedPacket->destAddress == BROADCAST_ADDRESS))
   {
      if(receivedPacket->payload[receivedPacket->payloadLength+1] & 0x80)
      {
         if(receivedPacket->flags == ACK)
         {
            res = ackReceived(receivedPacket->srcAddress);
         }
         else
         {
            sppRxStatus = PACKET_RECEIVED;
            res = TRUE;
            if(receivedPacket->flags & DO_ACK)
            {
               sendAck(receivedPacket);
            }
            sppRxStatus = RX_COMPLETE;
            if(rxCallBackFunction)
            {
               rxCallBackFunction();
            }
         }
      }
   }

   if(res == FALSE)
   {

      // rearming DMA channel
      DMA_ARM_CHANNEL(dmaNumberRx);
      RFIM |= IRQ_SFD;
      sppRxStatus = RX_WAIT;
   }
   return;
}   // ends rxCallBack




//-----------------------------------------------------------------------------
// void ackTimeout(...)
//
//  Description:
//      This function resends a packet if it is not ACK'ed by the recipient
//      within _ACK_TIMEOUT_ m-seconds. The message is resent _ACK_RETRIES_ times.
//      If the message remains un-ACK'ed, transmission is aborted and spp TX
//      status is set to DEST_UNREACHABLE.
//
//  Arguments:
//      void
//
//  Return value:
//      void
//-----------------------------------------------------------------------------
void ackTimeout(void){
   culTimer4AdmClear(ackTimerNumber);

   if(pAckData != NULL)
   {
      if(retransmissionCounter < ACK_RETRIES)
      {
         // Resending the message.
         pAckData->flags |= RETRANSMISSION;

         TIMER4_RUN(FALSE);

         sppSend(pAckData);

         T4CNT = 0;
         TIMER4_RUN(TRUE);

         retransmissionCounter++;
      }
      else
      {
         // The packet has been resent too many times. Assuming that the node is unreacheble.
         pAckData = 0;
         retransmissionCounter = 0;
         sppTxStatus = DEST_UNREACHABLE;
         RFIM &= ~IRQ_SFD;
      }
   }
   return;
} // ends ackTimeout




//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
BOOL sppInit(UINT32 frequency, BYTE address){
   BYTE res = 0;
   BOOL status = TRUE;

   sppSetAddress(address);

   // Clearing the states of the spp.
   sppTxStatus = TX_IDLE;
   sppRxStatus = RX_IDLE;
   retransmissionCounter = 0;
   ackTimerNumber = 0;
   pAckData = 0;

   // Clearing the RF interrupt flags and enable mask and enabling RF interrupts
   RFIF = 0;
   RFIM = 0;
   INT_SETFLAG(INUM_RF,INT_CLR);
   INT_ENABLE(INUM_RF,INT_ON);

   // Setting the frequency and initialising the radio
   res = halRfConfig(FREQUENCY_4_CC1110);
   if(res == FALSE){
      status = FALSE;
   }

   // Always in RX unless sending.

   //debug:
   MCSM1 = 0x2F;
//   MCSM1 = 0x3F;

   SRX();


   // Using the timer 4 administrator to generate interrupt to check if a message is unacked...
   culTimer4AdmInit();

   // Initialising the DMA administrator
   culDmaInit();

   // Requesting a DMA channel for transmit data. No callback function is used. Instead the TX_DONE
   // interrupt is used to determine when a transfer is finished. Configuring the DMA channel for
   // transmit. The data address and length will be set prior to each specific transmission.
   dmaTx = culDmaAllocChannel(&dmaNumberTx, 0);
   if((dmaNumberTx == 0) || (dmaNumberTx > 4)){
      status = FALSE;
   }

   culDmaToRadio(dmaTx, SPP_MAX_PAYLOAD_LENGTH + SPP_HEADER_AND_FOOTER_LENGTH, 0, FALSE);

   // Requesting a DMA channel for receiving data. Giving the address of the callback function.
   // Configuring the DMA channel for receive. The data address will be set prior to each specific
   // reception.
   dmaRx = culDmaAllocChannel(&dmaNumberRx, &rxCallBack);
   if((dmaNumberRx == 0) || (dmaNumberRx > 4)){
      status = FALSE;
   }
   culDmaFromRadio(dmaRx, 0, TRUE);

   // Making sure that none of the channels are armed.
   DMA_ABORT_CHANNEL(dmaNumberRx);
   DMA_ABORT_CHANNEL(dmaNumberTx);
   INT_ENABLE(INUM_DMA, INT_ON);

   return status;
} // ends sppInit




//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void sppSetAddress(BYTE address){
   myAddress = address;
} // Ends sppSetAddress()




// Internal function which enables the timeout when waiting for an ACK.
void waitForAck(void)
{
   ackTimerNumber = culTimer4AdmSet(ACK_TIMEOUT, &ackTimeout);
   SET_DMA_DEST(dmaRx,pAckBuffer);
   SET_DMA_LENGTH(dmaRx,6);

}   // Ends waitForAck()




//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
BYTE sppSend(SPP_STRUCT* pPacketPointer){
   BYTE res = TRUE;

   // Checking that length is not too long
   if (pPacketPointer->payloadLength > SPP_MAX_PAYLOAD_LENGTH)
   {
      res = TOO_LONG;
      sppTxStatus = TX_IDLE;
   }

   // Flipping the sequence bit, writing  total packet length and address if the transfer is not a retransmission.
   // If it is a retransmission, the fields are correct
   if(!(pPacketPointer->flags & RETRANSMISSION))
   {
      pPacketPointer->flags ^= SEQUENCE_BIT;
      pPacketPointer->payloadLength += SPP_HEADER_AND_FOOTER_LENGTH;
      pPacketPointer->srcAddress = myAddress;
   }


   // Setting up the DMA
   DMA_ABORT_CHANNEL(dmaNumberTx);
   SET_DMA_SOURCE(dmaTx,pPacketPointer);


   // Proceed if the packet length is OK.
   if (res == TRUE)
   {
      // Clearing RF interrupt flags and enabling RF interrupts.
      RFIF &= ~IRQ_DONE;
      RFIM &= ~IRQ_SFD;
      INT_SETFLAG(INUM_RF, INT_CLR);

      if(!CCA)
      {
         SRX();
         // Turning on Rx and waiting to make the RSSI value become valid.
         halWait(1);
      }

      if(CCA)
      {  // Setting up radio
         DMA_ABORT_CHANNEL(dmaNumberRx);
         SIDLE();
         RFTXRXIF = 0;
         INT_GLOBAL_ENABLE(FALSE);
         DMA_ARM_CHANNEL(dmaNumberTx);
         STX();
         INT_GLOBAL_ENABLE(TRUE);
         sppTxStatus = TX_IN_PROGRESS;

         if(pPacketPointer->flags & DO_ACK)
         {
            pAckData = pPacketPointer;
            waitForAck();
         }
         else
         {
            pAckData = NULL;
         }
         RFIM |= IRQ_DONE;
      }
      // The "air" is busy
      else
      {
         res = CHANNEL_BUSY;
         RFIM &= ~IRQ_DONE;
         // De-flipping the sequence bit.
         if(!(pPacketPointer->flags & RETRANSMISSION))
         {
            pPacketPointer->flags ^= SEQUENCE_BIT;
         }
      }
   }
   return res;
} // ends sppSend




// Internal function which is called when an ack is received.
// If the ACK is from the expected node, the retransmission of the packet is cancelled.
BOOL ackReceived(BYTE sourceAddress)
{
   BOOL res = FALSE;
   if(sourceAddress == pAckData->destAddress)
   {
      res = TRUE;
      culTimer4AdmClear(ackTimerNumber);
      sppTxStatus = TX_SUCCESSFUL;
      retransmissionCounter = 0;
      pAckData = 0;
   }

   return res;
}  //Ends ackReceived()



//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
#pragma vector=RF_VECTOR
__interrupt void spp_rf_IRQ(void)
{
   BYTE enabledAndActiveInterrupt;

   INT_GLOBAL_ENABLE(INT_OFF);
   enabledAndActiveInterrupt = RFIF;
   RFIF = 0x00;                        // Clear all interrupt flags
   INT_SETFLAG(INUM_RF, INT_CLR);    // Clear MCU interrupt flag
   enabledAndActiveInterrupt &= RFIM;

   // Start of frame delimiter (SFD)
   if(enabledAndActiveInterrupt & IRQ_SFD)
   {
      if(sppRxStatus == RX_WAIT)
      {
         sppRxStatus = RX_IN_PROGRESS;
         RFIM &= ~IRQ_SFD;
      }
   }

   // Transmission of a packet is finished. Enabling reception of ACK if required.
   if(enabledAndActiveInterrupt & IRQ_DONE)
   {
      if(sppTxStatus == TX_IN_PROGRESS)
      {
         if(pAckData == NULL)
         {
            sppTxStatus = TX_SUCCESSFUL;
         }
         else
         {
            DMA_ARM_CHANNEL(dmaNumberRx);
         }
      }

      // Clearing the tx done interrupt enable
      RFIM &= ~IRQ_DONE;

   }
   INT_GLOBAL_ENABLE(INT_ON);
}





//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void sppReceive(SPP_STRUCT* pReceiveData){

   sppRxStatus = RX_WAIT;

   DMA_ABORT_CHANNEL(dmaNumberRx);
   // Setting the address to where the received data are to be written. // Setting max number of bytes to receive
   SET_DMA_DEST(dmaRx,pReceiveData);
   SET_DMA_LENGTH(dmaRx,255);

   // Arming the DMA channel. The receiver will initate the transfer when a packet is received.
   DMA_ARM_CHANNEL(dmaNumberRx);

   // Debug:
   SIDLE();

   RFIF &= ~IRQ_SFD;
   RFIM |= IRQ_SFD;
   // Turning on the receiver
   SRX();

   return;
}



