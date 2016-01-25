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

Filename:     rfSendPacket.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include "hal.h"


//-----------------------------------------------------------------------------
// See hal.h for a description of this function.
//-----------------------------------------------------------------------------
BYTE halRfSendPacket(BYTE* pData, BYTE length){
    BYTE i = 0x00;

    if(length == 0)
    {
      return 0;
    }

    // Turning on radio and writing data including length;
    RFTXRXIF = 0;
    INT_ENABLE(INUM_RFTXRX, INT_OFF);
    RFIF &= ~IRQ_DONE;

    //debug:
    //PKTCTRL0 |= 0x20;
    STX();


    while(RFTXRXIF == 0);
    RFTXRXIF = 0;
    RFD = length;

    // Inserting data
    for(i = 0 ; i < length ; i++){
        while(RFTXRXIF == 0);
        RFTXRXIF = 0;
        RFD = pData[i];
    }

   // Waitting for transmission to finish
   while(!(RFIF & IRQ_DONE));
   RFIF &= ~IRQ_DONE;
   
   return i;
}

