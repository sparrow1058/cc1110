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

Filename:     string_lib.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0

Description:


******************************************************************************/
#include "app_ex_util.h"
#include "app_ex.h"

#define ARRAY_LENGTH         5
#define UART_ENABLE_RECEIVE         0x40

/******************************************************************************
* @fn  getNumber
*
* @brief
*     Function for reading a number in ASCII format from the UART and
*     converting it to a word.
*
* Parameters:
*
* @param  void
*
* @return INT16
*         Return integer
*
******************************************************************************/
INT16 getNumber(void)
{
  char array[ARRAY_LENGTH];
  BYTE i = 0;

  while(i<ARRAY_LENGTH)
  {
    array[i] = getkey();
    putchar(array[i]);

    if(array[i] == ENTER)
    {
      i = ARRAY_LENGTH;
      putchar('\n');

    }
    else if((array[i] < '0') || (array[i] > '9'))
    {
      printf((char*)"\n Please enter a number!!\n");
      i = 0;
    }
    else
    {
        i++;
    }
  }
  return atoi(array);
}


/******************************************************************************
* @fn  uartGetkey
*
* @brief
*     Function for reading a character from the UART.
*
* Parameters:
*
* @param  void
*
* @return char
*         Character returned from UART
*
******************************************************************************/
char uartGetkey (void)
{
   char c;
   BYTE status;

   // Turning on reception
   status = U0CSR;
   U0CSR |= UART_ENABLE_RECEIVE;

   while (!URX0IF && !stopApplication());
   c = U0DBUF;
   URX0IF = FALSE;

   // Restoring old state
   U0CSR = status;

   return c;
}
