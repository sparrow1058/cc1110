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

Filename:     adcSampleSingle.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
*******************************************************************************/
#include "hal.h"
/*
//-----------------------------------------------------------------------------
// See hal.h for a description of this function.
//-----------------------------------------------------------------------------
INT16 halAdcSampleSingle(BYTE reference, BYTE resolution, UINT8 input) {
    BYTE volatile temp;
    INT16 value;

    //reading out any old conversion value
    temp = ADCH;
    temp = ADCL;


    ADC_ENABLE_CHANNEL(input);
    ADC_STOP();

    ADC_SINGLE_CONVERSION(reference | resolution | input);

    while (!ADC_SAMPLE_READY());

    ADC_DISABLE_CHANNEL(input);

    value = (((INT16)ADCH) << 8);
    value |= ADCL;

    ADC_STOP();

    resolution >>= 3;
    return value >> (8 - resolution);
}*/

INT16 halAdcSampleSingle(BYTE reference, BYTE resolution, UINT8 input) {
    BYTE volatile temp;
    INT16 value;

    //reading out any old conversion value
    temp = ADCH;
    temp = ADCL;


    ADC_ENABLE_CHANNEL(input);

    ADC_STOP();

    halWait(5);

    ADC_SINGLE_CONVERSION(reference | resolution | input);

    while (!ADC_SAMPLE_READY());

    ADC_DISABLE_CHANNEL(input);

    value = ADCL;
    value |= (((INT16)ADCH) << 8);

    if(value & 0x8000)
    {
		value = ~value + 1;
    }

    resolution >>= 3;
    value >>= (7 - resolution);
	asm("nop");
	asm("nop");
    return value;
}

