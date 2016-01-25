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

Filename:     adc.c
Target:       cc2510
Author:       KJA
Revised:      20/6-2006
Revision:     1.0

Description:
    This application shows how to use the AD-converter (single sample mode).

    When the potmeter at the Evaluation Board is turned, the output voltage is
    sampled, converted and written to LCD. The blinking speed of the green and
    yellow LEDs is adjusted according to the voltage.
    The direction of the joystick is shown on the LCD.

******************************************************************************/
#include "app_ex.h"

// Prototypes
void initAdc(void);
void adc_main(void);
void updateVoltageLCD(INT8 potVoltage, INT8 adc_value);
void updateJoystickDirectionLCD(JOYSTICK_DIRECTION direction);
void updateCounter(INT8 delay);
INT8 scaleValue(INT8 adc_value);


/******************************************************************************
* @fn  initAdc
*
* @brief
*      Initializes components for use with the ADC application example (e.g.
*      LEDs, PotMeter, Joystick).
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void initAdc(void)
{
   initLcd();

   SET_MAIN_CLOCK_SOURCE(CRYSTAL);

   //init LEDs
   INIT_GLED();
   INIT_YLED();

   INIT_POT();
   INIT_JOYSTICK();
}


/******************************************************************************
* @fn  adc_main
*
* @brief
*      Main function of the ADC application example.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void adc_main(void){
#else
void main(void){
#endif
   JOYSTICK_DIRECTION joystickDirection = CENTRED;
   INT8 adc_value;
   UINT8 potVoltage = 0;
   BOOL updateLCD = TRUE;

   initAdc();

   while( !stopApplication() ){
     
      adc_value = getPotValue();

      if (potVoltage != scaleValue(adc_value)){
         //potVoltage changed since last sample
         potVoltage = scaleValue(adc_value);
         updateLCD = TRUE;
      }

      if (joystickDirection != getJoystickDirection()){
         //joystickDirection change since last sample
         joystickDirection = getJoystickDirection();
         updateLCD = TRUE;
      }

      if (updateLCD){
         updateVoltageLCD(potVoltage, adc_value);
         updateJoystickDirectionLCD(joystickDirection);
         updateLCD = FALSE;
      }
      updateCounter(adc_value);
   }

   LED1 = LED3 = LED_OFF;
   return;
}


/******************************************************************************
* @fn  updateCounter
*
* @brief
*      Function for updating counting speed for binary counter
*
* Parameters:
*
* @param  INT8  delay
*         New counting delay
*
* @return void
*
******************************************************************************/
void updateCounter(INT8 delay)
{
   static UINT8 counter = 0;
   UINT16 i = 0;

   i = ((delay > 0) ? 0x7F - delay : 0x7F);
   halWait( i );

   counter++;

   SET_LED_MASK( (BYTE)counter );
}


/******************************************************************************
* @fn  scaleValue
*
* @brief
*      Function for scaling ADC value according to current VDD
*
* Parameters:
*
* @param  INT8  adc_value
*         Sampled ADC value
*
* @return INT8
*         Scaled value (output voltage)
*
******************************************************************************/
#define VDD 33
// return 10 times sampled voltage
INT8 scaleValue(INT8 adc_value)
{
   //0x0000 = 0V and 0x0F = 3.3V
   float v;

   adc_value = (adc_value > 0 ? adc_value : 0);
   v = ((float)adc_value / (float)0x7F);
   v *= VDD;
   return (INT8)v;
}


/******************************************************************************
* @fn  updateVoltageLCD
*
* @brief
*      Function for printing PotVoltage and sampled ADC value to LCD.
*
* Parameters:
*
* @param  INT8  potVoltage
*         Potmeter voltage
*
*         INT8 adc_value
*         Sampled ADC value
*
* @return void
*
******************************************************************************/
void updateVoltageLCD(INT8 potVoltage, INT8 adc_value)
{
   char s[16];
   // potVoltage is 10 times correct voltage
   // print dc value on LCD
   sprintf(s, (char*)"%d.%d Volt (%d)",  ((INT16)(potVoltage / 10)), ((INT16)(potVoltage % 10)), adc_value);
   lcdUpdate((char*)"Voltage is:", s);
}


/******************************************************************************
* @fn  updateJoystickDirectionLCD
*
* @brief
*      Function for printing new Joystick direction to LCD.
*
* Parameters:
*
* @param  JOYSTICK_DIRECTION  direction
*         Current joystick direction
*
* @return void
*
******************************************************************************/
void updateJoystickDirectionLCD(JOYSTICK_DIRECTION direction)
{
   char d;
   switch (direction){
   case DOWN:
      d = ARROW_DOWN;
      break;
   case UP:
      d = ARROW_UP;
      break;
   case LEFT:
      d = ARROW_LEFT;
      break;
   case RIGHT:
      d = ARROW_RIGHT;
      break;
   case CENTRED:
      d = '*';
      break;
   }
   lcdUpdateChar(LINE1, 15, d);
}


/******************************************************************************
* @fn  adc_init
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
void adc_init(APPLICATION *a)
{
   a->menuText = (char*)"ADC";
   a->description = (char*)"Use joys./ pot";
   a->main_func = adc_main;
}
#endif
