#include "ioCC1110.h"
#define LED1  P1_0
#define LED2  P1_1
#define KEY1  P1_3
#define KEY2  P1_2
 unsigned int counter=0;
 unsigned int tempFlag=0;
void Delay(unsigned char n)
{
  unsigned char i;
  unsigned int j;
  for(i=0;i<n;i++)
    for(j=1;j;j++);
}
void init(void)
{
  P1DIR|=0x03;
  P1DIR&=0xF3;  //KEY1 =p1_3  KEY2 =P1_2
  LED1=1;
  LED2=1;
  T1CTL=0x3D;     // 
  EA=1;
  IEN1=0x02;    //中断使能
  
}
void main(void)
{

  init();
  LED1=0;
  LED2=1;

 while(1)
 {
    if(tempFlag)
    {
      LED1=!LED1;
      LED2=!LED2;
      Delay(1);
    }
 }
}
#pragma vector =T1_VECTOR
__interrupt void UART0_IRS(void)
{
    IRCON=0x00;
    if(counter <10) counter++;
    else 
    {
      counter=0;
      tempFlag=!tempFlag;
    }
}

/* 查询timer中断方式 
   if(IRCON>0)
   {
     IRCON=0x00;
     if(counter<10)
       counter++;
     else
     {
       counter=0;
       tempFlag=!tempFlag;
     }
   }
    if(tempFlag)
    {
      LED1=!LED1;
      LED2=!LED2;
      Delay(1);
    }
   }
×/



      //For test;按键触发方式
  /* 
  while(1)
  {
    if(KEY1==0)
    {   
      Delay(3);
      LED1=!LED1;
      LED2=!LED2;
    }
   }
  */