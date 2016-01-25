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

Filename:     pingPong.h
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0

Description:
   Contains definitions of the functions for the Ping-pong animation sequence.

******************************************************************************/

#ifndef PINGPONG_H
#define PINGPONG_H

#include "lcd.h"

typedef struct{
    INT8 lcdPosition;
    INT8 state;
    BYTE direction;
} BALL_DATA;


#define RIGHTWARDS      0x01
#define LEFTWARDS       0x02
#define ORIENTED_LEFT   0x04
#define ORIENTED_RIGHT  0x08


void pingPongInit(void);
void receiveBall(BYTE);
void sendBall(BYTE);
void updateBall(BALL_DATA* ballData);


#endif
