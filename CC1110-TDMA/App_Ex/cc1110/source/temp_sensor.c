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

Filename:     temp_sensor.c
Target:       cc2510
Author:       kja
Revised:      20/6-2006
Revision:     1.0

Description:

This application show the chip temperature on the LCD display.

******************************************************************************/

#include "app_ex.h"
#include "lcd128_64.h"

#define MAX_TEMP 50
#define DEGREE_SYMBOL_ADDRESS 0x01

void initTempSensor(void);
INT8 getTemperature(void);
void temp_sensor_main(void);


/******************************************************************************
*
*
******************************************************************************/
void initTempSensor(void){
   DISABLE_ALL_INTERRUPTS();

   SET_MAIN_CLOCK_SOURCE(CRYSTAL);

    *((BYTE __xdata*) 0xDF26) = 0x80;

 //  initLcd();
}

/******************************************************************************
*
*
******************************************************************************/
#define ADC14_TO_CELSIUS(ADC_VALUE)    ( ((ADC_VALUE) >> 4) - 315)
INT8 getTemperature(void){
  UINT8   i;
  UINT16  accValue;
  UINT16  value;

  accValue = 0;
  for( i = 0; i < 4; i++ )
  {
    ADC_SINGLE_CONVERSION(ADC_REF_1_25_V | ADC_14_BIT | ADC_TEMP_SENS);
    ADC_SAMPLE_SINGLE();
    while(!ADC_SAMPLE_READY());

    value =  ADCL >> 2;
    value |= (((UINT16)ADCH) << 6);

    accValue += value;
  }
  value = accValue >> 2; // devide by 4

  return ADC14_TO_CELSIUS(value);
}


/******************************************************************************
*
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void temp_sensor_main(void){
#else
void main(void){
#endif
   char temperature[10];
 // char degreeSymbol[] = {0x07, 0x05, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00};
   INT16 avgTemp;

   UINT8 i = 0;

   initTempSensor();
   ClearScreen();
   Print(0,10," TEMPERATURE ",1);
   Rectangle(2 , 2 , 127 , 7);
   if(LanguageSel == 1)
   {
       Print(4,8,"Chip temp. :",1);
   }
   else
   {
       Print(4,8,"оƬ�¶�Ϊ:  ",1);
   }

   while(!stopApplication()){
     avgTemp = 0;
     for(i = 0 ; i < 64 ; i++)
     {
        avgTemp += getTemperature();
     }
     avgTemp /= 64;
     sprintf(temperature, (char *)"%dC", (INT8)avgTemp);
     Print(4,98,(INT8U *)temperature,1);
      halWait(250);
      halWait(250);
      halWait(250);
      halWait(250);
   }

    *((BYTE __xdata*) 0xDF26) = 0x00;


}

/******************************************************************************
*
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void temp_sensor_init(APPLICATION *a){
   a->menuText = (char *)"Temp Sensor";
   a->description = (char *)"";
   a->main_func = temp_sensor_main;
}
#endif
