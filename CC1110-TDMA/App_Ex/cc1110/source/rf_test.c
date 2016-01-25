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

Filename:     rf_test.c
Target:       cc2510
Author:       efu
Revised:      20/6-2006
Revision:     1.0

Description:
    This file provides 4 small tests which demonstrates use of the radio.

******************************************************************************/


#include "app_ex.h"
#include "cul.h"
#include "pingPong.h"
#include "menu.h"
#include "string.h"
#ifdef CC1110_TDMA_RX
#include "mydefine.h"
#endif


#define ADDRESS_0 0x01
#define ADDRESS_1 0x02

#define RECEIVE_TIMEOUT               1200

void receiveMode(void);

void contionuousMode(void);

UINT8 myAddr;
UINT8 remoteAddr;
BOOL remoteAddressSet = FALSE;
BOOL myAddressSet = FALSE;
UINT32 frequency;



void Single_More_TDMA(char sell)
{
  if(sell == 0)
  {
    myAddr = ADDRESS_1;
    remoteAddr = ADDRESS_0;
    radioInit(frequency, myAddr);
    INT_GLOBAL_ENABLE(INT_ON);
    contionuousMode();
  }
  else if(sell == 1)
  {
    myAddr = ADDRESS_0;
    remoteAddr = ADDRESS_1;
    halWait(10);
    radioInit(frequency, myAddr);
    INT_GLOBAL_ENABLE(INT_ON);
    halWait(10);
    receiveMode();
   }
}

/******************************************************************************
* @fn  contionuousMode
*
* @brief
*      Function for transferring packets until stopped by user.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void contionuousMode(void)
{
#ifdef CC1110_TDMA_TX
   BOOL res;
   INT8U count;
   BYTE* receiveBuffer;

   BYTE RXbuff_command[7];

   BYTE length;
   INT8U Key = 0;
   BYTE sender;
   BYTE sendBuffer[5];
   P1DIR = 0xFF;
   sendBuffer[0] = 0x40;
   sendBuffer[1] = 0x02;
#ifdef CC1110_TDMA_TX1
   sendBuffer[2] = 0x01;
#endif
#ifdef CC1110_TDMA_TX2
   sendBuffer[2] = 0x02;
#endif
   sendBuffer[4] = 0x0D;
   P1DIR |= 0x02;
   P1INP |= ~0xF3;
   P1DIR &= 0xF3;
   while(1)
   {
      if(P1_2 == 0)
      {
        while(P1_2 == 0);
        while(P1_2 == 0);
        Key = '1';
      }
      while(Key != 0)
      {
          res = radioReceive(&receiveBuffer, &length, RECEIVE_TIMEOUT, &sender);
          RXbuff_command[0] = *receiveBuffer++;
          RXbuff_command[1] = *receiveBuffer++;
          RXbuff_command[2] = *receiveBuffer++;
          sendBuffer[3] = Key;
    #ifdef CC1110_TDMA_TX1
          if((res == TRUE)&&(RXbuff_command[0] == '1')&&(RXbuff_command[2] == 'B'))
    #endif
    #ifdef CC1110_TDMA_TX2
          if((res == TRUE)&&(RXbuff_command[0] == '2')&&(RXbuff_command[2] == 'B'))
    #endif
          {
            for(int i = 0;i<7;i++)
            RXbuff_command[i] = 0;
            P1 = ~P1;
            res = radioSend(sendBuffer, sizeof(sendBuffer), remoteAddr, DO_ACK);
            if(res == TRUE)
            {
              Key = 0;
               if(++count > 1)
               {
                  RLED = LED_ON;
                  Print(4,10,"     OK     ",1);
                  if(count >=250)
                  {
                      count = 2;
                  }
                  break;
               }
            }
               else
               {
                count++;
                if(count >=250)
                {
                  count = 2;
                  break;
                }
            }
         }
      }
   }
#endif
}


/******************************************************************************
* @fn  receiveMode
*
* @brief
*      Function for receiving data.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
extern void UartTX_Send_String(char *Data,int len);
unsigned char number1 = 0,number2 = 0,number[4];
void receiveMode(void)
{
#ifdef CC1110_TDMA_RX
   unsigned char temp2[5] = {0x68,0xaa,'U','Y','J'};
#endif
   BYTE* receiveBuffer;
   BYTE RXbuffer[5];
   BYTE sendBuffer1[] = "1:Begin";
   BYTE sendBuffer2[] = "2:Begin";
   BYTE length;
   BYTE res;
   BYTE sender;
   int type;
   int Receive_COUT = 5;
   P1DIR = 0xFF;
   type = 0;
   while(1)
   {
         if(type >= 2)
         type = 1;
         else
         type++;
         Receive_COUT = 3;
     while(Receive_COUT--)
     {
         if(type == 1)
         {
          res = radioSend(sendBuffer1, sizeof(sendBuffer1), remoteAddr, DO_ACK);
         }
         else
          res = radioSend(sendBuffer2, sizeof(sendBuffer2), remoteAddr, DO_ACK);
         if(res == TRUE)
         {
          res = radioReceive(&receiveBuffer, &length, 30, &sender);
          if(res == TRUE)
          {
             RLED = !RLED;//LED_ON;
             RXbuffer[0] = *receiveBuffer++;
             RXbuffer[1] = *receiveBuffer++;
             RXbuffer[2] = *receiveBuffer++;
             RXbuffer[3] = *receiveBuffer++;
             if((RXbuffer[0] == 0x40)&&(RXbuffer[1] == 0x02))
             {P1 = ~P1;}
            if(RXbuffer[2] == 0x01)
            {
              number1++;
              number[0] = number1/10/10/10+48;
              number[1] = number1/10/10%10+48;
              number[2] = number1/10%10+48;
              number[3] = number1%10+48;
#ifdef CC1110_TDMA_RX
            Print_word(number,7,79,4);
            UartSendWord(temp2,5);
            UartSendWord((void*)di_yz,sizeof(di_yz));
#endif
            }
            else
            {
              number2++;
              number[0] = number2/10/10/10+48;
              number[1] = number2/10/10%10+48;
              number[2] = number2/10%10+48;
              number[3] = number2%10+48;
#ifdef CC1110_TDMA_RX
            Print_word(number,7,0,4);
            UartSendWord(temp2,5);
            UartSendWord((void*)di_yz,sizeof(di_yz));
#endif
            }
             break;
          }
          else
          {
             RLED = LED_OFF;
          }
          RLED = LED_OFF;
        }
     }
   }
}

