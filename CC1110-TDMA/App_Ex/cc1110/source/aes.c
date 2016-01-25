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

Filename:     aes.c
Target:       cc2510
Author:       KJA
Revised:      20/6-2006
Revision:     1.0

Description:

    This application show how to use the AES (Advanced Encryption Standard) module.

    The program first encrypts a short string, and then decrypts it afterwards using
    the HAL library. Then a longer string is encrypted and decrypted using DMA.

******************************************************************************/

#include "app_ex.h"
#include "cul.h"
#include <string.h>

#define STRING_LENGTH       16
#define STRING_LENGTH_DMA 2*16

#define LENGTH_IV  16
#define LENGTH_KEY 16

void aes_main(void);

//key an initialization vector (IV)
BYTE  key[LENGTH_KEY] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x11};
BYTE  IV[LENGTH_IV ] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


volatile BOOL dmaFinished;

void initAes(void);
void setFinished(void);
void aes(void);
void dmaAes(void);
void scrollText(char *string, UINT8 length);


/******************************************************************************
* @fn  initAes
*
* @brief
*      Initializes components for use with the AES encryption routine
*      application example (e.g. LED's, LCD, PushButton).
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void initAes(void)
{
   initLcd();

   SET_MAIN_CLOCK_SOURCE(CRYSTAL);

   INIT_BUTTON();
   INIT_GLED();
}


/******************************************************************************
* @fn  aes_main
*
* @brief
*      Main function of the AES encryption application example.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
#ifdef COMPLETE_APPLICATION
void aes_main(void){
#else
void main(void){
#endif
   initAes();

   GLED = LED_OFF;

   aes();

   dmaAes();

   haltApplicationWithLED();
   return;
}


/******************************************************************************
* @fn  aes
*
* @brief
*       This function encrypts and decrypts a string with use of the AES
*       module and the hal library
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void aes(void)
{
   char plainText[STRING_LENGTH];
   char cipherText[STRING_LENGTH];

   //clearing both strings
   memset(plainText, ' ', STRING_LENGTH);
   memset(cipherText, ' ', STRING_LENGTH);

   GLED = LED_ON;
   //Inserting, printing plaintext and waiting for input
   strcpy(plainText, (char*)"Chipcon AS");
   lcdUpdate((char*)"Plain:", plainText);
   while(getJoystickDirection() != CENTRED);
   while(getJoystickDirection() == CENTRED);


   // Setting the correct mode for AES and loading the key
   AES_SETMODE(CTR);
   halAesLoadKeyOrInitVector(key, TRUE);

   GLED = LED_OFF;


   //encrypt text and clearing plain text
   halAesEncrDecr((BYTE *)plainText, STRING_LENGTH, (BYTE *)cipherText, IV, ENCRYPT);
   memset(plainText, ' ', STRING_LENGTH);

   //printing ciphertext and waiting for user
   lcdUpdate((char*)"Cipher:", cipherText);
   while(getJoystickDirection() != CENTRED);
   while(getJoystickDirection() == CENTRED);

   GLED = LED_ON;

   //decrypt text, printing result and waiting for user
   halAesEncrDecr((BYTE *)cipherText, STRING_LENGTH, (BYTE *)plainText, IV, DECRYPT);
   lcdUpdate((char*)"Plain:", plainText);
   while(getJoystickDirection() != CENTRED);
   while(getJoystickDirection() == CENTRED);
   GLED = LED_OFF;

   return;
}


/******************************************************************************
* @fn  dmaAes
*
* @brief
*       This function encrypts and decrypts a string with use of the AES
*       module and the cul library. The function uses DMA.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void dmaAes(void)
{
   char plainText[STRING_LENGTH_DMA];
   char cipherText[STRING_LENGTH_DMA];

   UINT8 i;

   // Setting up DMA by requesting two channels
   //(one for transferring data to the AES, the other from the AES)
   DMA_DESC *dmaDescIn;
   DMA_DESC *dmaDescOut;
   UINT8 dmaDescInID;
   UINT8 dmaDescOutID;
   culDmaInit();
   dmaDescIn = culDmaAllocChannel(&dmaDescInID, setFinished);
   dmaDescOut = culDmaAllocChannel(&dmaDescOutID, setFinished);


   // clear text
   memset(plainText, ' ', STRING_LENGTH_DMA);
   memset(cipherText, ' ', STRING_LENGTH_DMA);

   GLED = LED_ON;

   // init with plain text, printing text and waiting
   strcpy(plainText, (char*)"The world is going wireless!!!");
   scrollText(plainText, STRING_LENGTH_DMA);
   while(getJoystickDirection() != CENTRED);
   while(getJoystickDirection() == CENTRED);


   // enable interupt from DMA and loading Initialisation Vector using DMA
   INT_ENABLE(INUM_DMA, INT_ON);
   INT_GLOBAL_ENABLE(INT_ON);
   culDmaToAes(dmaDescIn, (BYTE __xdata*)IV, LENGTH_IV, TRUE);
   DMA_ARM_CHANNEL(dmaDescInID);
   AES_SET_ENCR_DECR_KEY_IV(AES_LOAD_IV);
   AES_START();
   dmaFinished = FALSE;
   while (!dmaFinished);


   // Encrypting text. Setting up 2 DMA channels and starting transfer
   // Waiting until finished
   culDmaToAes(dmaDescIn, (BYTE __xdata*)plainText, STRING_LENGTH_DMA, FALSE);
   culDmaFromAes(dmaDescOut, (BYTE __xdata*)cipherText, STRING_LENGTH_DMA, TRUE);
   AES_SET_ENCR_DECR_KEY_IV(AES_ENCRYPT);
   DMA_ARM_CHANNEL(dmaDescInID);
   DMA_ARM_CHANNEL(dmaDescOutID);

   for(i = 0; i < STRING_LENGTH_DMA / 16; i++){
      while (! (ENCCS & 0x08)); //wait for AES ready
      AES_START();
   }

   dmaFinished = FALSE;
   while (!dmaFinished);

   GLED = LED_OFF;


   // clearing plain text
   memset(plainText, ' ', STRING_LENGTH_DMA);

   // Printing cipher text
   lcdUpdate((char*)"Cipher:", cipherText);
   scrollText(cipherText, STRING_LENGTH_DMA);
   while(getJoystickDirection() != CENTRED);
   while(getJoystickDirection() == CENTRED);


   // setup for encryption
   // load IV. Waiting until finished.
   GLED = LED_ON;
   culDmaToAes(dmaDescIn, (BYTE __xdata*)IV  , LENGTH_IV, TRUE);
   DMA_ARM_CHANNEL(dmaDescInID);
   AES_SET_ENCR_DECR_KEY_IV(AES_LOAD_IV);
   AES_START();
   dmaFinished = FALSE;
   while (!dmaFinished);



   // Decrypting text. Waiting until finished.
   culDmaToAes(dmaDescIn, (BYTE __xdata*)cipherText, STRING_LENGTH_DMA, FALSE);
   culDmaFromAes(dmaDescOut, (BYTE __xdata*)plainText, STRING_LENGTH_DMA, TRUE);
   AES_SET_ENCR_DECR_KEY_IV(AES_ENCRYPT);
   DMA_ARM_CHANNEL(dmaDescInID);
   DMA_ARM_CHANNEL(dmaDescOutID);

   for(i = 0; i < STRING_LENGTH_DMA / 16; i++){
      while (! (ENCCS & 0x08)); // wait for AES ready
      AES_START();
   }
   dmaFinished = FALSE;
   while (!dmaFinished);

   GLED = LED_ON;

   //Printing decrypted text
   lcdUpdate((char*)"Plain:", plainText);
   scrollText(plainText, STRING_LENGTH_DMA);
}


/******************************************************************************
* @fn  setFinished
*
* @brief
*       DMA callback routine.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void setFinished(void)
{
   dmaFinished = TRUE;
}

/******************************************************************************
* @fn  aes_init
*
* @brief
*       Initializes the AES encryption application example.
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
void aes_init(APPLICATION *a){
   a->menuText = (char*)"AES (Crypto)";
   a->description = (char*)"Enc./decr.";
   a->main_func = aes_main;
}
#endif

