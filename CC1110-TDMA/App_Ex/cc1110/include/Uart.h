#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <string.h>

#include <config.h>

#ifdef CC1110
#include "ioCC1110.h"
#endif
#ifdef CC2510
#include "ioCC2510.h"
#endif

/*******************************************************************************
描述：
    串口发送字符串函数
函数名：void UartTX_Send_String(char *data,int len)
data:数据
len :数据长度
*******************************************************************************/
extern void UartTX_Send_String(char *Data,int len);
/*******************************************************************************
描述：
    串口发送一个字符
函数名：void UartTX_Send_char(char Ch)
Ch:数据
*******************************************************************************/
extern void UartTX_Send_char(char Ch);
/*******************************************************************************
描述：
    串口接收一个字符
函数名：extern char UartRX_Receive_Char (void)
*******************************************************************************/
extern char UartRX_Receive_Char (void);
/*******************************************************************************
描述：
    初始化串口
函数名：extern void initUART(void)
*******************************************************************************/
extern void initUART(void);


#endif
