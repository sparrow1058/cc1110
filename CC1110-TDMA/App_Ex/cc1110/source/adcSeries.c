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

Filename:     adcSeries.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0

Description:
    This example prints the pot-meter voltage versus time on the LCD.
    The sampling instant is set by timer 1 and the converted data is transferred
    using DMA. The data is then converted to be printed on the LCD.

******************************************************************************/
#include "app_ex.h"
#include <string.h>


// Prototypes
void initAdc(void);
void adc_main(void);
void updateVoltageLCD(INT8 potVoltage, INT8 adc_value);
void updateJoystickDirectionLCD(JOYSTICK_DIRECTION direction);
void updateCounter(INT8 delay);
INT8 scaleValue(INT8 adc_value);



typedef struct
{
   char symbol[8];
} SYMBOL;


#define NUMBER_OF_SAMPLES 40
#define NUMBER_OF_SYMBOLS 16


void shiftValues(INT8* values, BYTE length);
void graphElement(INT8 values[], SYMBOL *height, BYTE length);
void adc_series_main(void);


/******************************************************************************
* @fn  adc_series_main
*
* @brief
*      Main function for ADC series conversion application example
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void adc_series_main(void){
#else
void main(void){
#endif

   DMA_DESC dmaAdc;
   SYMBOL graphHeight[NUMBER_OF_SYMBOLS];
   BYTE i;
   BYTE k;
   char lcdData[2][16];
   INT8 volatile adcValues[NUMBER_OF_SAMPLES];

   DISABLE_ALL_INTERRUPTS();

   memset(graphHeight, 0, sizeof(graphHeight));
   memset((INT8*)adcValues, 0, sizeof(adcValues));

   SET_MAIN_CLOCK_SOURCE(CRYSTAL);
   CLKCON &= ~0x38;


   initLcd();

   INIT_GLED();
   INIT_YLED();

   lcdUpdate((char*)"Turn Potmeter",(char*)"");
   halWait(0xFF);
   halWait(0xFF);
   halWait(0xFF);

   // Setting up the DMA
   SET_WORD(dmaAdc.SRCADDRH, dmaAdc.SRCADDRL,   &X_ADCH);
   SET_WORD(dmaAdc.DESTADDRH, dmaAdc.DESTADDRL, &adcValues);
   SET_WORD(dmaAdc.LENH, dmaAdc.LENL, 1);
   dmaAdc.VLEN          = VLEN_USE_LEN;
   dmaAdc.PRIORITY      = PRI_HIGH;
   dmaAdc.M8            = M8_USE_8_BITS;
   dmaAdc.IRQMASK       = FALSE;
   dmaAdc.DESTINC       = DESTINC_0;
   dmaAdc.SRCINC        = SRCINC_0;
   dmaAdc.TRIG          = 28;
   dmaAdc.TMODE         = TMODE_SINGLE_REPEATED;
   dmaAdc.WORDSIZE      = WORDSIZE_BYTE;


   // Setting the descriptor pointer and arming the DMA. Using DMA channel 0
   DMA_SET_ADDR_DESC0(&dmaAdc);
   DMA_ABORT_CHANNEL(0);
   DMA_ARM_CHANNEL(0);


   // Setting up timer 1 to generate sampling commands to the ADC.
   TIMER1_INIT();
   halSetTimer1Period(40000);
   T1CCTL0 = 0x24;


   // Setting up the ADC to sample from channel 7 (pin P0.7 pot meter)
   ADC_ENABLE_CHANNEL(7);
   ADC_SEQUENCE_SETUP(ADC_REF_AVDD | ADC_14_BIT | ADC_AIN7);
   ADC_TRIGGER_FROM_TIMER1();


   strcpy(lcdData[0], (char*)"3.3V ");
   strcpy(lcdData[1], (char*)"0.0V_");

   lcdUpdate(lcdData[0], lcdData[1]);
   lcdUpdateSymbol(LINE1, 4, 0x46);


   // Starting timer 1
   TIMER1_RUN(TRUE);

   while(!stopApplication())//TRUE)
   {
      ADC_TRIGGER_FROM_TIMER1();

      halWait(50);

      // Checking if a DMA transfer has been performed.
      if(DMAIRQ & DMA_CHANNEL_0)
      {
         DMAIRQ &= ~DMA_CHANNEL_0;

         // Converting the new sample to "LCD-format"
         graphElement((INT8*)adcValues, graphHeight, NUMBER_OF_SYMBOLS/2);

         // Shifting the converted values
         shiftValues((INT8*)adcValues, NUMBER_OF_SAMPLES);

         //init new symbols
         for(i = 0; i < NUMBER_OF_SYMBOLS-1; i++)
         {
            initNewSymbol( (char*)&graphHeight[i], CHAR1_ADDRESS + (i*0x08));
         }

         // Updating the LCD
         for(i = 11, k = 1; i > 4; i--, k += 2)
         {
            lcdUpdateChar(LINE1, i, k);
            lcdUpdateChar(LINE2, i, k+1);
         }
      }

   }
   // Stopping the conversions
   ADC_STOP();
   DMA_ABORT_CHANNEL(0);
   TIMER1_RUN(FALSE); 
   ADC_SEQUENCE_SETUP(0x00);
   
   // Making sure the ADC is emptied
   adcValues[0] = ADCH;
   adcValues[1] = ADCL;
   adcValues[2] = ADCH;
   adcValues[3] = ADCL;     
}


/******************************************************************************
* @fn  shiftValues
*
* @brief
*      Right-shift of INT8 vectors
*
* Parameters:
*
* @param  INT8* values
*
*         BYTE length
*
* @return void
*
******************************************************************************/
void shiftValues(INT8* values, BYTE length)
{
   length--;

   while(length)
   {
      values[length] = values[length-1];
      length--;
   }
}


/******************************************************************************
* @fn  graphElement
*
* @brief
*      Processing new graph elements
*
* Parameters:
*
* @param  INT8 values[]
*         SYMBOL *height
*         BYTE length
*
* @return void
*
******************************************************************************/
void graphElement(INT8 values[], SYMBOL *height, BYTE length)
{
   INT8 i;
   BYTE j;
   BYTE k;
   INT8 temp = 0;

   for(k = 0; k < length; k++)
   {

      // resetting all entries
      for(i = 0; i < 8; i++)
      {
         height[2*k].symbol[i] = 0;
         height[2*k+1].symbol[i] = 0;
      }

      // processing values in blocks of 5
      for(j = 0; j < 5; j++)
      {

         if(values[j+5*k] > 0)
         {
            temp = (values[j+5*k] >> 3);
         }
         else
         {
            temp = 0;
         }

         i = 7;
         while((temp > 0) && (i >= 0))
         {
            height[2*k+1].symbol[i] |= (1 << j);
            i--;
            temp -= 1;
         }

         i = 7;
         while((temp > 0) && (i >= 0))
         {
            height[2*k].symbol[i] |= (1 << j);
            i--;
            temp -= 1;
         }

      }
   }
}


/******************************************************************************
* @fn  adc_series_init
*
* @brief
*      Initializes ADC application example.
*
* Parameters:
*
* @param  APPLICATION  *a
*         Main application
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void adc_series_init(APPLICATION *a){
   a->menuText = (char*)"ADC Series";
   a->description = (char*)"T1 and DMA";
   a->main_func = adc_series_main;
}
#endif
