#include "rf_cc1110.h"

#define IRQ_DONE  0x10

/*wait for some time*/
void halWait(INT8U wait);


/*
===============================================================================
* function : rf_cc1110_freq( )
  set the frequency of the rf radio
* input    : frequency,  the freq of the rf radio, example, 433000 = 433.000M
===============================================================================
*/
void rf_cc1110_freq(INT32U freq_KHZ)
{
   freq_KHZ = (freq_KHZ << 10);
   freq_KHZ /= 1000;
   freq_KHZ = (freq_KHZ << 6);
   freq_KHZ /= 26;
   FREQ0 = (INT8U) freq_KHZ;
   freq_KHZ >>= 8;
   FREQ1 = (INT8U) freq_KHZ;
   freq_KHZ >>= 8;
   FREQ2 = (INT8U) freq_KHZ;
}
/*
===============================================================================
* function : rf_cc1110_init( )
  Initialize the device with a frequency ( KHZ )
* input    : frequency,  the freq of the rf radio, example, 433000 = 433.000M
===============================================================================
*/
void rf_cc1110_init(INT32U freq_KHZ)
{
    /* RF settings SoC: CC1110, from smartRFstudio */
    PKTCTRL0  = 0x05; // packet automation control
    CHANNR    = 0x03; // channel number
    FSCTRL1   = 0x06; // frequency synthesizer control
    MDMCFG4   = 0xF5; // modem configuration
    MDMCFG3   = 0x83; // modem configuration
    MDMCFG2   = 0x03; // modem configuration
    DEVIATN   = 0x15; // modem deviation setting
    MCSM0     = 0x18; // main radio control state machine configuration
    FOCCFG    = 0x17; // frequency offset compensation configuration
    FSCAL3    = 0xE9; // frequency synthesizer calibration
    FSCAL2    = 0x2A; // frequency synthesizer calibration
    FSCAL1    = 0x00; // frequency synthesizer calibration
    FSCAL0    = 0x1F; // frequency synthesizer calibration
    TEST1     = 0x31; // various test settings
    TEST0     = 0x09; // various test settings
    PA_TABLE0 = 0xC0; // pa power setting 0
    //IOCFG0    = 0x06; // radio test signal configuration (p1_5)

    rf_cc1110_freq( freq_KHZ );
    PA_TABLE0 = 0xC0;               //Set power: +10dBm
    SIDLE();
    SCAL();
    while(MARCSTATE != 0x01);
    RFTXRXIF = 0;
}



INT8U volatile length;

INT8U volatile isLengthByte;
INT8U index;
INT8U* pPayload;
INT8U* pRssiVal;
INT8U* pLqiVal;
INT8U lengthReceived;
/*
===============================================================================
* function : halWait( )
  wait for some time
* input    : wait for somw time
===============================================================================
*/
void halWait(INT8U wait)
{
   INT32U largeWait;

   if(wait == 0) 
   {return;}

   largeWait = ((INT16U) (wait << 7));
   largeWait += 59*wait;

   largeWait = (largeWait >> (CLKCON & 0x07));//CLKSPD);
   while(largeWait--);
}
/*
===============================================================================
* function : rf_txrx_IRQ( )
  rxtx interrupt ISR
* input    : None
* output   : None
===============================================================================
*/
#pragma vector=RFTXRX_VECTOR
__interrupt void rf_txrx_IRQ(void)
{

    //The interrupt flag is cleared automatically
    if (isLengthByte)
    {
        length = RFD;
        isLengthByte = FALSE;
        lengthReceived = TRUE;
    }
    else if (index < length)
    {
        pPayload[index++] = RFD;
    }
    else if (index == length)
    {
       index++;
       *pRssiVal = RFD;
    }
    else
    {
       *pLqiVal = RFD;
       isLengthByte = TRUE;
    }

   return;
}
/*
===============================================================================
* function : rf_rec_packet( )
  receive a packet and returns the count
* input    : pData, a buffer to store received packet
             pRssi, pointer to RSSI byte
			 pLqi,  pointer to LQI buye
			 timeout, time out hanlder for receive
* output   : how many bytes received
===============================================================================
*/
INT8U rf_rec_packet(INT8U * pData, INT8U* pRssi, INT8U* pLqi, INT8U timeOut)
{
    index = 0;
    length = 0;

    RFTXRXIF = 0;
    IEN0 |= 0x01;			//ENABLE rxtx interrupt    
    IEN0 |= 0x80;			//enable global interrupt

    RFIF = RFIF & ~IRQ_DONE;

    pPayload = pData;
    pRssiVal = pRssi;
    pLqiVal = pLqi;
    *pRssiVal = 0x00;
    *pLqiVal = 0x00;
    lengthReceived = FALSE;

    isLengthByte = TRUE;

    SRX();

    while(!(RFIF & IRQ_DONE))
    {
      if(timeOut)
      {
         halWait(1);
         timeOut--;
      }
      else
      {
         if(lengthReceived == TRUE)
         {
            timeOut = 255;
            lengthReceived = FALSE;
         }
         else
         {
            SIDLE();
            IEN0 &= ~0x01;		//disable rxtx interrupt
            return 0;
         }
      }
   }

   SIDLE();
   RFIF &= ~IRQ_DONE;
   IEN0 &= ~0x01;

   if(*pLqi & 0x80)
   {
      return index-1;
   }
   else
   {
      return 0;
   }
}

/*
===============================================================================
* function : rf_send_packet( )
  send a packet and returns the count
* input    : pData, a buffer to stores a packet to be sent
             length, how many bytes should be sent
* output   : how many bytes has been sent
===============================================================================
*/
INT8U rf_send_packet(INT8U  * pData, INT8U length)
{
    INT8U i = 0x00;

    if(length == 0)				{ return 0; }

    // Turning on radio and writing data including length;
    RFTXRXIF = 0;
    IEN0 &= ~0x01;
    RFIF &= ~IRQ_DONE;

    STX();
    while(RFTXRXIF == 0);
    RFTXRXIF = 0;
    RFD = length;

    // Inserting data
    for(i = 0 ; i < length ; i++){
        while(RFTXRXIF == 0);
        RFTXRXIF = 0;
        RFD = pData[i];
    }

   // Waitting for transmission to finish
   while(!(RFIF & IRQ_DONE));
   RFIF &= ~IRQ_DONE;
   SIDLE();
   return i;
}
/*
===============================================================================
------------------------------------END OF FILE--------------------------------
===============================================================================
*/
