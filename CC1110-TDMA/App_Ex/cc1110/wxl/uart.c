////////////////////////////////////////////////////////////////////////
/*
-----------------------------__WXL__-----------------------------------
-----------------------------@2007-12----------------------------------
----------------------------__by__SU__---------------------------------
*/
////////////////////////////////////////////////////////////////////////

//完成CC1110/CC2510串口配置
//

#include"ioCC1110.h"
#include"mydefine.h"

Uart_Rx Uart_R; //串口接收数据 单元
Uart_State Uart_S;//串口接收数据状态
AF AF_Data;


#define ENABLE_ALL_INTERRUPT() (IEN0 |= 0x80)
#define DISABLE_ALL_INTERRUPT() (IEN0 &= ~0x80)
#define IO_PER_LOC_UART0_AT_PORT0_PIN2345() do { PERCFG = (PERCFG&~0x01)|0x00; } while (0)
#define uchar unsigned char
#define CRYSTAL 0x00
#define RC      0x01
#define HIGH_STOP                   0x02
#define LOW_STOP                    0x00
#define CLKSPD  (CLKCON & 0x07)
#define TRANSFER_MSB_FIRST          0x80
#define TRANSFER_MSB_LAST           0x00
#define UART_ENABLE_RECEIVE         0x40

#define UART_SETUP(uart, baudRate, options)      \
   do {                                          \
      if((uart) == 0){                           \
         if(PERCFG & 0x01){                      \
            P1SEL |= 0x30;                       \
         } else {                                \
            P0SEL |= 0x0C;                       \
         }                                       \
      }                                          \
      else {                                     \
         if(PERCFG & 0x02){                      \
            P1SEL |= 0xC0;                       \
         } else {                                \
            P0SEL |= 0x30;                       \
         }                                       \
      }                                          \
                                                 \
      U##uart##GCR = BAUD_E((baudRate),CLKSPD);  \
      U##uart##BAUD = BAUD_M(baudRate);          \
                                                 \
      U##uart##CSR |= 0x80;                      \
                                                 \
                                                 \
      U##uart##UCR |= ((options) | 0x80);        \
                                                 \
      if((options) & TRANSFER_MSB_FIRST){        \
         U##uart##GCR |= 0x20;                   \
      }                                          \
      U##uart##CSR |= 0x40;                      \
   } while(0)
#define SET_MAIN_CLOCK_SOURCE(source) \
   do {                               \
      if(source) {                    \
        CLKCON |= 0x40;               \
        while(!HIGH_FREQUENCY_RC_OSC_STABLE); \
        if(TICKSPD == 0){             \
          CLKCON |= 0x08;             \
        }                             \
        SLEEP |= 0x04;                \
      }                               \
      else {                          \
        SLEEP &= ~0x04;               \
        while(!XOSC_STABLE);          \
        asm("NOP");                   \
        CLKCON &= ~0x47;              \
        SLEEP |= 0x04;                \
      }                               \
   }while (0)
#define BAUD_E(baud, clkDivPow) (     \
    (baud==2400)   ?  6  +clkDivPow : \
    (baud==4800)   ?  7  +clkDivPow : \
    (baud==9600)   ?  8  +clkDivPow : \
    (baud==14400)  ?  9  +clkDivPow : \
    (baud==19200)  ?  9  +clkDivPow : \
    (baud==28800)  ?  10  +clkDivPow : \
    (baud==38400)  ?  10 +clkDivPow : \
    (baud==57600)  ?  11 +clkDivPow : \
    (baud==76800)  ?  11 +clkDivPow : \
    (baud==115200) ?  12 +clkDivPow : \
    (baud==153600) ?  0xaa +clkDivPow : \
    (baud==230400) ?  13 +clkDivPow : \
    (baud==307200) ?  0xaa +clkDivPow : \
    0  )


#define BAUD_M(baud) (      \
    (baud==2400)   ?  131  : \
    (baud==4800)   ?  131  : \
    (baud==9600)   ?  131  : \
    (baud==14400)  ?  34 : \
    (baud==19200)  ?  131  : \
    (baud==28800)  ?  34 : \
    (baud==38400)  ?  131  : \
    (baud==57600)  ?  34 : \
    (baud==76800)  ?  131  : \
    (baud==115200) ?  34 : \
    (baud==153600) ?  0xaa  : \
    (baud==230400) ?  164 : \
    (baud==307200) ?  0xaa  : \
  0)


#define XOSC_STABLE (SLEEP & 0x40)
#define TICKSPD ((CLKCON & 0x38) >> 3)
#define HIGH_FREQUENCY_RC_OSC_STABLE    (SLEEP & 0x20)

void ChangUartBaund(unsigned int temp);
void initUART(unsigned int baud);
void SET_UART0_ISR(void);
void uart_state_int(void);
void UartSendWord(uchar *msg,unsigned int lenth);
//*****************************************************************************
//串口初始化函数
//串口参数设置为57600，8,None,1,None
//打开串口，允许接收
//*****************************************************************************
void initUART(unsigned int baud)
{
   IO_PER_LOC_UART0_AT_PORT0_PIN2345();
   SET_MAIN_CLOCK_SOURCE(CRYSTAL);
   ChangUartBaund(baud);
   U0CSR = 0xC5;
}
void uart_state_int(void)
{
	Uart_R.count = false;
	Uart_R.Flag  = false;
	Uart_S = EMPTY;
}
//------------------------------------------------------------------------
//describe:chang band
//return:
//in:
//------------------------------------------------------------------------
void ChangUartBaund(unsigned int temp)
{
switch(temp)
{
case 2400:UART_SETUP(0, 2400, HIGH_STOP);break;
case 4800:UART_SETUP(0, 4800, HIGH_STOP);break;
case 9600:UART_SETUP(0, 9600, HIGH_STOP);break;
case 14400:UART_SETUP(0, 14400, HIGH_STOP);break;
case 19200:UART_SETUP(0, 19200, HIGH_STOP);break;
case 28800:UART_SETUP(0, 28800, HIGH_STOP);break;
case 38400:UART_SETUP(0, 38400, HIGH_STOP);break;
case 57600:UART_SETUP(0, 57600, HIGH_STOP);break;
//case 76800:UART_SETUP(0, 76800, HIGH_STOP);break;
//case 115200:UART_SETUP(0, 115200, HIGH_STOP);break;
default:  UART_SETUP(0, 9600, HIGH_STOP);break;
}
}
//example 9600;
//------------------------------------------------------------------------
//describe:open the uart isr
//return:
//in:
//------------------------------------------------------------------------
void SET_UART0_ISR(void)
{
   U0CSR |= 0xd0;
   U0UCR |= 0x02;
}
void UARTxISRopen(void)
{
  U0CSR |= 0x40;
  IEN0 |= 0X04;//开串口接收中断 'URX0IE = 1',
}
void UartSendWord(uchar *msg,unsigned int lenth)
{
	do{
		U0DBUF = *msg++;
		while(!UTX0IF);
		UTX0IF = 0;
	}while(--lenth);
}


#pragma vector = URX0_VECTOR
 __interrupt void URX0_ISR(void){
 	
   Uart_R.Buf[Uart_R.count++] = U0DBUF;

  if(((Uart_S == EMPTY)||(Uart_S == RXING))&&(Uart_R.count<= FRAMELENTH))//
   	{
   		Uart_S = RXING;
   		RestTimer(3);//   		
   	}
  else
   {
   	Uart_S = RXOVER;//收完   	
   	Uart_R.Flag = true;
  }
}
