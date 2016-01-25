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

Filename:     setTimer1Period.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include "hal.h"


//-----------------------------------------------------------------------------
// See hal.h for a description of this function.
//-----------------------------------------------------------------------------
WORD halSetTimer1Period(DWORD period){

   BYTE div = 0;

   // Checking that the period is not too short for the timer tick interval.
   if(TICKSPD > 5) {
      if( (period < 2*(TICKSPD-5)) && (period != 0) ){
         return 0;
      }
   }

   if(period == 0){  // Max timer period is used.
      div = 3;
      period = 0x0000FFFF;
   }
   else{
      period = ((period*26) >> TICKSPD); // Calculating number of timer ticks the period
      if(period&0xFFFF0000){             // Using prescaler to fit period
         period = (period >> 3);
         div = 1;
         while (period&0xFFFF0000){
            period = (period >> 2);
            div++;
            if(div > 3){                 // If the period is too long, 0 is returned.
               return 0;
            }
         }
      }
   }

   T1CTL = ((T1CTL&~0x0c) | (div << 2)); // Setting prescaler division value

   T1CC0L = (BYTE)(period);              // Setting counter value
   T1CC0H = (BYTE)(period >> 8);

   return (WORD)period;
}
