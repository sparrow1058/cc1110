////////////////////////////////////////////////////////////////////////
/*
-----------------------------__WXL__-----------------------------------
-----------------------------@2007-12----------------------------------
----------------------------__by__SU__---------------------------------
*/
////////////////////////////////////////////////////////////////////////
//开一个计数器用作各种超时


#include"ioCC1110.h"
#include "mydefine.h"

#define uchar unsigned char
#define uint unsigned int
#define BYTE unsigned char
#define INT_ON   1
#define LEDt  0
#define LED P1
#define OPENLED(x) {P1SEL&= ~(1<<x);P1DIR |=1<<x ;LED&=~(1<<x);}
#define CLOSELED(x) {P1SEL&= ~(1<<x);P1DIR |=1<<x;LED|=(1<<x);}

#define TIME3_ISR_OPEN() (IEN1 |= 0x08)
#define TIMER3_RUN(value) (T3CTL = (value) ? T3CTL|0x10 : T3CTL&~0x10)
#define TICKSPD ((CLKCON & 0x38) >> 3)

#define ENABLE_ALL_INTERRUPT() (IEN0 |= 0x80)
#define DISABLE_ALL_INTERRUPT() (IEN0 &= ~0x80)

#define TIMER34_INIT(timer)   \
   do {                       \
      T##timer##CTL   = 0x06; \
      T##timer##CCTL0 = 0x00; \
      T##timer##CC0   = 0x00; \
      T##timer##CCTL1 = 0x00; \
      T##timer##CC1   = 0x00; \
   } while (0)
#define INT_ENABLE_T3(on)     { T3IE    = on; }
#define TIMER34_ENABLE_OVERFLOW_INT(timer,val) \
   (T##timer##CTL =  (val) ? T##timer##CTL | 0x08 : T##timer##CTL & ~0x08)
#define INT_GLOBAL_ENABLE(on) EA=(!!on)


extern uchar display[4];
unsigned int Timercount[4];//T3串口超时计数
void RestTimer(unsigned char timercount);
unsigned int ReadTimer(unsigned char timercount);
void init_T3(void);
uchar timer3_int(void);
void CPLDLED(uchar x);

void CPLDLED(uchar x)
{
  if(LED&(0x01<<x))
  {OPENLED(x) ;return;}
  if(!(LED&(0x01<<x)))
    CLOSELED(x);
  return;
}


void RestTimer(unsigned char timercount)
{
	Timercount[timercount&0x03] = 0;
}
unsigned int  ReadTimer(unsigned char timercount)
{
	return Timercount[timercount&0x03];
}

uchar timer3_int(void)//timer3 1000
{
  uint period;
  uchar div;
        period = ((1000*32) >> TICKSPD);// Determining how many timer ticks the period consist of
        while(period > 255)
                {              // If the period is too long, the prescaler division is
                        period = (period >> 1);   // increased.
                        div++;
                        if(div > 7)
                        {              // If the period is too long when using max prescaler division,
                                return 0;         // 0 is returned.
                        }
                }
                // Timer 3 selected
                T3CTL |= 0xe0;//(div << 5);              // Setting prescaler value
                T3CC0 = (BYTE) period;            // Setting timer value.
                return 1;
}
void init_T3(void)
{
   TIMER34_INIT(3);
   timer3_int();
   INT_ENABLE_T3(INT_ON);
   TIMER34_ENABLE_OVERFLOW_INT(3,INT_ON);
   INT_GLOBAL_ENABLE(INT_ON);
   TIMER3_RUN(1);//运行
   TIME3_ISR_OPEN();//开中断
}
#pragma vector = T3_VECTOR
 __interrupt void TIMER1_ISR(void){
 	 static unsigned char temp = 0;
 	if(++temp == 4)
 		{
 			temp = 0;
 		}
 		#ifdef END_DEVICE
 		disp_nixie_light(display[temp],temp);
 		#endif
   DISABLE_ALL_INTERRUPT();
   IRCON &= ~0x08;
   Timercount[0]++;
   Timercount[1]++;
   Timercount[2]++;
   if(Timercount[3]++ == UART_TIMER_OVER)//串口超时标志
   	if(Uart_S == RXING)
   	{
   		Uart_S = TIMEROVER;
   		Uart_R.Flag = true;
  	}
   ENABLE_ALL_INTERRUPT();
  }
