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

Filename:     pingPong.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0

Description:
   Implementations of the functions for the Ping-pong animation sequence.

******************************************************************************/

#include "pingPong.h"


void pingPongInit(void)
{
   BYTE i;
   char pingPongSymbols[8][8] ={
      {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03},
      {0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F},
      {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}
   };

   for(i=0;i<8;i++)
   {
      initNewSymbol(&(pingPongSymbols[i][0]), (i<<3));
   }
}


void receiveBall(BYTE orientation)
{
   BALL_DATA ballData;
   BYTE i;
   lcdUpdate((char*)"",(char*)"");


   if(orientation == ORIENTED_RIGHT)
   {
      lcdUpdateSymbol(LINE1,9,0x00);
      lcdUpdateSymbol(LINE2,9,0x00);
      ballData.direction = LEFTWARDS;
      ballData.lcdPosition = 16;
      ballData.state = 1;
   }
   else
   {
      lcdUpdateSymbol(LINE1,6,0x07);
      lcdUpdateSymbol(LINE2,6,0x07);
      ballData.direction = RIGHTWARDS;
      ballData.lcdPosition = -1;
      ballData.state = 1;
   }


   for(i=0;i<12;i++)
   {
      updateBall(&ballData);
   }
   return;
}


void updateBall(BALL_DATA* ballData)
{
   if(ballData->direction & RIGHTWARDS)
   {
      if(ballData->state == 0)
      {

         //      -- --       -- --
         //      -* *-   ->  -- **
         //      -* *-       -- **
         //      -- --       -- --
         if(ballData->lcdPosition >= 0)
         {
            lcdUpdateChar(LINE1,ballData->lcdPosition,' ');
            lcdUpdateChar(LINE2,ballData->lcdPosition,' ');
         }
         ballData->lcdPosition++;
         lcdUpdateSymbol(LINE1,ballData->lcdPosition,0x06);
         lcdUpdateSymbol(LINE2,ballData->lcdPosition,0x01);
         ballData->state = 1;
         waitVisible(BEARLY_VISIBLE);
      }
      else
      {
         //      -- --       -- --
         //      ** --   ->  -* *-
         //      ** --       -* *-
         //      -- --       -- --
         if(ballData->lcdPosition >= 0)
         {
            lcdUpdateSymbol(LINE1,ballData->lcdPosition,0x05);
            lcdUpdateSymbol(LINE2,ballData->lcdPosition,0x02);
         }
         lcdUpdateSymbol(LINE1,ballData->lcdPosition+1,0x04);
         lcdUpdateSymbol(LINE2,ballData->lcdPosition+1,0x03);
         ballData->state = 0;
         waitVisible(BEARLY_VISIBLE);
      }
   }
   else
   {
      if(ballData->state == 0)
      {
         //      -- --       -- --
         //      ** --   <-  -* *-
         //      ** --       -* *-
         //      -- --       -- --
         if(ballData->lcdPosition >= 0)
         {
            lcdUpdateChar(LINE1,ballData->lcdPosition+1,' ');
            lcdUpdateChar(LINE2,ballData->lcdPosition+1,' ');
         }
         lcdUpdateSymbol(LINE1,ballData->lcdPosition,0x06);
         lcdUpdateSymbol(LINE2,ballData->lcdPosition,0x01);
         ballData->state = 1;
         waitVisible(BEARLY_VISIBLE);
      }
      else
      {

         //      -- --       -- --
         //      -* *-   <-  -- **
         //      -* *-       -- **
         //      -- --       -- --
         lcdUpdateSymbol(LINE1,ballData->lcdPosition,0x04);
         lcdUpdateSymbol(LINE2,ballData->lcdPosition,0x03);
         ballData->lcdPosition--;
         if(ballData->lcdPosition >= 0)
         {
            lcdUpdateSymbol(LINE1,ballData->lcdPosition,0x05);
            lcdUpdateSymbol(LINE2,ballData->lcdPosition,0x02);
         }
         ballData->state = 0;
         waitVisible(BEARLY_VISIBLE);
      }
   }
}


void sendBall(BYTE orientation)
{
   BALL_DATA ballData;
   BYTE i;
   lcdUpdate((char*)"",(char*)"");

   if(orientation == ORIENTED_RIGHT)
   {
      lcdUpdateSymbol(LINE1,9,0x00);
      lcdUpdateSymbol(LINE2,9,0x00);
      ballData.direction = RIGHTWARDS;
      ballData.lcdPosition = 10;
      ballData.state = 1;
      lcdUpdateSymbol(LINE1,10,0x05);
      lcdUpdateSymbol(LINE2,10,0x02);
   }
   else
   {
      lcdUpdateSymbol(LINE1,6,0x07);
      lcdUpdateSymbol(LINE2,6,0x07);
      ballData.direction = LEFTWARDS;
      ballData.lcdPosition = 5;
      ballData.state = 1;
      lcdUpdateSymbol(LINE1,5,0x06);
      lcdUpdateSymbol(LINE2,5,0x01);
   }


   for(i=0;i<11;i++)
   {
      updateBall(&ballData);
   }

   if(orientation == ORIENTED_RIGHT)
   {
      lcdUpdateChar(LINE1,15,' ');
      lcdUpdateChar(LINE2,15,' ');
   }
   else
   {
      lcdUpdateChar(LINE1,0,' ');
      lcdUpdateChar(LINE2,0,' ');
   }

   return;
}
