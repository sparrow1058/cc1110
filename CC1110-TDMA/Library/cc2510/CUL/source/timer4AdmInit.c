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

Filename:     timer4AdmInit.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "cul.h"

//Protos:
__interrupt void P1_IRQ(void);


volatile TIMER4_TABLE_ENTRY timer4Table[TIMER_ADM_TABLE_LENGTH];
BYTE k = 0x00;

//-----------------------------------------------------------------------------
// See cul.h for a description of this function.
//-----------------------------------------------------------------------------
void culTimer4AdmInit(void){
   //BYTE res = 0x00;

   // Clearing the table
   memset((void*)timer4Table, 0, sizeof(timer4Table));

   // Erasing old Timer 4 interrupt flag
   TIMIF &= ~0x38;
   T4IF = 0;

   // Initialising timer 4
   TIMER34_INIT(4);

   halSetTimer34Period(4, 500);
   // Setting channel 0 output pin to toggle at each timeout.
   T4CCTL0 = 0x14;
   // No operation on channel 1
   T4CCTL1 = 0x00;


   // Configuring pin settings:
   // Rising edge on P1 gives interrupt.
   PICTL &= ~0x02;
   // Setting pin for peripheral operation.
   P1SEL = 0x01;
   // Clearing P1 interrupt flags.
   P1IFG &= ~0x01;


   // Enabling interrupt from P1_0
   P1IEN = 0x01;

   INT_SETFLAG(INUM_P1INT, INT_CLR);
   INT_ENABLE(INUM_P1INT, INT_ON);
   //---------------------------------------------------------------



   // Turning on timer
   TIMER4_RUN(TRUE) ;
} // ends culTimer4AdmInit(...)






//-----------------------------------------------------------------------------
// T4_Adm_IRQ
//
//  Description:
//      This interrupt routine is run each time timer 4 generates an interrupt
//      request (every m-second). The table is parsed through, and the counters
//      are incremented. If a counter is equal to the timeout value in the table,
//      the _callBackFunction_ is called and the counter is reset.
//      At the end, the interrupt flags are cleared.
//-----------------------------------------------------------------------------
/*
#pragma vector=T4_VECTOR
__interrupt void T4_Adm_IRQ (void){
BYTE counterValue;

INT_GLOBAL_ENABLE(INT_OFF);

for(k = 0; k < TIMER_ADM_TABLE_LENGTH; k++){
if(timer4Table[k].timeout){
counterValue = ++timer4Table[k].counter;
if(counterValue == timer4Table[k].timeout){
timer4Table[k].counter = 0;
if(timer4Table[k].callBackFunction){
timer4Table[k].callBackFunction();
                }
            }
        }
    }

TIMIF &= ~0x38;         // Clearing interrupt source flag
INT_GLOBAL_ENABLE(INT_ON);
}   // ends T4_Adm_IRQ

*/


#pragma vector=P1INT_VECTOR
__interrupt void P1_IRQ(void){
   BYTE counterValue;

   EA = FALSE;
   INT_SETFLAG(INUM_P1INT, FALSE);
   if (P1IFG & 0x01)
   {
      P1IFG &= ~0x01;

      for(k = 0; k < TIMER_ADM_TABLE_LENGTH; k++){
         if(timer4Table[k].timeout){
            counterValue = ++timer4Table[k].counter;
            if(counterValue == timer4Table[k].timeout){
               timer4Table[k].counter = 0;
               if(timer4Table[k].callBackFunction){
                  timer4Table[k].callBackFunction();
               }
            }
         }
      }

   }
   if (P1IFG & 0x02)
   {
      P1IFG &= ~0x02;
   }
   if (P1IFG & 0x04)
   {
      P1IFG &= ~0x04;
   }
   if (P1IFG & 0x08)
   {
      P1IFG &= ~0x08;
   }
   if (P1IFG & 0x10)
   {
      P1IFG &= ~0x10;
   }
   if (P1IFG & 0x20)
   {
      P1IFG &= ~0x20;
   }
   if (P1IFG & 0x40)
   {
      P1IFG &= ~0x40;
   }
   if (P1IFG & 0x80)
   {
      P1IFG &= ~0x80;
   }

   EA = TRUE;
}
