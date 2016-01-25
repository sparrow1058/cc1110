/******************************************************************************
*                                                                             *
*        **********                                                           *
*       ************                                                          *
*      ***        ***                                                         *
*      ***   +++   ***                                                        *
*      ***   + +   ***                                                        *
*      ***   +                            CHIPCON                             *
*      ***   + +   ***                                                        *
*      ***   +++   ***                                                        *
*      ***       ***                                                          *
*       ***********                                                           *
*        *********                                                            *
*                                                                             *
*******************************************************************************

Filename:     chipconLogo.c
Target:       cc2510
Author:       KJA
Revised:      20/6-2006
Revision:     1.0

Description:
              This function shows the Chipcon logo in the LCD display.

******************************************************************************/
#include "lcd.h"
#include <string.h>

// Prototypes
void showLogo(void);

// Char1  Char2  Char3
// -----  -****  -----	
// ----*  *****  *----
// ---**  *----  ***--
// --**-  -----  -***-
// -***-  -----  ---**
// -**--  --***  *---*
// ***--  -*---  -*---
// **---  *----  -----	
//		
//	**---  *----  -----
//	***--  -*---  -*---
//	-**--  --***  *---*
//	-***-  -----  ---**
//	--**-  -----  -***-
//	---**  *----  ***--
//	----*  *****  *----
//	-----  -****  -----
// Char4  Char5  Char6


void showLogo(void){
   char chipconLogo[6][8] ={
      {0x00, 0x01, 0x03, 0x06, 0x0E, 0x0C, 0x1C, 0x18}, //char1
      {0x0F, 0x1F, 0x10, 0x00, 0x00, 0x07, 0x08, 0x10}, //char2
      {0x00, 0x10, 0x1C, 0x0E, 0x03, 0x10, 0x08, 0x00}, //char3
      {0x18, 0x18, 0x0C, 0x0E, 0x06, 0x03, 0x01, 0x00}, //char4
      {0x10, 0x08, 0x07, 0x00, 0x00, 0x10, 0x1F, 0x0F}, //char5
      {0x00, 0x08, 0x10, 0x03, 0x0E, 0x1C, 0x10, 0x00}  //char6
   };

   UINT8 i;
   char logo[2][16];

   //init new symbols
   for(i = 0; i < 6; i++){
      initNewSymbol( &chipconLogo[i][0], CHAR1_ADDRESS + (i*0x08));
   }

 	logo[0][0] = 0x01;
	logo[0][1] = 0x02;
	logo[0][2] = 0x03;
   logo[0][3] = ' ';
   strcpy(&logo[0][3], (char*)"   Chipcon   ");

   logo[1][0] = 0x04;
	logo[1][1] = 0x05;
	logo[1][2] = 0x06;
   logo[1][3] = ' ';


   strcpy(&logo[1][3], (char*)"   CC2510     ");


   lcdUpdate(logo[0], logo[1]);
}
