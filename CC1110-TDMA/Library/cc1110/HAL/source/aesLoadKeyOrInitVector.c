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

Filename:     aesLoadKeyOrInitVector.c
Target:       cc2510
Author:       EFU
Revised:      20/6-2006
Revision:     1.0
******************************************************************************/
#include "hal.h"

//-----------------------------------------------------------------------------
// See hal.h for a description of this function.
//-----------------------------------------------------------------------------
void halAesLoadKeyOrInitVector(BYTE* pData, BOOL key){
   UINT8 i;

   // Checking whether to load a key or an initialisation vector.
   if(key){
      AES_SET_ENCR_DECR_KEY_IV(AES_LOAD_KEY);
   }
   else {
      AES_SET_ENCR_DECR_KEY_IV(AES_LOAD_IV);
   }
   // Starting loading of key or vector.
   AES_START();

   // loading the data (key or vector)
   for(i = 0; i < 16; i++){
      ENCDI = pData[i];
   }
   return;
}
