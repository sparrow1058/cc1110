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

Filename:     setTimer34Period.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include "hal.h"


//------------------------------------------------------------------------------------------------------
// See hal.h for a description of this function.
//------------------------------------------------------------------------------------------------------
BYTE halSetTimer34Period(BYTE timer, DWORD period){

        BYTE div = 0;

        if(TICKSPD > 5) { // Checking that the period is not too short.
        if( (period < 2*(TICKSPD-5)) && (period != 0) ){
               return 0;
           }
    }

        if(period == 0){  // If period is 0, max period length and max prescaler
                div = 7;  // division is used.
                period = 255;
        } else {
                period = ((period*26) >> TICKSPD);// Determining how many timer ticks the period consist of
                while(period > 255){              // If the period is too long, the prescaler division is
                        period = (period >> 1);   // increased.
                        div++;
                        if(div > 7){              // If the period is too long when using max prescaler division,
                                return 0;         // 0 is returned.
                        }
                }
        }

        if(timer == 4){
                // Timer 4 selected
                T4CTL |= (div << 5);              // Setting prescaler value
                T4CC0 = (BYTE) period;            // Setting timer value.
        } else if(timer == 3){
                // Timer 3 selected
                T3CTL |= (div << 5);              // Setting prescaler value
                T3CC0 = (BYTE) period;            // Setting timer value.
        } else {return 0;}

        return period;

}
