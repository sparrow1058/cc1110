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

Filename:     int_timer.c
Target:       cc2510
Author:       kja
Revised:      20/6-2006
Revision:     1.0

Description:
    Timer 1,2 and 3 are configured to run and generate interrupts. Each timer
    is associated with an asterisk on the LCD.

******************************************************************************/
#include "app_ex.h"

// Prototypes
void int_timer_main(void);
void int_timer_init(APPLICATION *a);
void int_timer_T1_IRQ(void);
void int_timer_T2_IRQ(void);
void int_timer_T3_IRQ(void);
void int_timer_T4_IRQ(void);
void timer_int_main(void);



   UINT8 cnt;
   UINT8 presc;

  _Bool t1 = 0;
  _Bool t2 = 0;
  _Bool t3 = 0;
  UINT16 t1Val = 0;
  UINT16 t3Val = 0;


/******************************************************************************
* @fn  timer_int_main
*
* @brief
*     Main function.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void timer_int_main(void){
#else
void main(void){
#endif

   UINT8 timer3Period;
   UINT16 timer1Period;


   initLcd();

   lcdUpdate((char*)"T1:  T2:  T3: ",(char*)"(LEFT to exit)");

   //Init I/O ports for LEDs and turn light off
   INIT_GLED();
   INIT_YLED();

   YLED = LED_OFF;
   SET_MAIN_CLOCK_SOURCE(CRYSTAL);


   // Enabling overflow interrupt from timer 1
   TIMER1_INIT();
   timer1Period = halSetTimer1Period(800);
   if(timer1Period != 0)
   {
      TIMER1_ENABLE_OVERFLOW_INT(TRUE);
      INT_ENABLE(INUM_T1, INT_ON);
      TIMER1_RUN(TRUE);
   }



   TIMER2_INIT();
   TIMER2_ENABLE_INTERRUPT();
   INT_ENABLE(INUM_T2, INT_ON);
   halSetTimer2Period(400000, &cnt, &presc);


   // Setting up Timer 3 and enabling overflow interrupt.
   TIMER34_INIT(3);
   timer3Period = halSetTimer34Period(3, 100);
   if(timer3Period != 0)
   {
      TIMER34_ENABLE_OVERFLOW_INT(3,INT_ON);
      INT_ENABLE(INUM_T3, INT_ON);
      TIMER3_RUN(TRUE);
   }



   INT_GLOBAL_ENABLE(TRUE);


   // Running until the application is stopped.
   while(!stopApplication());


   LED1 = LED3 = LED_OFF;
   INT_ENABLE(INUM_T1, INT_OFF);
   INT_ENABLE(INUM_T2, INT_OFF);
   INT_ENABLE(INUM_T3, INT_OFF);
   INT_ENABLE(INUM_T4, INT_OFF);
}


/******************************************************************************
* @fn  int_timer_T1_IRQ
*
* @brief
*     Interrupt handler for timer T1 interrupts.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
   void int_timer_T1_IRQ(void){
#else
   #pragma vector=T1_VECTOR
   __interrupt void T1_IRQ(void){
#endif
   EA = FALSE;
   if(T1CTL & 0x10){
      //T1 overflow
      t1Val++;
      if(t1Val == 1000)
      {
         t1Val = 0;
         if(t1)
         {
            t1 = 0;
            lcdUpdateChar(LINE1,3,(char)'*');
         }
         else
         {
            t1 = 1;
            lcdUpdateChar(LINE1,3,(char)' ');
         }
      }
   }
   EA = TRUE;
   T1CTL &= ~0x10;
}


/******************************************************************************
* @fn  int_timer_T2_IRQ
*
* @brief
*     Interrupt handler for timer T2 interrupts.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
   void int_timer_T2_IRQ(void){
#else
   #pragma vector=T2_VECTOR
   __interrupt void T2_IRQ(void){
#endif
   EA = FALSE;
   if(TIMER2_EXPIRED){
      TIMER2_CLEAR_EXPIRED();
      if(t2)
      {
         t2 = 0;
         lcdUpdateChar(LINE1,8,(char)'*');
      }
      else
      {
         t2 = 1;
         lcdUpdateChar(LINE1,8,(char)' ');
      }
   }

   TIMER2_SET_PRESCALER(presc);
   TIMER2_SET_COUNTER(cnt);
   EA = TRUE;
}



/******************************************************************************
* @fn  int_timer_T3_IRQ
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
   void int_timer_T3_IRQ(void){
#else
   #pragma vector=T3_VECTOR
   __interrupt void T3_IRQ(void){
#endif
   EA = FALSE;
   if(TIMIF & 0x01){
      //T3 overflow
      t3Val++;
      if(t3Val == 1000)
      {
         t3Val = 0;
         if(t3)
         {
            t3 = 0;
            lcdUpdateChar(LINE1,13,(char)'*');
         }
         else
         {
            t3 = 1;
            lcdUpdateChar(LINE1,13,(char)' ');
         }
      }
   }
   EA = TRUE;
   TIMIF &= ~0x01;
}


/******************************************************************************
* @fn  timer_int_init
*
* @brief
*     Initializes timer interrupt application example.
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
void timer_int_init(APPLICATION *a){
   a->menuText = (char*)"Timer";
   a->description = (char*)"Interrupts";
   a->main_func = timer_int_main;
   a->interrupts[INUM_T1] = int_timer_T1_IRQ;
   a->interrupts[INUM_T2] = int_timer_T2_IRQ;
   a->interrupts[INUM_T3] = int_timer_T3_IRQ;
}
#endif

