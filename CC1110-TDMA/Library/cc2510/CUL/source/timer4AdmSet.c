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

Filename:     timer4AdmSet.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/

#include "cul.h"

extern volatile TIMER4_TABLE_ENTRY timer4Table[TIMER_ADM_TABLE_LENGTH];

//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
BYTE culTimer4AdmSet(DWORD timeout, FUNCTION* callBackFunction){
   BYTE status = 0x00;
   BYTE i = 0x00;

   // Checking the arguments...
   if((timeout == 0) || (callBackFunction == NULL)){
      return 0xFF;
   }

   status = IEN0;
   INT_GLOBAL_ENABLE(INT_OFF);

   // Searching for an available entry in the table.
   for(i = 0; i < TIMER_ADM_TABLE_LENGTH; i++){
      if(timer4Table[i].timeout == 0){

         // Storing the interrupt enable register, and turning off interrupts

         // Setting up the table.
         timer4Table[i].timeout = timeout;
         timer4Table[i].counter = 0;
         timer4Table[i].callBackFunction = callBackFunction;
         break;
      }
   }

   // Restoring the interrupt enable status.
   IEN0 = status;

   if(i < TIMER_ADM_TABLE_LENGTH )
   {
      return i;
   }
   else
   {
      // No available entry in the table, returning error value
      return 0xFF;
   }
}   // ends culTimer4AdmSet(...)
