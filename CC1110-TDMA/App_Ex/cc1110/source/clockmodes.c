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

Filename:     clockmodes.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0

Description:
    This application shows how to switch between the main clock sources.
    The green LED toggles according to clock speed.

******************************************************************************/
#include "app_ex.h"

// Prototypes
void blinkLeds(void);
void initClockmodes(void);
void clockmodes_main(void);


/******************************************************************************
* @fn  initClockmodes
*
* @brief
*      Initializes components for the clock mode application example.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void initClockmodes(void)
{
   DISABLE_ALL_INTERRUPTS();

   SLEEP &= ~0x04;
   halWait(1);

   initLcd();

   P1SEL &= ~0x01; // may have been set by another app
   INIT_GLED();
}


/******************************************************************************
* @fn  blinkLeds
*
* @brief
*      Toggle LEDs according to clock speed.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void blinkLeds(void)
{
   static BYTE counter = 0;
   //Blinking LEDs.
   UINT16 timeOut = 0xFFFF;
   while(timeOut--){
      asm("NOP");
   }

   counter++;

   GLED ^= 1;
} //ends blink LEDs


/******************************************************************************
* @fn  clockmodes_main
*
* @brief
*      Main function, switches between RCosc and Xosc.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void clockmodes_main(void){
#else
void main(void){
#endif
   BYTE state = 0;
   char *menuText[] = {(char*)" RC 13 MHz", (char*)" Crystal 26 MHz"};


   initClockmodes();

   while((state = lcdMenu(menuText,2)) != ABORT_MENU)
   {
      if(state == 0)
      {
         //Turning on 16 MHz RC oscillator and waiting for HFRC stable
         SET_MAIN_CLOCK_SOURCE(RC);
         while (!HIGH_FREQUENCY_RC_OSC_STABLE);
         lcdUpdate((char*)"Internal 13 MHz",(char*)"RC Osc.");
      }
      else if(state == 1)
      {
         //Turning on 32 MHz Crystal oscillator and waiting for it to become stable
         SET_MAIN_CLOCK_SOURCE(CRYSTAL);
         //CLKCON = 0x00;
         while (!XOSC_STABLE);
         // SET_MAIN_CLOCK_SPEED(MHZ_32);
         lcdUpdate((char*)"External 26 MHZ",(char*)"X-stal Osc.");
      }


      while(!stopApplication())
      {
         blinkLeds();
      }


   }
}


/******************************************************************************
* @fn  clockmodes_init
*
* @brief
*      Initializes the clock modes application example.
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
void clockmodes_init(APPLICATION *a)
{
   a->menuText = (char*)"Clock Modes";
   a->description = (char*)"";
   a->main_func = clockmodes_main;
}
#endif
