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

Filename:     aesEncrDecr.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/

#include "hal.h"
#include <stdio.h>
#include <string.h>

// Data can not be read out before AES is ready. Using a wait loop.
#define DELAY 0x0F


//-----------------------------------------------------------------------------
// See hal.h for a description of this function.
//-----------------------------------------------------------------------------
void halAesEncrDecr(BYTE *pDataIn, UINT16 length, BYTE *pDataOut, BYTE *pInitVector, BOOL decr){
   UINT16 i;
   UINT8 j, k;
   BYTE mode = 0;
   UINT16 nbrOfBlocks;
   UINT16 convertedBlock;
   UINT8  delay;
   nbrOfBlocks = length / 0x10;

   if((length % 0x10) != 0){
      // length not multiplum of 16, convert one block extra with zeropadding
      nbrOfBlocks++;
   }

   // Loading the IV.
   halAesLoadKeyOrInitVector(pInitVector, FALSE);

   // Starting either encryption or decryption
   if(decr){
      AES_SET_ENCR_DECR_KEY_IV(AES_DECRYPT);
   } else {
      AES_SET_ENCR_DECR_KEY_IV(AES_ENCRYPT);
   }

   // Getting the operation mode.
   mode = ENCCS & 0x70;

   for(convertedBlock = 0; convertedBlock < nbrOfBlocks; convertedBlock++){
      // Starting the conversion.
      AES_START();

      i = convertedBlock * 16;
      // Counter, Output Feedback and Cipher Feedback operates on 4 bytes and not 16 bytes.
      if((mode == CFB) || (mode == OFB) || (mode == CTR)){
         for(j = 0; j < 4; j++){
            // Writing the input data
            // Zeropadding the remainder of the block
            for(k = 0; k < 4; k++){
               ENCDI = ((i + 4*j + k < length) ? pDataIn[i + 4*j + k] : 0x00 );
            }
            // wait for data ready
            delay = DELAY;
            while(delay--);
            // Read out data for every 4th byte
            for(k = 0; k < 4; k++){
               pDataOut[i + 4*j + k] = ENCDO;
            }
         }
      }
      else if(mode == CBC_MAC){
         // Writing the input data
         // Zeropadding the remainder of the block
         for(j = 0; j < 16; j++){
            ENCDI = ((i + j < length) ? pDataIn[i + j] : 0x00 );
         }
         // The last block of the CBC-MAC is computed by using CBC mode.
         if(convertedBlock == nbrOfBlocks - 2){
            AES_SETMODE(CBC);
            delay = DELAY;
            while(delay--);
         }
         // The CBC-MAC does not produce an output on the n-1 first blocks
         // only the last block is read out
         else if(convertedBlock == nbrOfBlocks - 1){
            // wait for data ready
            delay = DELAY;
            while(delay--);
            for(j = 0; j < 16; j++){
               pDataOut[j] = ENCDO;
            }
         }
      }
      else{
         // Writing the input data
         // Zeropadding the remainder of the block
         for(j = 0; j < 16; j++){
            ENCDI = ((i+j < length) ? pDataIn[i+j] : 0x00 );
         }
         // wait for data ready
         delay = DELAY;
         while(delay--);
         // Read out data
         for(j = 0; j < 16; j++){
            pDataOut[i+j] = ENCDO;
         }
      }
   }
}

