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

Filename:     RF04Dev.c
Target:       cc2510
Author:       KJA
Revised:      20/6-2006
Revision:     1.0

Description:
Implementation of commonly used functions for the RF04EB and CC2430DB.

* For use with CC2430DB add "CC2430DB" to project options.
  Project -> Options... -> C compiler -> Preprocessor -> Defined symbols

* For use with RF04EB do _not_ add "CC2430DB to project options.

******************************************************************************/

#include "ioCC2510.h"

#ifdef CC2430DB
   #include "CC2430DB.h"
#else
   #include "RF04EB.h"
#endif


/******************************************************************************
* @fn  getJoystickDirection
*
* @brief
*      This function utilizes the 8-bit ADC to give an indication of the
*      current position of the joystick. Current support is for 90 degrees
*      positioning only.
*
* Parameters:
*
* @param  void
*
* @return JOYSTICK_DIRECTION
*          DOWN: Joystick direction is down (270 degrees)
*          LEFT: Joystick direction is left (180 degrees)
*	   RIGHT: Joystick direction is right (0 degrees)
*	   UP: Joystick direction is up (90 degrees)
*	   CENTRED: Joystick direction is centred (passive position)
*
******************************************************************************/
JOYSTICK_DIRECTION getJoystickDirection( void ) {
    INT8 adcValue, i;
    JOYSTICK_DIRECTION direction[2];

    for(i = 0; i < 2; i++){
       adcValue = halAdcSampleSingle(ADC_REF_AVDD, ADC_8_BIT, ADC_INPUT_JOYSTICK);

       if (adcValue < 0x10) {
          direction[i] = DOWN;  // Measured [0xFC, 0x03]
       } else if (adcValue < 0x39) {
          direction[i] = LEFT;  // Measured [0x2E, 0x37]
       } else if (adcValue < 0x57) {
          direction[i] = RIGHT; // Measured [0x4C, 0x54]
       } else if (adcValue < 0x67) {
          direction[i] = UP;    // Measured [0x59, 0x64]
       } else {
          direction[i] = CENTRED; // Measured [0x69, 73]
       }
    }

    if(direction[0] == direction[1]){
       return direction[0];
    }
    else{
       return CENTRED;
    }
}


/******************************************************************************
* @fn  getPotValue
*
* @brief
*      This function utilizes the 8-bit ADC to obtain a digital value for the
*      potentiometer resistance.
*
* Parameters:
*
* @param  void
*
* @return UINT8
*         0xFF
*         0x00
*
******************************************************************************/
UINT8 getPotValue( void ){
    INT8 adcValue = halAdcSampleSingle(ADC_REF_AVDD, ADC_8_BIT, ADC_INPUT_POT);
    return (adcValue > 0) ? adcValue : 0;
}


/******************************************************************************
* @fn  buttonPushed
*
* @brief
*      This function detects if the button is being pushed. The function
*      implements software debounce. Return true only if previuosly called
*      with button not pushed. Return true only once each time the button
*      is pressed.
*
* Parameters:
*
* @param  void
*
* @return BOOL
*          TRUE: Button is being pushed
*          FALSE: Button is not being pushed
*
******************************************************************************/
#define   BUTTON_ACTIVE_TIMEOUT   10
BOOL buttonPushed( void ) {
   UINT8 i;
   BOOL value;
   static BOOL prevValue;

   if (value = BUTTON_PRESSED()){
      for(i = 0;i < BUTTON_ACTIVE_TIMEOUT; i++){
         if(!BUTTON_PRESSED()){
            value = FALSE;
            break;
         }
      }
   }

   if(value){
      if (!prevValue){
         value = prevValue = TRUE;
      }
      else{
         value = FALSE;
      }
   }
   else{
      prevValue = FALSE;
   }
   return value;
}


/******************************************************************************
* @fn  joystickPushed
*
* @brief
*      This function detects if the joystick is being pushed. The function
*      implements software debounce. Return true only if previuosly called
*      with joystick not pushed. Return true only once each time the joystick
*      is pressed.
*
* Parameters:
*
* @param  void
*
* @return BOOL
*          TRUE: Button is being pushed
*          FALSE: Button is not being pushed
*
******************************************************************************/
BOOL joystickPushed( void ) {
   UINT8 i;
   BOOL value;
   static BOOL prevValue;

   if (value = JOYSTICK_PRESSED()){
      for(i = 0;i < BUTTON_ACTIVE_TIMEOUT; i++){
         if(!JOYSTICK_PRESSED()){
            value = FALSE;
            break;
         }
      }
   }

   if(value){
      if (!prevValue){
         value = prevValue = TRUE;
      }
      else{
         value = FALSE;
      }
   }
   else{
      prevValue = FALSE;
   }

   return value;
}

#ifdef CC2430DB
/******************************************************************************
* @fn  getLdrValue
*
* @brief
*      Returns a value indicating the illumination of the light dependent
*      resistor (LDR). Values range from 0xFF (255) for saturated sensor
*      (resistor value around 5 kOhm), to 0 for dark sensor (resistor value
*      around 20MOhm).
*
* Parameters:
*
* @param  void
*
* @return UINT8
*         0xFF (255)  maximum illumination (saturated sensor)
*         0x00        dark sensor
*
******************************************************************************/
UINT8 getLdrValue( void ){
   INT8 adcValue = halAdcSampleSingle(ADC_REF_AVDD, ADC_8_BIT, ADC_INPUT_LDR);

   adcValue = (adcValue > 0) ? adcValue : 0;
   // max 8 bit value from ADC is 0x7F (127)
   adcValue *= 2;

   return 255 - adcValue;
}

/******************************************************************************
* @fn  getXacceleration
*
* @brief
*      This function returns the value of the current acceleration of the of
*      accelerometer in the x-axis.
*
* Parameters:
*
* @param  void
*
* @return UINT8
*         0xFF (255)	+18g (theoretical value)
*	  0x7F (127)      0g no acceleration
*         0x00          -18g (theoretical value)
*
* Wait 20 ms after VCC is turned on before calling this function.
******************************************************************************/
UINT8 getXacceleration( void ){
   INT8 adcValue = halAdcSampleSingle(ADC_REF_AVDD, ADC_8_BIT, ADC_INPUT_ACC_X);
   return (adcValue > 0) ? adcValue : 0;
}

/******************************************************************************
* @fn  getYacceleration
*
* @brief
*      This function returns the value of the current acceleration of the of
*      accelerometer in the y-axis.
*
* Parameters:
*
* @param  void
*
* @return UINT8
*         0xFF (255)	+18g (theoretical value)
*	  0x7F (127)      0g no acceleration
*         0x00          -18g (theoretical value)
*
* Wait 20 ms after VCC is turned on before calling this function.
******************************************************************************/
UINT8 getYacceleration( void ){
   INT8 adcValue = halAdcSampleSingle(ADC_REF_AVDD, ADC_8_BIT, ADC_INPUT_ACC_Y);
   return (adcValue > 0) ? adcValue : 0;
}
#endif
