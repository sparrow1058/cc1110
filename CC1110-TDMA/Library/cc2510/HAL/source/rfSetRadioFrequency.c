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

Filename:     rfSetRadioFrequency.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include "hal.h"

//-----------------------------------------------------------------------------
// See hal.h for a description of this function.
//-----------------------------------------------------------------------------
void halRfSetRadioFrequency(UINT32 frequency)
{

   frequency = (frequency << 10);
   frequency /= 1000;
   frequency = (frequency << 6);
   frequency /= 26;
   FREQ0 = (BYTE) frequency;
   frequency >>= 8;
   FREQ1 = (BYTE) frequency;
   frequency >>= 8;
   FREQ2 = (BYTE) frequency;
   return;
}
