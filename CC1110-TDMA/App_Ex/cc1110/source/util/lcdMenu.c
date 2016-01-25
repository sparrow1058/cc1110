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

Filename:     lcdMenu.c
Target:       cc2510
Author:       KJA
Revised:      20/6-2006
Revision:     1.0

Description:
   This function displays a menu on the LCD. Use joystick UP/ DOWN to navigate
   and RIGHT to accept.

   _text_ is a pointer to an array of strings. Each string must have
   one space in front of the menu text.

******************************************************************************/
#include "lcd.h"
#include "app_ex_util.h"

UINT8 lcdMenu(char *text[], const UINT8 n){
   JOYSTICK_DIRECTION jsv;
   UINT8 active = 0;
   UINT8 line1  = 0;


   BOOL updateLcd = TRUE;

   while(getJoystickDirection() != CENTRED);

   while (TRUE){
      jsv = getJoystickDirection();

      //while(getJoystickDirection() != CENTRED);

      if(updateLcd){
         if(n > 1){
            lcdUpdate(text[line1], text[line1 + 1]);
         }
         else{
            lcdUpdate(text[line1], (char*)"");
         }


         if(line1 > 0)
            lcdUpdateChar(LINE1, 15, ARROW_UP);
         if(line1 + 1 < n - 1)
            lcdUpdateChar(LINE2, 15, ARROW_DOWN);

         if(active == line1){
            lcdUpdateChar(LINE1,0,ARROW_RIGHT);
         }
         else{
            lcdUpdateChar(LINE2,0,ARROW_RIGHT);
         }

         while((jsv = getJoystickDirection()) != CENTRED);
         updateLcd = FALSE;
      }

      switch (jsv){
      case UP:
         if(active > 0){
            active--;
            if (line1 != 0){
               line1--;
            }
            updateLcd = TRUE;
         }
         break;
      case DOWN:
         if(active < n-1){
            active++;
            if (line1 < n-2){
               line1++;
            }
            updateLcd = TRUE;
         }
         break;
      case LEFT:
         while(getJoystickDirection() != CENTRED);
         return ABORT_MENU;
      case RIGHT:
         while(getJoystickDirection() != CENTRED);
         return active;
      default:
         break;
      }
   }
}
