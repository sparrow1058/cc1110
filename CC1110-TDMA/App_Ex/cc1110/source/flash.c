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

Filename:     flash.c
Target:       cc2510
Author:       efu
Revised:      20/6-2006
Revision:     1.0

Description:

    This example reads data from the UART and stores it in a flash address
    space. The data is written to the LCD. The next time this example is run,
    the previously stored data is fetched and written to the LCD. Both DMA and
    CPU flash write is used.

    NOTE: The flash supports only a limited number of write/erase cycles,
    therefore, this function should not be used many times, as this may wear
    out the flash page used in this example.

******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "app_ex.h"

// Prototypes
void writeFlashUsingDMA(BYTE* pSrcAddr, INT16 length, WORD flashAddress, BOOL erase);
BOOL flashUnused(BYTE* data, INT16 length);
void initFlash(void);
void flash_main(void);


#define STRING_LENGTH 30
#define PAGE_ADDRESS 0x7000
#define PAGE_NUMBER (PAGE_ADDRESS >> 10)


__no_init const char __code testData[2048] @ PAGE_ADDRESS;


volatile    DMA_DESC dmaChannel;

/******************************************************************************
* @fn  writeFlashUsingDMA
*
* @brief
*      Writes data to flash using DMA. Erases the page in advance if told to.
*
* Parameters:
*
* @param  BYTE* pSrcAddr
*         The start of the data to be written to flash.
*
*         INT16 length
*         The number of bytes to be written to flash.
*
*         WORD flashAddress
*         The address in flash the data is to be written to.
*
*         BOOL erase
*         Indicating whether the flash is to be erased or not.
*
* @return void
*
******************************************************************************/
void writeFlashUsingDMA(BYTE* pSrcAddr, INT16 length, WORD flashAddress, BOOL erase)
{
   BYTE buffer[10];

   INT_GLOBAL_ENABLE(INT_OFF);


   // Setting up the flash address,
   // erasing the page if required.
   SET_WORD(FADDRH, FADDRL, (int)(flashAddress >> 1));
   if(erase == TRUE)
   {
      halFlashErasePage(buffer, PAGE_NUMBER);
   }

   halWait(0xFF);

   // Making sure a multiplum of 4 bytes is transferred.
   while(length & 0x0003){
      length++;
   }


   SET_WORD(dmaChannel.SRCADDRH, dmaChannel.SRCADDRL,   pSrcAddr);   // The start address of the segment
   SET_WORD(dmaChannel.DESTADDRH, dmaChannel.DESTADDRL, &X_FWDATA);  // Input of the AES module
   SET_WORD(dmaChannel.LENH, dmaChannel.LENL, length);               // Setting the length of the transfer (bytes)
   dmaChannel.VLEN      = VLEN_USE_LEN;      // Using the length field
   dmaChannel.PRIORITY  = PRI_LOW;          // High priority
   dmaChannel.M8        = M8_USE_8_BITS;     // Transferring all 8 bits in each byte.
   dmaChannel.IRQMASK   = FALSE;             // The DMA complete interrupt flag is set at completion.
   dmaChannel.DESTINC   = DESTINC_0;         // The destination address is constant
   dmaChannel.SRCINC    = SRCINC_1;          // The address for data fetch is inremented by 1 byte
   dmaChannel.TRIG      = DMATRIG_FLASH;     // Setting the FLASH module to generate the DMA trigger
   dmaChannel.TMODE     = TMODE_SINGLE;      // A single byte is transferred each time.
   dmaChannel.WORDSIZE  = WORDSIZE_BYTE;     // Set to count bytes.

   // Setting up the DMA.
   // Clearing all DMA complete flags and arming the channel.
   DMA_SET_ADDR_DESC0(&dmaChannel);
   DMA_ABORT_CHANNEL(0);
   DMAIRQ &= ~DMA_CHANNEL_0;
   DMA_ARM_CHANNEL(0);

   asm("NOP");

   // Starting to write
   FLASH_CONFIG(WRITE);

   // Waiting for the DMA to finish.
   while(!(DMAIRQ & DMA_CHANNEL_0));
   DMAIRQ &= ~DMA_CHANNEL_0;

   return;
}




/******************************************************************************
* @fn  flashUnused
*
* @brief
*      Checks whether the given flash area is uninitialized (0xFF).
*
* Parameters:
*
* @param  BYTE* data
*         The start of the data to be investigated.
*
*         INT16 length
*         The number of bytes to be insvestigated.
*
* @return BOOL
*         Indicating if flash was unused (TRUE) or not (FALSE).
*
******************************************************************************/
BOOL flashUnused(BYTE* data, INT16 length)
{
   while((data[--length] == 0xFF) && (length));

   if(length == 0)return TRUE;
   else return FALSE;
}


