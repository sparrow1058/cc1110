/******************************************************************************
Filename:     app_ex_main.c
Target:       cc2430
Revised:      16/12-2005
Revision:     1.0

Description:
    Application example, SmartRF04EB.

******************************************************************************/
#include "app_ex.h"
#include "RF04EB.h"
#include "hal.h"
#include "lcd128_64.h"
#include "string.h"
#include "menu.h"
#ifdef CC1110_TDMA_RX
#include "mydefine.h"
#endif


INT8U LanguageSel = 0;
//APPLICATION apps[NBR_OF_APPS];
UINT8 activeApp;

void keyinit(void)
{
	INIT_JOYSTICK();
	IO_DIR_PORT_PIN(2, 0, IO_IN);
	IO_DIR_PORT_PIN(0, 4, IO_IN);
	IO_DIR_PORT_PIN(0, 5, IO_IN);
}



/*********************************************************************************
//函数名：INT8U ScanKey(void)
//输入：无
//输出：无
//功能描述：按键扫描
*********************************************************************************/
INT8U ScanKey(void)
{
	INT8U tt;
	if(KEY_OK == 0) return K_OK;
        if(KEY_CANCEL == 0) return K_CANCEL;
        else
        {
                tt = halAdcSampleSingle(ADC_REF_AVDD, ADC_8_BIT, ADC_INPUT_JOYSTICK);
                if((tt > 0xA0 ) && (tt < 0xC5)) return K_UP;
                else if((tt < 0x8a) && (tt > 0x60)) return K_DOWN;
                else if((tt < 0x55) && (tt > 0x32)) return K_LIFT;
                else if(tt == 0) return K_RIGHT;
                else return 0xff;
        }
}


/******************************************************************************
* @fn  main
*
* @brief
*      Main function of application example.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
extern void Single_More_TDMA(char sell);

static void  delay(unsigned long i)
{
  unsigned long j;
  for(j = 0;j<i;j++);  for(j = 0;j<i;j++);  for(j = 0;j<i;j++);  for(j = 0;j<i;j++);
}

void main(void)
{

#ifdef CC1110_TDMA_RX
  unsigned char temp2[5] = {0x68,0xaa,'U','Y','J'};
#endif
  SET_MAIN_CLOCK_SOURCE(CRYSTAL);
  keyinit();
#ifdef CC1110_TDMA_RX
  Uart_int(57600);
  disp_open_wil();
  delay(50000);
  Print_words("时分多址通讯实验",0,11,8);
  Print_words("接收的数据：",2,0,6);
  Print_words("节点一：",4,0,4);
  Print_words("节点二：",4,72,4);
  UartSendWord(temp2,5);
  UartSendWord((void*)di_yz,sizeof(di_yz));
#endif
while(1)
  {
#ifdef CC1110_TDMA_RX
    Single_More_TDMA(TDMA_RX);
#endif
#ifdef CC1110_TDMA_TX
    Single_More_TDMA(TDMA_TX);
#endif
  }
}
/******************************************************************************
* @fn  stopApplication
*
* @brief
*     Check for stop application command, Joystick direction left.
*
* Parameters:
*
* @param  void
*
* @return BOOL
*         TRUE    Terminate current application
*         FALSE   No action
*
******************************************************************************/
BOOL stopApplication(void)
{
   return (ScanKey() == K_CANCEL);
}

/******************************************************************************
* @fn  haltApplication
*
* @brief
*     Halts until user stops application.
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void haltApplication(void)
{
   while(!stopApplication());
}


/******************************************************************************
* @fn  haltApplicationWithLED
*
* @brief
*     Halts until user stops application, toggle LEDs
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
void haltApplicationWithLED(void)
{
   BOOL direction = 0;
   BYTE led_values = 0x01;

   INIT_YLED();

   while(!stopApplication()){
      YLED = !!((led_values & 0x01));

      if(direction == 0)
         (led_values < 0x08) ? (led_values <<= 1):(led_values = 0x08, direction = 1);
      else
         (led_values > 0x01) ? (led_values >>= 1):(led_values = 0x01, direction = 0);

      halWait(40);
   }
}
