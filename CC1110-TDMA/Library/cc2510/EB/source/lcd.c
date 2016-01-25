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

Filename:     lcd.c
Target:       cc2510
Author:       KJA
Revised:      20/6-2006
Revision:     1.0

Description:
Function implementations for common LCD functions for use with the SmartRF04EB.

******************************************************************************/

#include "lcd.h"
#include "RF04EB.h"
#include "hal.h"

// SM-Bus address of the LCD controller
#define LCD_ADDR        0x76

// RS selects the register to be accessed for read and write
// RS = 0 => selects instruction register for write and busy flag
// RS = 1 => selects the data register for both read and write
#define RS_0            0x00
#define RS_1            0x40

#define CHAR1_ADDRESS   0x08

// protos
BYTE lcdConvertChar(BYTE c);
void smbSend( BYTE *buffer, const UINT8 n );
BOOL smbSendByte( BYTE b );
void smbReceive( BYTE address, int length, BYTE *buffer );
BYTE smbReceiveByte( BOOL lastByte );
void smbStart( void );
void smbStop( void );
void smbWrite( BOOL value );
void smbClock( BOOL value );
void waitLCD( void );

#define DATA_HIGH()    do{IO_DIR_PORT_PIN(1, 2, IO_IN); } while(0)
#define DATA_LOW()     do{IO_DIR_PORT_PIN(1, 2, IO_OUT); LCD_SDA = 0; }while(0)



/******************************************************************************
* See lcd.h for a description of this function.
******************************************************************************/
void initNewSymbol(char* symbol, BYTE address){
   BYTE sendBuffer[10];

   //send control data
   sendBuffer[0] = LCD_ADDR;
   sendBuffer[1] = RS_0;

   if (address < 0x40)
      sendBuffer[2] = 0x80;
   else
      sendBuffer[2] = 0xC0;

   sendBuffer[3] = 0x40 | (address & 0x3F);
   smbSend(sendBuffer, 4);

   //send data
   sendBuffer[0] = LCD_ADDR;
   sendBuffer[1] = RS_1;

   sendBuffer[2] = symbol[0];
   sendBuffer[3] = symbol[1];
   sendBuffer[4] = symbol[2];
   sendBuffer[5] = symbol[3];
   sendBuffer[6] = symbol[4];
   sendBuffer[7] = symbol[5];
   sendBuffer[8] = symbol[6];
   sendBuffer[9] = symbol[7];

   smbSend(sendBuffer, 10);
}


/******************************************************************************
* See lcd.h for a description of this function.
******************************************************************************/
void initLcd(void){
   BYTE sendBuffer[8];

   // Setting the ports as inputs.
   IO_DIR_PORT_PIN(1, 2, IO_IN);
   IO_DIR_PORT_PIN(2, 0, IO_IN);

   // Setting P2_3 and P2_4 for general IO operation.
   IO_FUNC_PORT_PIN(1, 2, IO_FUNC_GIO);
   IO_FUNC_PORT_PIN(2, 0, IO_FUNC_GIO);

   // Setting ports for pull-up.
   IO_IMODE_PORT_PIN(1, 2, IO_IMODE_PUD);
   IO_IMODE_PORT_PIN(2, 0, IO_IMODE_PUD);
   IO_PUD_PORT(1, IO_PULLUP);
   IO_PUD_PORT(2, IO_PULLUP);

   // Setting up the lcd initialisation data.
   sendBuffer[0] = LCD_ADDR;
   sendBuffer[1] = RS_0;      // Instruction Register
   sendBuffer[2] = 0x0C;      // Display control         D =  1:      Display On
   //                                                    C =  0:      Cursor Off
   //                                                    B =  0:      Cursor character blink off
   sendBuffer[3] = 0x21;      // Function set            H =  1:      Use extended instruction set
   sendBuffer[4] = 0xA0;      // Set DDRAM address       ADD = 0x20
   sendBuffer[5] = 0x07;      // Display configuration   P =  1:      Column data right to left
   //                                                    Q =  1:      Row data, bottom to top
   sendBuffer[6] = 0x34;      // Function set            DL=  0:      4 bits
   //                                                    M =  1:      2-line by 16 display
   //                                                    SL=  0:      MUX1:18
   //                                                    H =  0:      Use basic instruction set
   sendBuffer[7] = 0x01;      // Clearing display
   smbSend(sendBuffer, 8);

   lcdUpdate((char*)"", (char*)"");  //clear display
}



