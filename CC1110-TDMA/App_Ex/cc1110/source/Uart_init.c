/******************************************************************************
*                                                                             *
*    ****       ******       ****
*     ****     ********     ****
*      ****   ****  ****   ****
*       **** ****    **** ****              wxl
*        *******      *******
*         *****        *****                    成都无线龙通讯科技有限公司
*                                                                             *
*******************************************************************************

Filename:     uart.c                          文 件 名：      uart.c
Target:       cc1110                          使用芯片:       cc1110
Author:       WXL                             程 序 员：      无线龙
data:         1/12-2007                       日    期：      1/12-2007
******************************************************************************/
/**********************************头文件**************************************/
#include <Uart.h>
void initUART(void);

/*******************************************************************************
描述：设置选择主时钟

函数名：void SET_MAIN_CLOCK_SOURCE(int source)
*******************************************************************************/


void SET_MAIN_CLOCK_SOURCE(int source)
{
	  if(source)
	  {
        CLKCON |= 0x40;
        while(!HIGH_FREQUENCY_RC_OSC_STABLE);
        SLEEP |= 0x04;
      }
      else{							
        SLEEP &= ~0x04;
        while(!XOSC_STABLE);
        CLKCON &= ~0x47;
        SLEEP |= 0x04;
      }
}

/*******************************************************************************
描述：
    初始化串口
函数名：void initUART(void)
*******************************************************************************/


void initUART(void)
{
	// 初始化串口方向
	IO_PER_LOC_UART0_AT_PORT0_PIN2345();
	//选择晶振
	SET_MAIN_CLOCK_SOURCE(CRYSTAL);
	//设置57600-8-n-1
	UART_SETUP(0, 57600, HIGH_STOP);
	UTX0IF = 1;
}
