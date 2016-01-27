

#include "ioCC1110.h"
#include "rf_cc1110.h"
#include "tm1650.h"
#define uint unsigned int
#define uchar unsigned char
#define LED1	P1_0
#define LED2	P1_1
#define KEY1    P1_3
#define KEY2    P1_2
#define TM_LED1		0x68
#define TM_LED2		0x6A
#define MYADDR		0x01
void Delay(uint);
volatile INT8U len;
uchar CODE[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0~9显示代码 公阴
//uchar CODE[10] = {0xC0,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0~9显示代码 公阴
HDINFO	RFInfo;
uchar ReceiveFlag=0;
void Delay(uint n)
{
	uint i;
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
}

void ioInit()
{
	P1DIR=0x3B;
	P0SEL!=0x30;
	P0DIR|=0x30;	//set p0_2 P0_3 out put
	LED1=1;
    LED2=1;


}
void HandleCallInfo(INT8U *buffer)
{
	 RFInfo.Handle	=buffer[0]<<8+buffer[1];
	 RFInfo.Table	=buffer[2];
	 RFInfo.Type	=buffer[3];
	 RFInfo.Device	=buffer[4];
	 RFInfo.data[0]	=buffer[5];
	 RFInfo.data[1]	=buffer[6];
	 RFInfo.data[2]	=buffer[7];
	 RFInfo.CheckSum=buffer[8]<<8+buffer[9];
	

}
int main( void )
{
    INT8U buffer[10]={0,1,0x32,0x22,4,5,6,7,8,9};
	INT8U rssi, lqi;

    CLKCON &= ~0x40;              //晶振
    while(!(SLEEP & 0x40));      //等待晶振稳定
    CLKCON &= ~0x47;             //TICHSPD128分频，CLKSPD不分频
    SLEEP |= 0x04; 		 //关闭不用的RC振荡器
	ioInit();
    rf_cc1110_init( 433000 );
    IEN0 = 0x81;
	TM1650_Set(0);
	HandleCallInfo(buffer);
    while( 1 ){

	if(KEY2==0)
	{
		
		 //TM1650_Show(RFInfo.Table);
		if(ReceiveFlag==1)
		{
			ReceiveFlag=0;
			LED1=1;
			rf_send_packet((uchar *)&RFInfo, 10 );//发送应答信号
			
		}
	}
     len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
    
     if( len == 10 )
     {
		HandleCallInfo(buffer);
		TM1650_Show(RFInfo.Table);
		RFInfo.Device=MYADDR;
		ReceiveFlag=1;
		LED1=0;
		//rf_send_packet((uchar *)&RFInfo, 10 );//发送应答信号

	 }
     len = 0;
    }

  
  

}