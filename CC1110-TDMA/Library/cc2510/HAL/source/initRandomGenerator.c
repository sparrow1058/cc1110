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

Filename:     initRandomGenerator.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include"hal.h"
#define FREQUENCY_4_CC1110       433500     // radioConfigure() in

//-----------------------------------------------------------------------------
// See hal.h for a description of this function.
//-----------------------------------------------------------------------------
void halInitRandomGenerator(void)
{


   BYTE i;
   //debug:
   BYTE volatile test;

   halRfConfig(FREQUENCY_4_CC1110);

   // Turning on receiver to get output from IF-ADC.
   SRX();
   halWait(1);

   ENABLE_RANDOM_GENERATOR();

   for(i = 0 ; i < 32 ; i++)
   {
      RNDH = RSSI;
      CLOCK_RANDOM_GENERATOR();
   }

   return;
}