/******************************************************************************
* See lcd.h for a description of this function.
******************************************************************************/
void lcdUpdate(char *pLine1, char *pLine2) {
   lcdUpdateLine(LINE1, pLine1);
   lcdUpdateLine(LINE2, pLine2);
   return;
}


/******************************************************************************
* See lcd.h for a description of this function.
******************************************************************************/
void waitVisible(BOOL visibility)
{
   UINT16 i;

   if(visibility){
      i = 100;
   }
   else{
      i = 6;
   }

   while(i--){
      halWait(10);
   }
}


/******************************************************************************
* See lcd.h for a description of this function.
******************************************************************************/
void lcdUpdateLine(UINT8 line, char *pLine) {
   BYTE sendBuffer[50];
   UINT8 i, j;
   char c;

   i = 0;
   sendBuffer[i++] = LCD_ADDR;
   sendBuffer[i++] = RS_0;
   sendBuffer[i++] = ((line == LINE1) ?  LINE1_ADDR : LINE2_ADDR);
   smbSend(sendBuffer, i);

   i = j = 0;
   sendBuffer[i++] = LCD_ADDR;
   sendBuffer[i++] = RS_1;
   while( ( (c = pLine[j]) != '\0' ) && j < LINE_SIZE ){
      sendBuffer[i++] = lcdConvertChar(c);
      j++;
   }
   for ( ;j < LINE_SIZE; j++){
      sendBuffer[i++] = lcdConvertChar(' ');
   }
   smbSend(sendBuffer, i);
}


/******************************************************************************
* See lcd.h for a description of this function.
******************************************************************************/
void lcdUpdateChar(UINT8 line, UINT8 position, char c){
   lcdUpdateSymbol(line, position, lcdConvertChar(c));
}


/******************************************************************************
* See lcd.h for a description of this function.
******************************************************************************/
void lcdUpdateSymbol(UINT8 line, UINT8 position, char c){
   BYTE sendBuffer[3];
   UINT8 i;

   if(position > LINE_SIZE){
      position = LINE_SIZE - 1;
   }

   i = 0;
   sendBuffer[i++] = LCD_ADDR;
   sendBuffer[i++] = RS_0;
   sendBuffer[i++] = ((line == LINE1) ?  LINE1_ADDR : LINE2_ADDR) + position;
   smbSend(sendBuffer, i);

   i = 0;
   sendBuffer[i++] = LCD_ADDR;
   sendBuffer[i++] = RS_1;
   sendBuffer[i++] = c;
   smbSend(sendBuffer, i);
}



/******************************************************************************
* See lcd.h for a description of this function.
******************************************************************************/
void scrollText(char *string, UINT8 length){
   UINT8 i = 0;

   while(getJoystickDirection() != CENTRED);

   do{
      lcdUpdateLine(LINE2, &string[i]);
      i++;

      if(getJoystickDirection() != CENTRED)
      {
         lcdUpdateLine(LINE2, &(string[length-17]));
         return;
      }

      halWait(100);
   } while((string[i+15] != '\0') && (i + 15) < length);
}

/******************************************************************************
* Internal function for lcd.c
******************************************************************************/
void smbSend(BYTE *buffer, const UINT8 n){
   UINT8 i = 0;

   smbStart();
   for(i = 0; i < n; i++){
      while(!smbSendByte(buffer[i])); //send until ACK received
   }
   smbStop();
}

/******************************************************************************
* Internal function for lcd.c
******************************************************************************/
BOOL smbSendByte(BYTE b){
   UINT8 i;
   BOOL ack;
   for (i = 0; i < 8; i++){
      smbWrite(b & 0x80);
      b = (b <<  1);
   }
   smbClock(0);
   DATA_HIGH();
   smbClock(1);
   ack = !LCD_SDA;
   return ack; //high = ACK received, else ACK not received
}

/******************************************************************************
* Internal function for lcd.c
* Function for writing bit to the data line. Setting the port as input
* make the SMBus go high because of the pull-up resistors.
*
******************************************************************************/
void smbWrite(BOOL value){
   smbClock(0);
   waitLCD();
   if(value){
      DATA_HIGH();
   }
   else{
      DATA_LOW();
   }
   smbClock(1);
   waitLCD();
}