/******************************************************************************
* @fn  initFlash
*
* @brief
*      Initializes components for use with the Flash application example.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void initFlash(void)
{
   initLcd();

   INIT_BUTTON();
   INIT_GLED();
   INIT_YLED();

}


/******************************************************************************
* @fn  flash_main
*
* @brief
*      Main function.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void flash_main(void){
#else
void main(void){
#endif
   BYTE buffer[30];
   char inputBuffer[STRING_LENGTH];
   INT8 pointer = 0;
   BOOL stop = FALSE;
   BOOL write = FALSE;
   char c;
   char *menuText[] = {(char*)" CPU write?", (char*)" DMA write?"};
   BYTE command;
   BOOL unUsed;

   initFlash();

   // Clearing buffers
   memset(buffer,0,sizeof(buffer));
   memset(inputBuffer,0,sizeof(inputBuffer));

   // Setting up UART
   UART_SETUP(0,57600,HIGH_STOP);
   UTX0IF = 1;  // Set UART 0 TX interrupt flag

   while(getJoystickDirection() != CENTRED);

   //Displaying the stored flash message.
   lcdUpdateLine(LINE1,(char*)"Last written:");
   if((unUsed = flashUnused((BYTE*)testData, STRING_LENGTH)))
   {
      lcdUpdateLine(LINE2,(char*)"Unused");
   }
   else
   {
      scrollText((char*) testData, STRING_LENGTH);
   }

   while(getJoystickDirection() != CENTRED);
   while(getJoystickDirection() == CENTRED);
   while(getJoystickDirection() != CENTRED);


   // User decides whether to use CPU or DMA to write flash or to abort.
   command = lcdMenu(menuText,2);
   if(command == ABORT_MENU)
   {
      return;
   }


   // Uart communication
   lcdUpdate((char*)"Enter UART", (char*)"data");
   printf((char*)"\n\nFlash Programming\n");


   printf((char*)"Press a key\n\n");
   uartGetkey (); // wait for a key to be pressed or the application to be ended
   if (stopApplication() ) return;
   else
   {
      inputBuffer[0] = U0DBUF;
      halWait(5);
      USART0_FLUSH();
      inputBuffer[1] = U0DBUF;
   }

   // Printing the previously written data
   printf((char*)"\nLast written:\n");
   if(unUsed)
   {
      printf((char*)"Unused\n");
   }
   else
   {
      printf((char*)"%s\n",&testData);
   }

   //Aquiring new data:
   printf((char*)"\n\nType data to be written.\nWill be printed to the LCD next time.");
   printf((char*)"\n(ENTER: store in flash, ESC: abort)\n\n");
   memset(inputBuffer,0,STRING_LENGTH);

   while(!stop)
   {
      c = getkey();
      U0DBUF = c;

      switch (c){
      case ENTER:
         inputBuffer[pointer] = 0;
         printf((char*)"\n\nTo write: %s\nENTER if OK.\n",inputBuffer);
         if(getkey() == ENTER)
         {
            // Write data to flash;
            stop = TRUE;
            write = TRUE;
         }
         else
         {
            // Reaquire data.
            printf((char*)"\nEnter text:\n");
            pointer = 0;
         }
         break;
      case BACK_SPACE:
         // Erasing the last typed data.
         if (pointer > 0)
         {
            pointer--;
            inputBuffer[pointer] = ' ';
         }
         break;
      case ESC:
         // Abort Flash write.
         stop = TRUE;
         write = FALSE;
         break;
      default:
         // Add typed data to buffer.
         if (pointer < STRING_LENGTH-1)
         {
            inputBuffer[pointer] = c;
            pointer++;
         }
         break;
      }
   }

  INT_GLOBAL_ENABLE(INT_OFF);

   // Updating the flash if asked to.
   if(write == TRUE)
   {
      if(command == 0)
      {
         halFlashWritePage((BYTE*) &inputBuffer, buffer, PAGE_NUMBER);
      }
      else
      {
         writeFlashUsingDMA((BYTE*) &inputBuffer, STRING_LENGTH, PAGE_ADDRESS, TRUE);
      }
      printf((char*)"\nUpdated:");
      printf((char*)" %s\n",(char __code*) (PAGE_NUMBER << 10));
      lcdUpdateLine(LINE1,(char*)"Updated");
   }
   else
   {
      printf((char*)"\nNot updated\n");
      lcdUpdateLine(LINE1,(char*)"Not updated");
   }
   lcdUpdateLine(LINE2,(char*)"LEFT to continue");


   // Done
   haltApplicationWithLED();

   return;
}


/******************************************************************************
* @fn  flash_init
*
* @brief
*      Initializes the Flash application example.
*
* Parameters:
*
* @param  APPLICATION *a
*         Main application
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void flash_init(APPLICATION *a)
{
   a->menuText = (char*)"Flash Writing";
   a->description = (char*)"57600 8-N-1";
   a->main_func = flash_main;
}
#endif
