////////////////////////////////////////////////////////////////////////
/*
-----------------------------__WXL__-----------------------------------
-----------------------------@2007-12----------------------------------
----------------------------__by__SU__---------------------------------
*/
////////////////////////////////////////////////////////////////////////

//本程序块实现通道的建立 部分解析
//2007-12-14 14:06:50

#include "mydefine.h"
#include "string.h"

void Uart_int(unsigned int baud);
void Timer_int(void);
Uart_State Chick_Uart_State(void);

void Uart_int(unsigned int baud)
{
  initUART(baud);
  UARTxISRopen();
  uart_state_int();
}
void Timer_int(void)
{
	init_T3();
}

Uart_State Chick_Uart_State(void)
{
	return Uart_S;
}












