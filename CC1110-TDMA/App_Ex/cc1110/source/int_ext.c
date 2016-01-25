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

Filename:     int_ext.c
Target:       cc2510
Author:       kja
Revised:      20/6-2006
Revision:     1.0

Description:
    Shows how to enable external interrupts. Pushbutton S1 is used.

******************************************************************************/

#include "app_ex.h"

// Prototypes
void external_int_main(void);
void external_int_P0_IRQ(void);


#define BUTTON_PUSH_IF_MASK     0x02


/******************************************************************************
* @fn  external_int_main
*
* @brief
*     Main function.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
   void external_int_main(void){
#else
   void main(void){
#endif

   //Initializing LED, button S1 and LCD
   INIT_GLED();
   INIT_BUTTON();
   initLcd();


   // Setting up and enabling external interrupt
   P0IFG = 0x00;
   PICTL |= 0x08;                  // Enable interrupt from P0_1 low
   PICTL |= 0x01;                  // Falling edge gives interrupt
   INT_ENABLE(INUM_P0INT, INT_ON); // Enabling interrupt from P0
   INT_GLOBAL_ENABLE(TRUE);

   // Informing to push S1
   lcdUpdate((char*)"Press S1 ",(char*)"");

   while( !stopApplication() );

   INT_GLOBAL_ENABLE(FALSE);
   return;
}


/******************************************************************************
* @fn  external_int_P0_IRQ
*
* @brief
*     Interrupt handler for external interrupt from P0.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void external_int_P0_IRQ(void){
#else
#pragma vector=P0INT_VECTOR
__interrupt void P0_IRQ(void){
#endif
   // check if PUSH_BUTTON is pressed
   if(P0IFG & BUTTON_PUSH_IF_MASK){
      GLED ^= 1;
      P0IFG &= ~BUTTON_PUSH_IF_MASK;
   }
   P0IF = FALSE;
}


/******************************************************************************
* @fn  external_int_init
*
* @brief
*     Initializes external interrupt application example.
*
* Parameters:
*
* @param  APPLICATION *a
*         Main application
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void external_int_init(APPLICATION *a)
{
   a->menuText = (char*)"External";
   a->description = (char*)"Interrupt";
   a->main_func = external_int_main;
   a->interrupts[INUM_P0INT] = external_int_P0_IRQ;
}
#endif
