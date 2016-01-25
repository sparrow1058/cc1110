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

Filename:     setTimer2Period.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include "hal.h"

//-----------------------------------------------------------------------------
// See hal.h for a description of this function.
//-----------------------------------------------------------------------------
int temp = 0;
BOOL halSetTimer2Period(UINT32 period, UINT8* cnt, UINT8* presc)
{
   BYTE tip = 0;
   UINT16 prescaler = 1;
   UINT16 counter;

   // Times 26 and devided by 64 (crystal clock frequency and minimum tick period of T2).
   period = (UINT32)((float)period * 0.40625);

   // Compensating for TICKSPD.
   period = (period >>  TICKSPD);


   while(period > 65280)
   {
      tip++;
      if(tip == 3)
      {  // Step from 256 to 1024 clock cycles
         period = period >> 1;
      }
      period = period >> 1;
   }

   if(tip > 3)
   {
      return FALSE;
   }

   while(((counter = (period / prescaler))  > 255))
   {
      prescaler++;
   }

   TIMER2_SET_COUNTER((UINT8)counter);
   TIMER2_SET_PRESCALER((UINT8) prescaler);
   TIMER2_SET_TICK_PERIOD(tip);

   *cnt = (UINT8) counter;
   *presc = (UINT8) prescaler;
if(++temp > 10)
{
  temp = 0;
   return TRUE;
}
else
return FALSE;

}
