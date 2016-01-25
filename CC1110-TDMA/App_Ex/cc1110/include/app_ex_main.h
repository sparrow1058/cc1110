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

Filename:     app_ex_main.h
Target:       cc2510
Author:       kja
Revised:      20/6-2006
Revision:     1.0

Description:

This file is the header file for the complete application with the menu system.

******************************************************************************/

// when defined each of the sub application is "mounted" in the system.

#ifndef APP_EX_MAIN_H
#define APP_EX_MAIN_H

#include "hal.h"


// ID for each of the sub application
enum app {RF_TEST, ADC_CONV, ADC_SERIES, TEMP_SENSOR, STOP_WATCH, UART, CLOCKMODES,
           RANDOM, AES, FLASH, DMA, POWER, TIMER_INT, EXTERNAL_INT};

#define NBR_OF_APPS 14


typedef struct APP_S{
   char *menuText;
   char *description;
   void (*main_func)(void);
   void (*interrupts[NBR_OF_INTERRUPTS])(void);
} APPLICATION;

// Prototypes
void initAppEx(void);
void updateMenu(void);
BOOL waitForInput(void);
void startApplication(void);
BOOL stopApplication(void);
void haltApplication(void);
void haltApplicationWithLED(void);
INT8U ScanKey(void);
void keyinit(void);

void showLogo(void);
UINT8 lcdMenu(char *text[], const UINT8 n);

void dummyInterrupt(void);
#endif

/*
Template for application init.
#ifdef COMPLETE_APPLICATION
void int_ext_init(APPLICATION *a){
   a->menuText = "Menu text";
   a->description = "Descriptin";
   a->main_func = app_main;
   a->interrupts[INUM_x] = x_IRQ;
}
#endif
*/
