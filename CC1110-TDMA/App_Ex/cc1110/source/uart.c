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

Filename:     uart.c
Target:       cc2510
Author:       kja
Revised:      20/6-2006
Revision:     1.0

Description:
    Reads in data written through the UART. When ENTER is pressed, the data is
    written back. ESC ends the session.

    Set up hyperterminal for 57600 kBaud transfer.
    Use 8 bits, no flow control and no parity bit.

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "app_ex.h"
#include "lcd128_64.h"


#define BUFFER_SIZE 32

typedef struct{
   UINT8 pointer;
   char text[BUFFER_SIZE];
} BUFFER;

BUFFER buffer;

void initUART(void);
void uart_main(void);


/******************************************************************************
* @fn  initUART
*
* @brief
*      Initializes components for the UART application example.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void initUART(void)
{
   // Setup for UART0
   IO_PER_LOC_UART0_AT_PORT0_PIN2345();

   SET_MAIN_CLOCK_SOURCE(CRYSTAL);

   UART_SETUP(0, 57600, HIGH_STOP);

   UTX0IF = 1;

 //  initLcd();
}


/******************************************************************************
* @fn  uart_main
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
void uart_main(void)
{
#else
void main(void){
#endif
   char c = 0;
   UINT8 i;

   char arr1[17] = "";
   initUART();

   memset(buffer.text, ' ', BUFFER_SIZE);
   buffer.pointer = 0;
   ClearScreen();
   Print(0,10,"  --UART--",1);
   if(LanguageSel == 1)
   {
       Print(2,10,"Plesae input:",1);
   }
   else
   {
       Print(2,8,"«Î¥”¥Æø⁄ ‰»Î:",1);
   }
 //  Print6(1,20,"57600 8-N-1",1);

   Rectangle(2 , 4 , 127 , 7);
   printf((char*)"\n\nType text\n");
   printf((char*)"ENTER to readback text to hyperterminal\n");
   printf((char*)"BACKSPACE to delete last written character\n");
   printf((char*)"ESC to quit\n");

   while( !stopApplication()){
      switch ( c ){
      case ENTER:
         for(i = 0; i < BUFFER_SIZE; i++){
            printf((char*)"%c", buffer.text[i]);
         }
         printf((char*)"\n");

         memset(buffer.text, ' ', BUFFER_SIZE);
         buffer.pointer = 0;
         break;
      case BACK_SPACE:
         if (buffer.pointer > 0){
            buffer.pointer--;
            buffer.text[buffer.pointer] = ' ';
         }
         break;
      case ESC:
         break;
      default:
         if (buffer.pointer < BUFFER_SIZE){
            buffer.text[buffer.pointer] = c;
            buffer.pointer++;
         }
         break;
      }
      memcpy(arr1,&buffer.text[0],16);
      arr1[16] = '\0';
      Print(5,3,(INT8U*)arr1,1);
   }
}


/******************************************************************************
* @fn  uart_init
*
* @brief
*      Initializes the UART application example.
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
void uart_init(APPLICATION *a)
{
   a->menuText = (char*)"UART <--> LCD";
   a->description = (char*)"57600 8-N-1";
   a->main_func = uart_main;
}
#endif
