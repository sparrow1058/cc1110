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

Filename:     random.c
Target:       cc2510
Author:       kja
Revised:      20/6-2006
Revision:     1.0

Description:
    This application shows how to create a random sequence of bytes. The sequence
    is displayed on the LCD and used to toggle the green and yellow LEDs.

******************************************************************************/

#include "app_ex.h"

void initRandom(void);
void random_main(void);


/******************************************************************************
* @fn  initRandom
*
* @brief
*      Initializes the components for the random generator application example.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void initRandom(void)
{
   DISABLE_ALL_INTERRUPTS();

      halInitRandomGenerator();

   initLcd();

   INIT_GLED();
   INIT_YLED();
}


/******************************************************************************
* @fn  random_main
*
* @brief
*      Main function.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void random_main(void){
#else
void main(void){
#endif
   BYTE randomChar;
   UINT8 i, rand;

   initRandom();

   lcdUpdate((char*)"Random text:", (char*)"");

   while( !stopApplication() ){
      //read random register
      for(i = 0; (i < LINE_SIZE); i++)
      {
         GET_RANDOM_BYTE(rand);

         SET_LED_MASK( rand );

         // randChar is equal a random value between 'a' and 'z'
         // in the ascii table
         randomChar = 'a' + (rand % ('z' - 'a')) ;
         lcdUpdateChar(LINE2, i, randomChar);

         halWait(0x0F);
      }

      LED1 = LED3 = LED_OFF;
   }
   return;
}


/******************************************************************************
* @fn  clockmodes_init
*
* @brief
*      Initializes the random generator application example.
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
void random_init(APPLICATION *a){
   a->menuText = (char*)"Random";
   a->description = (char*)"Sequence";
   a->main_func = random_main;
}
#endif
