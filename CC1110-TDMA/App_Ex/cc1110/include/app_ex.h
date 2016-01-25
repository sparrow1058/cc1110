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

Filename:     app_ex.h
Target:       cc2510
Author:       KJA
Revised:      20/6-2006
Revision:     1.0

Description:
    This file includes prototypes for the init methods for each
    application present in the main menu.

******************************************************************************/

#ifndef APP_EX_H
#define APP_EX_H

#include "app_ex_main.h"
#include "app_ex_util.h"
#include "lcd.h"


/* ADC */
void adc_init(APPLICATION *a);

/* ADC series */
void adc_series_init(APPLICATION *a);

/*Stop watch*/
void stop_watch_init(APPLICATION *a);

/*Temp Sensor*/
void temp_sensor_init(APPLICATION *a);

/*UART*/
void uart_init(APPLICATION *a);

/*Clockmodes*/
void clockmodes_init(APPLICATION *a);

/*RF test*/
void rf_test_init(APPLICATION *a);

// RF232
void rf232_init(APPLICATION *a);

/*Random*/
void random_init(APPLICATION *a);

/*AES*/
void aes_init(APPLICATION *a);

/*FLASH*/
void flash_init(APPLICATION *a);

/*DMA*/
void dma_init(APPLICATION *a);

/*Power modes*/
void power_init(APPLICATION *a);

/*Timer interrupt*/
void timer_int_init(APPLICATION *a);

/*External interrupt*/
void external_int_init(APPLICATION *a);

#endif
