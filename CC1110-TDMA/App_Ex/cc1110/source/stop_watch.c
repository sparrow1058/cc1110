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

Filename:     stop_watch.c
Target:       cc2510
Author:       kja
Revised:      20/6-2006
Revision:     1.0

Description:
    This application shows a stopwatch. Use S1 on the EB card to start, stop
    and reset timer. Timer 3 is used for time-keeping.

******************************************************************************/

#include "app_ex.h"
#include "RF04EB.h"
#include "lcd128_64.h"

typedef enum {START_STATE, RUN_STATE, STOP_STATE} STATE;

typedef struct TIME_S{
   UINT8 h;
   UINT8 m;
   UINT8 s;
} TIME;

void initStopWatch(void);
void printTime(void);
void incrementTime(void);
void stop_watch_main(void);
void stop_watch_T3_IRQ(void);

UINT16 overflow;
TIME t;


/******************************************************************************
* @fn  initStopWatch
*
* @brief
*      Initializes components for the stopwatch application example.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void initStopWatch(void)
{
  //interrupts[INUM_T3] = stop_watch_T3_IRQ;
   INIT_GLED();

   SET_MAIN_CLOCK_SOURCE(CRYSTAL);
   CLKCON &= ~0x38;


   // Enabling overflow interrupt from timer 3
   TIMER34_INIT(3);
   halSetTimer34Period(3, 1000);
   INT_ENABLE(INUM_T3, INT_ON);
   TIMER34_ENABLE_OVERFLOW_INT(3,INT_ON);
   INT_GLOBAL_ENABLE(INT_ON);
}



/******************************************************************************
* @fn  stop_watch_main
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
void stop_watch_main(void){
#else
void main(void){
#endif
   STATE state = START_STATE;
   initStopWatch();
   TIMER3_RUN(FALSE);
   ClearScreen();
   Print(0,5,"--STOP WATCH--",1);

   Rectangle(2 , 4 , 108 , 7);

   while(!stopApplication()){
      switch (state)
      {
                case START_STATE:
                {
                        t.h = t.m = t.s = 0;
                        overflow = 0;
                        printTime();
                        if(LanguageSel == 1)
                        {
                                Print6(2,10," OK for START    ",1);
                        }
                        else
                        {
                                Print(2,8,"按OK键开始:",1);
                        }
                        if(ScanKey() == K_OK)
                        {
                                while(ScanKey() != 0xff);
                                halWait(5);
                                TIMER3_RUN(TRUE);
                                state = RUN_STATE;
                                if(LanguageSel == 1)
                                {
                                        Print6(2,10," OK for STOP   ",1);
                                }
                                else
                                {
                                        Print(2,8,"按OK键停止:",1);
                                }

                        }
                }break;

                case RUN_STATE:
                {
                        INT_GLOBAL_ENABLE(INT_OFF);

                        if(overflow > 0 && overflow < 0x09)
                        {
                                GLED = LED_ON;
                        }
                        else if(overflow > (UINT16)1000)
                        {
                                //overflow = 0;
                                overflow -= 1000;

                                incrementTime();
                                printTime();
                        }
                        else
                        {
                                GLED = LED_OFF;
                        }
                        if(ScanKey() == K_OK)
                        {
                                while(ScanKey() != 0xff);
                                halWait(5);
                                TIMER3_RUN(FALSE);
                                state = STOP_STATE;
                                GLED = LED_OFF;
                        }

                        INT_GLOBAL_ENABLE(INT_ON);
                }break;
                case STOP_STATE:
                {
                        printTime();
                        if(LanguageSel == 1)
                        {
                                Print6(2,10," Total time is:",1);
                        }
                        else
                        {
                                Print(2,8,"总计时间为:",1);
                        }
                        if(ScanKey() == K_OK)
                        {
                                while(ScanKey() != 0xff);
                                halWait(5);
                                state = START_STATE;
                        }
                }break;
                default:
                break;
        }
   }
   while(ScanKey() != 0xff);
   halWait(5);
   INT_GLOBAL_ENABLE(INT_OFF);
   return;
}


/******************************************************************************
* @fn  incrementTime
*
* @brief
*      Increments time with 1 second. Wraps around for 60s and 60min.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void incrementTime(void)
{
   if(t.s < 59){
      t.s++;
   }
   else{
      t.s = 0;
      if(t.m < 59){
         t.m++;
      }
      else{
         t.m = 0;
         t.h++;
      }
   }
}


/******************************************************************************
* @fn  printTime
*
* @brief
*      Prints current time to LCD.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void printTime(void)
{
   char s[9];
   s[0] = '0' + (t.h/10);
   s[1] = '0' + (t.h%10);
   s[2] = ':';
   s[3] = '0' + (t.m/10);
   s[4] = '0' + (t.m%10);
   s[5] = ':';
   s[6] = '0' + (t.s/10);
   s[7] = '0' + (t.s%10);
   s[8] = '\0'; //endline
   Print(5,25,(INT8U *)s,1);
}


/******************************************************************************
* @fn  stop_watch_T3_IRQ
*
* @brief
*     Interrupt handler for timer T3 interrupts.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
   void stop_watch_T3_IRQ(void){
#else
#pragma vector=T3_VECTOR
__interrupt void stop_watch_T3_IRQ(void){
#endif
   if(TIMIF & 0x01){ //T3 overflow
      overflow++;
   }
   TIMIF &= ~0x01;
}


/******************************************************************************
* @fn  stop_watch_init
*
* @brief
*      Initializes the stopwatch application example.
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
void stop_watch_init(APPLICATION *a)
{
   a->menuText = (char*)"Stop Watch";
   a->description = (char*)"Timer 3";
   a->main_func = stop_watch_main;
   a->interrupts[INUM_T3] = stop_watch_T3_IRQ;
}
#endif
