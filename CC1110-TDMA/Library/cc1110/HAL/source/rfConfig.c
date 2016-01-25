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

Filename:     rfConfig.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include "hal.h"
//#define FSTEST _XREGDF20
//#define PTEST _XREGDF21
//#define AGCTEST _XREGDF22
//#define TEST2 _XREGDF23
//#define TEST1 _XREGDF24
//#define TEST0 _XREGDF25
//static INT16 perRssiOffset;             // RSSI offset for receiver, depends on
#define FREQUENCY_4_CC1110       433500     // radioConfigure() in
//-----------------------------------------------------------------------------
// See hal.h for a description of this function.
//-----------------------------------------------------------------------------
BOOL halRfConfig(UINT32 frequency)
{
   BOOL status;

   //Turning on crystal oscillator
   SET_MAIN_CLOCK_SOURCE(CRYSTAL);

   // Setting the frequency
  // halRfSetRadioFrequency(frequency);


   if (frequency == FREQUENCY_4_CC1110)
   {
           // 433 MHz
            PA_TABLE0 = 0xC0;
            FREQ2 = 0x10;               // Actually 433.500 MHz to fit band
            FREQ1 = 0xAC;
            FREQ0 = 0x4E;

            // Settings from SmartRFStudio for CC1110, VERSION == 0x03
            // 250 kBaud, GFSK modulation, 540 kHz RX filter bandwidth.

            // Freq. dependent value: 433 MHz band vs 868/915 MHz band:
            FSCTRL1 =  (frequency == FREQUENCY_4_CC1110) ? 0x0C : 0x12;
            FSCTRL1  = 0x12;   // Frequency synthesizer control.
            FSCTRL0  = 0x00;   // Frequency synthesizer control.
            MDMCFG4  = 0x2D;   // Modem configuration.
            MDMCFG3  = 0x3B;   // Modem configuration.
            MDMCFG2  = 0x13;   // Modem configuration.
            MDMCFG1  = 0x22;   // Modem configuration.
            MDMCFG0  = 0xF8;   // Modem configuration.
            DEVIATN  = 0x62;   // Modem deviation setting (when FSK modulation is enabled).
            FREND1   = 0xB6;   // Front end RX configuration.
            FREND0   = 0x10;   // Front end RX configuration.
            MCSM0    = 0x18;   // Main Radio Control State Machine configuration.
            FOCCFG   = 0x1D;   // Frequency Offset Compensation Configuration.
            BSCFG    = 0x1C;   // Bit synchronization Configuration.
            AGCCTRL2 = 0xC7;   // AGC control.
            AGCCTRL1 = 0x00;   // AGC control.
            AGCCTRL0 = 0xB0;   // AGC control.
            FSCAL3   = 0xEA;   // Frequency synthesizer calibration.
            // Freq. dependent value: 433 MHz band vs 868/915 MHz band:
            FSCAL2 =  (frequency == FREQUENCY_4_CC1110) ? 0x0A : 0x2A;
            FSCAL0   = 0x1F;   // Frequency synthesizer calibration.
            TEST2    = 0x88;   // Various test settings.
            TEST1    = 0x31;   // Various test settings.
            TEST0    = 0x09;   // Various test settings.

            // Determine proper RSSI offset for receiver (freq and rate dependent)
            // and configure the radio frequency to use
     // 433 MHz

 //               perRssiOffset = 73;

#if 0
// 250kbps FSK setup (for other data rates or modulation formats, please see SmartRF Studio).
      // Dynamic packet length and append status
      PKTLEN = 0xFF;
      PKTCTRL0 = 0x05;
      PKTCTRL1 = 0x04;

      // IF frequency
      FSCTRL1 = 0x0A;
      FSCTRL0 = 0x00;

      // filter BW, data rate,
      MDMCFG4 = 0x2D;
      MDMCFG3 = 0x3B;

      // Modulation format, detection level
      MDMCFG2 = 0x73;
      MDMCFG1 = 0x22;
      MDMCFG0 = 0xF8;

      // Deviation setting
      DEVIATN = 0x00;

      // Calibration synth
      MCSM2 = 0x07;
      MCSM1 = 0x30;
      MCSM0 = 0x10;

      // Frequency offset compensation configuration
      FOCCFG = 0x1D;

      // Bit synchronization
      BSCFG = 0x1C;

      // AGC settings
      AGCCTRL2	= 0xC7;
      AGCCTRL1	= 0x00;
      AGCCTRL0	= 0xB2;

      // Front end settings (from SmartRf04)
      FREND1 = 0xB6;
      FREND0 = 0x10;

      // Synth calibration
      FSCAL3 = 0xEA;
      FSCAL2 = 0x0A;
      FSCAL1 = 0x00;
      FSCAL0 = 0x11;

      // From Smart RF Studio
      FOCCFG = 0x1D;
      BSCFG = 0x1C;
      FSTEST = 0x59;
      PTEST = 0x7F;
      AGCTEST = 0x3F;
      TEST2 = 0x88;
      TEST1 = 0x31;
      TEST0 = 0x0B;


      // Output power
      PA_TABLE0 = 0xFF;

#endif
      // Calibrating synth.
      SIDLE();
      SCAL();
      while(MARCSTATE != 0x01);

      INT_SETFLAG(INUM_RFTXRX,INT_CLR);


      status = TRUE;
   }
   else {
      status = FALSE;
   }

   return status;
}
