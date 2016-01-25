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

Filename:     app_ex_interrupt.c
Target:       cc2510
Author:       KJA
Revised:      20/6-2006
Revision:     1.0

Description:
    Sets up interrupt handlers for all interrupts in the application example.

******************************************************************************/

#include "app_ex_main.h"
#include "hal.h"

__interrupt void RFERR_IRQ(void);
__interrupt void ADC_IRQ(void);
__interrupt void URX0_IRQ(void);
__interrupt void URX1_IRQ(void);
__interrupt void ENC_IRQ(void);
__interrupt void ST_IRQ(void);
__interrupt void P2INT_IRQ(void);
__interrupt void UTX0_IRQ(void);
__interrupt void T1_IRQ(void);
__interrupt void T2_IRQ(void);
__interrupt void T3_IRQ(void);
__interrupt void P0INT_IRQ(void);
__interrupt void UTX1_IRQ(void);
__interrupt void P1INT_IRQ(void);
__interrupt void WDT_IRQ(void);

extern APPLICATION apps[NBR_OF_APPS];
extern UINT8 activeApp;

void dummyInterrupt(void){
}


/*
#pragma vector=RFERR_VECTOR
__interrupt void RFERR_IRQ(void){
   apps[activeApp].interrupts[INUM_RFERR]();
}
*/

#pragma vector=ADC_VECTOR
__interrupt void ADC_IRQ(void){
   apps[activeApp].interrupts[INUM_ADC]();
}
/*
#pragma vector=URX0_int
__interrupt void URX0_IRQ(void){
   apps[activeApp].interrupts[INUM_URX0]();
}
*/
#pragma vector=URX1_VECTOR
__interrupt void URX1_IRQ(void){
   apps[activeApp].interrupts[INUM_URX1]();
}
#pragma vector=ENC_VECTOR
__interrupt void ENC_IRQ(void){
   apps[activeApp].interrupts[INUM_ENC]();
}
#pragma vector=ST_VECTOR
__interrupt void ST_IRQ(void){
   apps[activeApp].interrupts[INUM_ST]();
}
#pragma vector=P2INT_VECTOR
__interrupt void P2INT_IRQ(void){
   apps[activeApp].interrupts[INUM_P2INT]();
}
#pragma vector=UTX0_VECTOR
__interrupt void UTX0_IRQ(void){
   apps[activeApp].interrupts[INUM_UTX0]();
}

/*
This interrupt routine is defined in CUL
#pragma vector=DMA_int
__interrupts void DMA_IRQ(void){
   apps[activeApp].interrupts[INUM_DMA]();
}
*/

#pragma vector=T1_VECTOR
__interrupt void T1_IRQ(void){
   apps[activeApp].interrupts[INUM_T1]();
}

#pragma vector=T2_VECTOR
__interrupt void T2_IRQ(void){
   apps[activeApp].interrupts[INUM_T2]();
}
#pragma vector=T3_VECTOR
__interrupt void T3_IRQ(void){
   apps[activeApp].interrupts[INUM_T3]();
}

/*
#pragma vector=T4_int
__interrupt void T4_IRQ(void){
   apps[activeApp].interrupts[INUM_T4]();
}
*/

#pragma vector=P0INT_VECTOR
__interrupt void P0INT_IRQ(void){
   apps[activeApp].interrupts[INUM_P0INT]();
}
#pragma vector=UTX1_VECTOR
__interrupt void UTX1_IRQ(void){
   apps[activeApp].interrupts[INUM_UTX1]();
}
/*
#pragma vector=P1INT_VECTOR
__interrupt void P1INT_IRQ(void){
   apps[activeApp].interrupts[INUM_P1INT]();
}
*/

/*
#pragma vector=RF_int
__interrupt void RF_IRQ(void){
   apps[activeApp].interrupts[INUM_RF]();
}
*/

#pragma vector=WDT_VECTOR
__interrupt void WDT_IRQ(void){
   apps[activeApp].interrupts[INUM_WDT]();
}