/******************************************************************************
* Internal function for lcd.c
******************************************************************************/
void smbReceive(BYTE address, int length, BYTE *buffer){
   int i;

   smbStart();

   while (!smbSendByte(address)); //send address until ACK'ed

   for(i = 0; i < length; i++){
      buffer[i] = smbReceiveByte( i==(length-1) ); //TRUE if last byte
   }

   smbClock(1);
   smbWrite(0);

   smbStop();
}

/******************************************************************************
* Internal function for lcd.c
******************************************************************************/
BYTE smbReceiveByte(BOOL lastByte){
   int i;

   BYTE inData = 0;

   DATA_HIGH();

   for(i = 0; i < 8 * 2; i++){
      if(LCD_SCL == 0){ //clk low
         if(LCD_SDA){ //read '1'
            inData = (inData<<1) | 0x01;
         } else {
            inData = (inData<<1) & ~0x01;
         }
         waitLCD();
         smbClock(1);
      }
      else{
         waitLCD();
         smbClock(0);
      }
   }
   smbClock(0);

   //do not ACK if last byte
   lastByte ? smbWrite(1) :  smbWrite(0);

   smbClock(1);
   smbClock(0);

   return inData;
}


/******************************************************************************
* Internal function for lcd.c
*
* This function is used to clock the SMBus connected to the LCD. If a negative
* edge is to follow, the pin is set as an output and driven low. If a positive
* edge is to follow, the pin is set as an input and the pull-up resistor is
* to drive the node high. This way the slave device can hold the node low if
* a longer setup time is desired.
*
******************************************************************************/
void smbClock(BOOL value){
   if(!value)
   {
      IO_DIR_PORT_PIN(2, 0, IO_OUT);
      LCD_SCL = 0;
   }
   else {
      IO_DIR_PORT_PIN(2, 0, IO_IN);
   }
   waitLCD();
}

/******************************************************************************
* Internal function for lcd.c
*
* This function initiates SMBus communication. It makes sure that both the
* data and the clock of the SMBus are high. Then the data pin is set low
* while the clock is kept high. This initializes SMBus transfer.
******************************************************************************/
void smbStart(){
   while (! (LCD_SDA && LCD_SCL) ); //wait for Data and clk high
   DATA_LOW();
   waitLCD();
   smbClock(0);
}

/******************************************************************************
* Internal function for lcd.c
*
* This function terminates SMBus communication. It makes sure that the data
* and clock of the SMBus are low and high, respectively. Then the data pin is
* set high while the clock is kept high. This terminates SMBus transfer.
******************************************************************************/
void smbStop(){
   while (! (!LCD_SDA && LCD_SCL));
   smbClock(0);
   DATA_HIGH();
   waitLCD();
   smbClock(1);
}


/******************************************************************************
* Internal function for lcd.c
*
* This function does the timing of clk
******************************************************************************/
void waitLCD(){
   UINT8 i = 0x01;
   while(i--);
}
/******************************************************************************
* Internal function for lcd.c
*
*  BYTE LcdConvertChar(BYTE c)
*
*  DESCRIPTION:
*      Converts ASCII characters to the character set the LCD display uses
*      (works for all digits and letters and some other common characters)
******************************************************************************/
BYTE lcdConvertChar(BYTE c) {
   //character set R in data sheet
   if ((c >= 'a') && (c <= 'z')) //lower case
      c += ('a' - 0xE1);//0x80;
   else if ((c >= 'A') && (c <= 'Z')) //upper case
      c+= ('A' - 0xC1);
   else if ((c >= ' ') && (c <= '#') || (c >= '%') && (c <= '?'))
      c += (' ' - 0xA0);
   else{
      switch (c){
      case '$':
         c = 0x82;
         break;
      case '?:
         c = 0xDF;
         break;
      case '?:
         c = 0x81;
         break;
      case '@':
         c = 0x80;
         break;
      case '[':
         c = 0x8A;
         break;
      case ']':
         c = 0x54;
         break;
      case '_':
         c = 0x5A;
         break;
      case '?:
         c = 0x9D;
         break;
      case '?:
         c = 0x8C;
         break;
      case '?:
         c = 0x8F;
         break;
      case '?:
         c = 0x9C;
         break;
      case '?:
         c = 0x8B;
         break;
      case '?:
         c = 0x8E;
         break;
      case ARROW_LEFT:
         c = 0x10;
         break;
      case ARROW_RIGHT:
         c = 0x20;
         break;
      case ARROW_UP:
         c = 0x12;
         break;
      case ARROW_DOWN:
         c = 0x30;
         break;
      }
   }
   return c;
}

